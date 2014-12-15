#pragma once

struct Output;

class OutputManager
{
public:
    // Create basic output mananger (Single file per output, no packing)
    static OutputManager* Create();

    Output GetOutput(_In_z_ const wchar_t* name);

protected:
    OutputManager();

    friend struct Output;
    virtual HANDLE LockOutput(_In_z_ const wchar_t* name) = 0;
    virtual void UnlockOutput(HANDLE handle, uint32_t bytesWritten) = 0;

private:
    OutputManager(const OutputManager&);
    OutputManager& operator= (const OutputManager&);
};

struct Output
{
    Output(Output&&);
    ~Output();

    void Write(_In_bytecount_(size) void* p, uint32_t size);

private:
    friend class OutputManager;
    Output(_In_ OutputManager* outputManager, HANDLE handle);

    // No copy
    Output(const Output&);
    Output& operator= (const Output&);

    OutputManager*  _output;
    HANDLE          _handle;
    uint32_t        _bytesWritten;
};
