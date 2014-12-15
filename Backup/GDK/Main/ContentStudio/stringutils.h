#pragma once

std::wstring FileFromFilePath(LPCWSTR szFilePath, BOOL includeExtensionInName);
std::wstring FilePathwithNewExtension(LPCWSTR szFilePath, LPCWSTR szExtension);
std::wstring FilePathOnly(LPCWSTR szFilePath);
std::wstring FilePathOnlyEx(LPCWSTR szFilePath);
std::wstring AnsiToWide(std::string s);
std::string WideToAnsi(std::wstring ws);
std::wstring FileExtensionOnly(LPCWSTR szExtension);
void GetExtensionsListFromFilteredExtensions(LPCWSTR szfilter, std::vector<std::wstring>& extensionsList);
void GetExtensionsListFromFilter(LPCWSTR szfilter, std::vector<std::wstring>& extensionsList);
void ConvertFilterWithEmbeddedNullsInitialFilterVersion(LPCWSTR szFilter, std::wstring& outFilter);


inline void trim_right(std::string &source, const std::string &t) {
source.erase(source.find_last_not_of(t)+1);
}

inline void trim_right(std::wstring &source, const std::wstring &t) {
source.erase(source.find_last_not_of(t)+1);
}

inline void trim_left(std::string &source, const std::string &t) {
source.erase(0, source.find_first_not_of(t));
}

inline void trim_left(std::wstring &source, const std::wstring &t) {
source.erase(0, source.find_first_not_of(t));
}

inline void trim(std::string &source, const std::string &t) {
trim_right(source, t); trim_left(source, t);
}

inline void trim(std::wstring &source, const std::wstring &t) {
trim_right(source, t); trim_left(source, t);
}

inline void trim_spaces(std::string &source) {
trim_right(source, " "); trim_left(source, " ");
}

inline void trim_spaces(std::wstring &source) {
trim_right(source, L" "); trim_left(source, L" ");
}

