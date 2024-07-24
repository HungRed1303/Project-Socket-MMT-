#pragma once

#include "Header.h"
#include "StringFunc.h"
#include "ConnectFunc.h"
#include "SupportFunc.h"
#include "Config.h"

string GetContentFormat(const string&);
string GetMimeFormat(const vector <string>&, const vector <string>&, const vector <string>&, const string&);
int SendMail(const vector <string>&, const vector <string>&, const vector <string>&, const string&, const string&, const vector <string>&);
int AttachFile(const SOCKET&, const vector <string>&, const vector <string>&, const vector <string>&, const string&, const string&, const vector <string>&);