#pragma once

#include "Header.h"
#include "Config.h"

wstring GetMimeTypeFromString(const wstring&);
string GetTime();
int GetLengthNumber(size_t);
vector <size_t> GetSizeOfMails(string, size_t);
string GenerateUUID();
vector <string> ReadInfo();