#pragma once

#include <iostream>
#include <pugixml.hpp>
#include <string>

struct Config {
    string username;
    string address;
    string password;
    string mailServer;
    int smtpPort;
    int pop3Port;
    int autoLoad;
    string important;
    string work;
    string spam;
    string inbox;
};

Config ReadConfig(const string& file);