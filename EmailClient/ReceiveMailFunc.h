#pragma once

#include "Header.h"
#include "StringFunc.h"
#include "ConnectFunc.h"
#include "SupportFunc.h"
#include "Config.h"
#include "FileFunc.h"

string GetData(const SOCKET&, int, size_t);
void ReceiveMail(string, string);