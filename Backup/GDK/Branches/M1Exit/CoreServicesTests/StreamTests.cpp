// Tests the various streams in the core services library
#include <stde/types.h>
#include <stde/com_ptr.h>
#include <CoreServices\FileStream.h>
#include <CoreServices\MemoryStream.h>
#include <CoreServices\SubStream.h>

#include "SubTest.h"

using namespace CoreServices;

static bool CreateNewFileStreamTest();
static bool CreateNewMemoryStreamTest();
static bool GrowableMemoryStreamTest();
static bool GrowableMemoryStreamChunkTest();
static bool AttachedMemoryStreamTest();
static bool CreateNewSubStreamTest();

static SubTest subTests[] =
{
    DefineSubTest(CreateNewFileStreamTest),
    DefineSubTest(CreateNewMemoryStreamTest),
    DefineSubTest(GrowableMemoryStreamTest),
    DefineSubTest(GrowableMemoryStreamChunkTest),
    DefineSubTest(AttachedMemoryStreamTest),
    DefineSubTest(CreateNewSubStreamTest),
};

bool StreamTests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}

bool CreateNewFileStreamTest()
{
    bool testPassed = false;
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;
    std::wstring path = L"teststreamfile.bin";
    byte b = 123;
    byte c = 0;
    unsigned long cbWritten = 0;
    unsigned long cbRead = 0;

    hr = FileStream::Create(path, false, &spStream);
    if (SUCCEEDED(hr))
    {
        hr = spStream->Write((void*)&b, sizeof(b), &cbWritten);
        if (SUCCEEDED(hr) && cbWritten == sizeof(b))
        {
            // close stream
            spStream = nullptr;

            // open previously written stream
            hr = FileStream::Create(path, true, &spStream);
            if (SUCCEEDED(hr))
            {
                hr = spStream->Read((void*)&c, sizeof(c), &cbRead);
                if (SUCCEEDED(hr) && cbRead == sizeof(c) && (c == b))
                {
                    // test passed
                    testPassed = true;
                }
            }
        }
    }

    // clean up dookies
    DeleteFile(path.c_str());

    return testPassed;
}

bool CreateNewMemoryStreamTest()
{
    bool testPassed = false;
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;
    byte b = 123;
    byte c = 0;
    unsigned long cbWritten = 0;
    unsigned long cbRead = 0;

    // Create a growable memory stream
    hr = MemoryStream::Create(sizeof(b), &spStream);
    if (SUCCEEDED(hr))
    {
        hr = spStream->Write((void*)&b, sizeof(b), &cbWritten);
        if (SUCCEEDED(hr) && cbWritten == sizeof(b))
        {
            // rewind stream to beginning
            hr = MemoryStream::Seek(spStream, STREAM_SEEK_SET, 0);
            if (SUCCEEDED(hr))
            {
                hr = spStream->Read((void*)&c, sizeof(c), &cbRead);
                if (SUCCEEDED(hr) && cbRead == sizeof(c) && (c == b))
                {
                    // test passed
                    testPassed = true;
                }
            }
        }
    }

    return testPassed;
}

bool GrowableMemoryStreamTest()
{
    bool testPassed = false;
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;
    byte b = 0;
    byte c = 0;
    unsigned long cbWritten = 0;
    unsigned long cbRead = 0;
    STATSTG stat = {0};

    // Create a growable memory stream ( 1 byte in length )
    hr = MemoryStream::Create(sizeof(b), &spStream);
    if (SUCCEEDED(hr))
    {
        hr = spStream->Stat(&stat, STGTY_STREAM);
        if (SUCCEEDED(hr) && stat.cbSize.LowPart == sizeof(b))
        {
            bool allDataWritten = true;
            // write 10 bytes of data (1 element at a time, causing massive reallocations)
            for (int i = 0; i < 10; i++)
            {
                b = (byte)i;

                hr = spStream->Write((void*)&b, sizeof(b), &cbWritten);
                if (SUCCEEDED(hr) && cbWritten == sizeof(b))
                {
                    
                }
                else
                {
                    allDataWritten = false;
                }
            }

            if (allDataWritten)
            {
                // Check that the newly allcated size matches what we wrote
                hr = spStream->Stat(&stat, STGTY_STREAM);
                if (SUCCEEDED(hr) && stat.cbSize.LowPart == (sizeof(b) * 10))
                {
                    // rewind stream to beginning
                    hr = MemoryStream::Seek(spStream, STREAM_SEEK_SET, 0);
                    if (SUCCEEDED(hr))
                    {
                        bool allDataIsValid = true;
                        for (int i = 0; i < 10; i++)
                        {
                            b = (byte)i;

                            hr = spStream->Read((void*)&c, sizeof(c), &cbRead);
                            if (SUCCEEDED(hr) && cbRead == sizeof(c) && (c == b))
                            {
                                // data is valid so far...
                            }
                            else
                            {
                                allDataIsValid = false;
                            }
                        }

                        testPassed = allDataIsValid;
                    }
                }
            }
        }
    }

    return testPassed;
}

