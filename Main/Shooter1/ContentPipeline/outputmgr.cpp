#include "precomp.h"
#include "outputmgr.h"

//
// Single file per output, no packing
//
class BasicOutputManager : public OutputManager
{
public:
    BasicOutputManager()
    {}

protected:
    HANDLE LockOutput(_In_z_ const wchar_t* name) override
    {
        FileHandle file(CreateFile((GetConfig()->DestRoot + name).c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
        CHECKGLE(file.IsValid());
        return file.Detach();
    }

    void UnlockOutput(HANDLE handle, uint32_t bytesWritten) override
    {
        // For the basic output manager, the bytes written should be
        // exactly the same as the file size
        DWORD size = GetFileSize(handle, nullptr);
        CHECKTRUE(size == bytesWritten);

        CloseHandle(handle);
    }
};

OutputManager* OutputManager::Create()
{
    return new BasicOutputManager();
}

OutputManager::OutputManager()
{
}

_Use_decl_annotations_
Output OutputManager::GetOutput(const wchar_t* name)
{
    return Output(this, LockOutput(name));
}

_Use_decl_annotations_
Output::Output(OutputManager* outputManager, HANDLE handle) :
    _output(outputManager), _handle(handle), _bytesWritten(0)
{
}

Output::Output(Output&& other) :
    _output(other._output), _handle(other._handle), _bytesWritten(other._bytesWritten)
{
    other._output = nullptr;
    other._handle = nullptr;
    other._bytesWritten = 0;
}

Output::~Output()
{
    if (_output != nullptr)
    {
        _output->UnlockOutput(_handle, _bytesWritten);
        _output = nullptr;
    }
    _handle = nullptr;
}

_Use_decl_annotations_
void Output::Write(void* p, uint32_t size)
{
    DWORD written = 0;
    CHECKGLE(WriteFile(_handle, p, size, &written, nullptr));
    CHECKTRUE(written == size);
    _bytesWritten += size;
}
