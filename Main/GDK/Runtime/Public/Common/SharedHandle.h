#pragma once

#include <Platform.h>

namespace GDK
{
    template <typename THandle, void (*TCloseHandle)(THandle)>
    class SharedHandle
    {
    public:
        SharedHandle() :
            _block(nullptr)
        {
        }

        SharedHandle(THandle handle) :
            _block(new control_block)
        {
            _block->handle = handle;
            _block->refCount = 1;
        }

        SharedHandle(const SharedHandle& copied)
            : _block(copied._block)
        {
            InternalAddRef();
        }

        SharedHandle(SharedHandle&& moved)
            : _block(moved._block)
        {
            moved._block = nullptr;
        }

        ~SharedHandle()
        {
            InternalRelease();
        }

        SharedHandle& operator= (const SharedHandle& other)
        {
            InternalRelease();
            _block = other._block;
            InternalAddRef();
            return *this;
        }

        SharedHandle& operator= (THandle other)
        {
            InternalRelease();
            _block = new control_block;
            _block->handle = other;
            _block->refCount = 1;
            return *this;
        }

        THandle Get() const
        {
            if (_block != nullptr)
            {
                return _block->handle;
            }
            else
            {
                uint32_t invalid = 0xFFFFFFFF;
                return *reinterpret_cast<THandle*>(&invalid);
            }
        }

    private:
        void InternalAddRef()
        {
            if (_block != nullptr)
            {
                ++_block->refCount;
            }
        }

        void InternalRelease()
        {
            if (_block != nullptr)
            {
                if (--_block->refCount == 0)
                {
                    TCloseHandle(_block->handle);
                    memset(_block, 0, sizeof(control_block));
                    delete _block;
                }
                _block = nullptr;
            }
        }

        struct control_block
        {
            THandle handle;
            uint32_t refCount;
        };

        control_block* _block;
    };
}
