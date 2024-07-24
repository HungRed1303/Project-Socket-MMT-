#pragma once

#include "Header.h"
#include "StringFunc.h"
#include "Config.h"

int CreateWinsock();
SOCKET CreateSocket();
void CheckSend(int);
void CheckReceive(int, char*);
int ConnectServer(const SOCKET&, const int, string);