bool GrowableMemoryStreamChunkTest()
{
    bool testPassed = false;
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;
    byte b = 0;
    byte c = 0;
    unsigned long cbWritten = 0;
    unsigned long cbRead = 0;
    STATSTG stat = {0};

    // Create a growable memory stream ( 0 byte in length )
    hr = MemoryStream::Create(0, &spStream);
    if (SUCCEEDED(hr))
    {
        hr = spStream->Stat(&stat, STGTY_STREAM);
        if (SUCCEEDED(hr) && stat.cbSize.LowPart == 0)
        {
            bool allDataWritten = true;
            byte data1[] = {0,1,2,3,4,5,6,7,8,9};
            byte data2[] = {10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
            // write 2 chunks of data (causing 2 reallocations)
            hr = spStream->Write((void*)data1, sizeof(data1), &cbWritten);
            if (SUCCEEDED(hr) && cbWritten == sizeof(data1))
            {
                // data chunk 1 written
            }
            else
            {
                allDataWritten = false;
            }
            
            hr = spStream->Write((void*)&data2, sizeof(data2), &cbWritten);
            if (SUCCEEDED(hr) && cbWritten == sizeof(data2))
            {
                // data chunk 2 written
            }
            else
            {
                allDataWritten = false;
            }

            if (allDataWritten)
            {
                // Check that the newly allcated size matches what we wrote
                hr = spStream->Stat(&stat, STGTY_STREAM);
                if (SUCCEEDED(hr) && stat.cbSize.LowPart == (sizeof(data1) + sizeof(data2)))
                {
                    // rewind stream to beginning
                    hr = MemoryStream::Seek(spStream, STREAM_SEEK_SET, 0);
                    if (SUCCEEDED(hr))
                    {
                        bool allDataIsValid = true;
                        for (int i = 0; i < 26; i++)
                        {
                            b = (byte)i;

                            hr = spStream->Read((void*)&c, sizeof(c), &cbRead);
                            if (SUCCEEDED(hr) && cbRead == sizeof(c) && (c == b))
                            {
                                // data is valid so far...
                            }
                            else
                            {
                                allDataIsValid = false;
                            }
                        }

                        testPassed = allDataIsValid;
                    }
                }
            }
        }
    }

    return testPassed;
}

bool AttachedMemoryStreamTest()
{
    bool testPassed = false;
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;
    byte b = 0;
    byte c = 0;
    unsigned long cbRead = 0;
    STATSTG stat = {0};
    byte data1[] = {0,1,2,3,4,5,6,7,8,9};

    // Create a growable memory stream ( 0 byte in length )
    hr = MemoryStream::Attach((void*)data1, sizeof(data1), &spStream);
    if (SUCCEEDED(hr))
    {
        hr = spStream->Stat(&stat, STGTY_STREAM);
        if (SUCCEEDED(hr) && stat.cbSize.LowPart == sizeof(data1))
        {
            // rewind stream to beginning
            hr = MemoryStream::Seek(spStream, STREAM_SEEK_SET, 0);
            if (SUCCEEDED(hr))
            {
                bool allDataIsValid = true;
                for (int i = 0; i < 10; i++)
                {
                    b = (byte)i;

                    hr = spStream->Read((void*)&c, sizeof(c), &cbRead);
                    if (SUCCEEDED(hr) && cbRead == sizeof(c) && (c == b))
                    {
                        // data is valid so far...
                    }
                    else
                    {
                        allDataIsValid = false;
                    }
                }

                testPassed = allDataIsValid;
            }
        }
    }

    return testPassed;
}

bool CreateNewSubStreamTest()
{
    bool testPassed = false;
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;
    stde::com_ptr<IStream> spSubStream;
    byte b = 0;
    byte c = 0;
    unsigned long cbRead = 0;
    STATSTG stat = {0};
    byte data1[] = {0,1,2,3,4,5,6,7,8,9};

    // Create a growable memory stream ( 0 byte in length )
    hr = MemoryStream::Attach((void*)data1, sizeof(data1), &spStream);
    if (SUCCEEDED(hr))
    {
        hr = spStream->Stat(&stat, STGTY_STREAM);
        if (SUCCEEDED(hr) && stat.cbSize.LowPart == sizeof(data1))
        {
            // seek to 2 bytes in
            hr = MemoryStream::Seek(spStream, STREAM_SEEK_SET, 2);
            if (SUCCEEDED(hr))
            {
                // create a sub stream that is 5 bytes in length from the current stream
                // that is already seeked 2 bytes in
                hr = SubStream::Create(spStream, 5, &spSubStream);
                if (SUCCEEDED(hr))
                {
                    hr = spSubStream->Stat(&stat, STGTY_STREAM);
                    if (SUCCEEDED(hr) && stat.cbSize.LowPart == 5)
                    {
                        bool allDataIsValid = true;
                        for (int i = 2; i < 7; i++)
                        {
                            b = (byte)i;

                            hr = spSubStream->Read((void*)&c, sizeof(c), &cbRead);
                            if (SUCCEEDED(hr) && cbRead == sizeof(c) && (c == b))
                            {
                                // data is valid so far...
                            }
                            else
                            {
                                allDataIsValid = false;
                            }
                        }

                        testPassed = allDataIsValid;
                    }
                }
            }
        }
    }

    return testPassed;
}