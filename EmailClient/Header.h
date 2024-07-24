#pragma once

#include <iostream>
#include <algorithm>
#include <winsock2.h>
#include <vector>
#include <tchar.h>
#include <WS2tcpip.h>
#include <string>
#include <fstream>
#include <urlmon.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <pugixml.hpp>
#include <direct.h>
#include <Windows.h>
#include <filesystem>
#include <thread>
#include <map>
#include <chrono>
#include <mutex>

#pragma comment( lib, "urlmon" )
#pragma comment( lib, "rpcrt4.lib" )

using namespace std;

const int PACKET_SIZE = 72;
const int MAX_SIZE_ATTACHMENT = 3000000;
const int MAX_SIZE_BUFFER = 4096;
const int Width1 = 50;
const int Width2 = 105;
const int Width3 = 168;
const int AddMemory = 1000;

struct Mail {
	string sender, subject, date, content;
	vector <string> names;
	vector <string> fileData;
};
