#pragma once

#include "Header.h"
#include "SupportFunc.h"
#include "StringFunc.h"
#include "FileFunc.h"
#include "SendMailFunc.h"
#include "ReceiveMailFunc.h"

void ShowListReceivedMails(string, string, const filesystem::path&);
bool ShowMail(size_t, const filesystem::path&);
void Login();
int SendMenu();
int SelectMailMenu(const filesystem::path&);
void SelectFolderMenu();
void Menu();