#include "Configuration.h"
#include "FileStream.h"
#include "Debug.h"

#include <stde\types.h>
#include <stde\conversion.h>
#include <strsafe.h>

using namespace CoreServices;
using namespace std;

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

HRESULT Configuration::Create(_Out_ ConfigurationPtr* pspConfiguration)
{
    HRESULT hr = S_OK;

    ISNOTNULL(pspConfiguration, E_POINTER);

    (*pspConfiguration).reset(new Configuration);
    ISNOTNULL(*pspConfiguration, E_OUTOFMEMORY);

EXIT
    return hr;
}

HRESULT Configuration::Load(_In_ const wstring& filePath, _Out_ ConfigurationPtr* pspConfiguration)
{
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;

    CHECKHR(FileStream::Create(filePath, true, &spStream));
    CHECKHR(Load(spStream, pspConfiguration));

EXIT
    return hr;
}

HRESULT Configuration::Load(_In_ stde::com_ptr<IStream> spStream, _Out_ ConfigurationPtr* pspConfiguration)
{
    HRESULT hr = S_OK;
    STATSTG stats = {0};
    static char szLine[512];

    ISNOTNULL(spStream, E_INVALIDARG);
    ISNOTNULL(pspConfiguration, E_POINTER);

    CHECKHR(spStream->Stat(&stats, STATFLAG_NONAME));
    {
        (*pspConfiguration).reset(new Configuration);

        ulong cbBuffer = static_cast<ulong>(stats.cbSize.QuadPart);
        unique_ptr<char, stde::array_deleter<char>> buffer(new char[cbBuffer]);

        ulong cbRead = 0;
        CHECKHR(spStream->Read(buffer.get(), cbBuffer, &cbRead));

        // Copy the whole thing into a stringstream for easier processing
        istringstream stream(buffer.get(), ios_base::in);
        while (!stream.eof())
        {
            stream.getline(szLine, _countof(szLine));

            std::string line(szLine);
            if (line.empty())
                continue;

            if (line[line.size()-1] == 13) // carriage return
                line.pop_back();

            size_t equals = line.find_first_of('=');
            if (equals != line.npos)
            {
                string name(line.substr(0, equals));
                string value(line.substr(equals+1));
                (*pspConfiguration)->SetStringValue(name, value);
            }
        }
    }

EXIT
    // if we failed, we should null out the config object to release it
    if (FAILED(hr) && pspConfiguration)
        (*pspConfiguration).reset();

    return hr;
}

HRESULT Configuration::Save(_In_ const wstring& filePath)
{
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;

    CHECKHR(FileStream::Create(filePath, false, &spStream));
    CHECKHR(Save(spStream));

EXIT
    return hr;
}

HRESULT Configuration::Save(_In_ stde::com_ptr<IStream> spStream)
{
    HRESULT hr = S_OK;

    ISNOTNULL(spStream, E_INVALIDARG);

    static char szLine[512];
    size_t cchLine = 0;
    ulong cbWritten = 0;
    for (SettingMap::const_iterator it = _settings.begin(); it != _settings.end(); ++it)
    {
        CHECKHR(StringCchPrintfA(szLine, _countof(szLine), "%s=%s\n", it->first.c_str(), it->second.c_str()));
        CHECKHR(StringCchLengthA(szLine, _countof(szLine), &cchLine));
        CHECKHR(spStream->Write(szLine, static_cast<ulong>(cchLine * sizeof(szLine[0])), &cbWritten));
    }

EXIT
    return hr;
}

size_t Configuration::GetCount() const
{
    return _settings.size();
}

std::string Configuration::GetValueNameAt(_In_ size_t index) const
{
    if (index < GetCount())
    {
        SettingMap::const_iterator it = _settings.cbegin();
        for (size_t i = 0; i < index; i++)
        {
            it++;
        }

        return it->first;
    }

    return "";
}

string Configuration::GetStringValue(_In_ const string& name, _In_ const char* defaultValue) const
{
    SettingMap::const_iterator it = _settings.find(name);
    return (it != _settings.cend()) ? it->second : defaultValue;
}

void Configuration::SetStringValue(_In_ const string& name, _In_ const string& value)
{
    _settings[name] = value;
}


