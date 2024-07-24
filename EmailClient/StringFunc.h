#pragma once

#include "Header.h"
#include "FileFunc.h"

string Base64Encode(const char*, size_t);
string Base64Decode(string);
bool isBase64(unsigned char);
string GetFileExtension(const string&);
string GetFilename(const string&);
wstring StrToWstr(const string&);
string WstrToStr(wstring);
pair <size_t, size_t> GetDataFromSTAT(const char*);
string GetData(const SOCKET&, int, size_t);
Mail GetInfoFromMsgFile(string, const filesystem::path&);
string GetMessID(const string&);
bool ContainsKeyword(const std::string&, const std::string&);