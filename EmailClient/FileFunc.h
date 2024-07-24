#pragma once

#include "Header.h"
#include "Config.h"
#include "StringFunc.h"

void InitFolder();
filesystem::path GetPath();
bool CheckExist(const filesystem::path&);
void CreateFolder(const filesystem::path&);
map <string, bool> ReadStatusFromFile(const filesystem::path&);
vector <string> ReadFilenameFromFile(const filesystem::path&);
void MarkMail(string, const filesystem::path&);
void UpdateStatusFile(string, const filesystem::path&);
void SaveMail(string data, string name, const filesystem::path&);
void SaveFile(const vector <string>&, const vector <string>&, const filesystem::path&);
Config ReadConfig(const std::string&);