#pragma once

std::wstring FileFromFilePath(LPCWSTR szFilePath, BOOL includeExtensionInName);
std::wstring FilePathwithNewExtension(LPCWSTR szFilePath, LPCWSTR szExtension);
std::wstring FilePathOnly(LPCWSTR szFilePath);
std::wstring AnsiToWide(std::string s);
std::string WideToAnsi(std::wstring ws);
