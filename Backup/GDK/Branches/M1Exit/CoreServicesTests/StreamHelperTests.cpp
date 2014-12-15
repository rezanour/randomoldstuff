#include "SubTest.h"
#include <CoreServices\MemoryStream.h>
#include <CoreServices\StreamHelper.h>

using namespace CoreServices;

namespace
{
    bool BasicReadWriteTest();

    SubTest subTests[] =
    {
        DefineSubTest(BasicReadWriteTest),
    };

    bool BasicReadWriteTest()
    {
        stde::com_ptr<IStream> spStream;

        if (FAILED(MemoryStream::Create(100, &spStream)))
            return false;

        size_t value = 12584;
        {
            StreamHelper writer(spStream);
            if (FAILED((writer.WriteValue(value, 4))))
                return false;

            if (FAILED((writer.WriteValue(value, 2))))
                return false;
        }

        LARGE_INTEGER pos;
        ULARGE_INTEGER newPos;
        pos.QuadPart = 0;
        if (FAILED(spStream->Seek(pos, STREAM_SEEK_SET, &newPos)))
            return false;

        size_t readValue = 0;
        {
            StreamHelper reader(spStream);
            if (FAILED((reader.ReadValue(readValue, 4))))
                return false;

            if (readValue != value)
                return false;

            if (FAILED((reader.ReadValue(readValue, 2))))
                return false;

            if (readValue != value)
                return false;
        }

        return true;
    }
}

bool StreamHelperTests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}
