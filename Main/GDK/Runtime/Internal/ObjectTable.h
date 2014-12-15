#pragma once

#include <Platform.h>
#include <GDKError.h>
#include <cassert>

namespace GDK
{
    template <typename THandle, typename TObject, void (*TInitialize)(TObject*), void (*TCleanup)(TObject*)>
    class ObjectTable;

    class ObjectTableInternal
    {
        template <typename THandle, typename TObject, void (*TInitialize)(TObject*), void (*TCleanup)(TObject*)>
        friend class ObjectTable;

        static uint8_t _nextId;
    };

    template <typename THandle, typename TObject, void (*TInitialize)(TObject*), void (*TCleanup)(TObject*)>
    class ObjectTable
    {
        static_assert(std::is_pod<TObject>::value, "TObject must be POD struct");
        static_assert(sizeof(THandle) == sizeof(uint32_t), "handles must be 32bits");

        struct trackedobject_t
        {
            TObject object;
            uint8_t revision;
            bool valid;
        };

    public:

        ObjectTable(uint32_t initialSize) :
            _id(ObjectTableInternal::_nextId++), _entries(initialSize), _numEntries(0), _highWatermark(0)
        {
        }

        ~ObjectTable()
        {
            auto entry = _entries.data();
            auto count = _entries.size();
            while (count-- > 0)
            {
                if (entry->valid)
                {
                    TCleanup(&entry->object);
                }
            }
        }

        bool IsValidHandle(THandle handle) const
        {
            uint32_t value = *reinterpret_cast<uint32_t*>(&handle);
            uint32_t index = value & 0x0000FFFF;
            uint8_t tableId = static_cast<uint8_t>(value >> 24);
            uint8_t revision = static_cast<uint8_t>((value << 8) >> 24);

            return (value != 0xFFFFFFFF && 
                    index < _highWatermark &&
                    _entries[index].valid && 
                    _id == tableId && 
                    _entries[index].revision == revision);
        }

        static THandle InvalidHandleValue()
        {
            static THandle handle = { 0xFFFFFFFF };
            return handle;
        }

        uint32_t GetNumObjects() const { return _numEntries; }

        THandle New()
        {
            uint32_t index = _highWatermark;
            if (_freeIndices.size() > 0)
            {
                index = _freeIndices[_freeIndices.size() - 1];
                _freeIndices.pop_back();
            }

            if (index == _highWatermark)
            {
                if (++_highWatermark >= _entries.size())
                {
                    _entries.resize(_entries.size() * 2);
                }
            }

            trackedobject_t* t = &_entries[index];
            TInitialize(&t->object);

            ++_numEntries;
            t->valid = true;
            return MakeHandle(t->revision, index);
        }

        void Delete(THandle handle)
        {
            if (!IsValidHandle(handle))
            {
                assert(false);
                return;
            }

            uint32_t value = *reinterpret_cast<uint32_t*>(&handle);
            uint32_t index = value & 0x0000FFFF;
            trackedobject_t* t = &_entries[index];

            TCleanup(&t->object);
            t->valid = false;
            ++t->revision;

            --_numEntries;
            _freeIndices.push_back(index);
        }

        TObject* Get(THandle handle)
        {
            if (!IsValidHandle(handle))
            {
                assert(false);
                return nullptr;
            }

            uint32_t value = *reinterpret_cast<uint32_t*>(&handle);
            uint32_t index = value & 0x0000FFFF;
            trackedobject_t* t = &_entries[index];
            return &t->object;
        }

    private:
        THandle MakeHandle(uint8_t revision, uint32_t index) const
        {
            assert(index < USHRT_MAX);
            THandle handle = { (static_cast<uint32_t>(_id) << 24) | (static_cast<uint32_t>(revision) << 16) | index };
            return handle;
        }

        uint8_t _id;
        uint32_t _numEntries;
        uint32_t _highWatermark;
        std::vector<trackedobject_t> _entries;
        std::vector<uint32_t> _freeIndices;
    };
}
