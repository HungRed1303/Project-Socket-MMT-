#include "FileFunc.h"

filesystem::path GetPath() {

    filesystem::path projectPath = filesystem::current_path();
    return projectPath;
}

void InitFolder() {

    Config config = ReadConfig("XMLFile.xml");
    filesystem::path curPath = GetPath();
    CreateFolder(curPath / "Inbox");
    CreateFolder(curPath / "Project");
    CreateFolder(curPath / "Important");
    CreateFolder(curPath / "Work");
    CreateFolder(curPath / "Spam");

    if (CheckExist(curPath / "Inbox" / "buffer.txt") == false) {
        ofstream outfile1(curPath / "Inbox" / "buffer.txt");
        outfile1.close();
    }
    if (CheckExist(curPath / "Project" / "buffer.txt") == false) {
        ofstream outfile1(curPath / "Project" / "buffer.txt");
        outfile1.close();
    }
    if (CheckExist(curPath / "Important" / "buffer.txt") == false) {
        ofstream outfile2(curPath / "Important" / "buffer.txt");
        outfile2.close();
    }
    if (CheckExist(curPath / "Work" / "buffer.txt") == false) {
        ofstream outfile3(curPath / "Work" / "buffer.txt");
        outfile3.close();
    }
    if (CheckExist(curPath / "Spam" / "buffer.txt") == false) {
        ofstream outfile4(curPath / "Spam" / "buffer.txt");
        outfile4.close();
    }
}

bool CheckExist(const filesystem::path& path) {

    return filesystem::exists(path);
}

void CreateFolder(const filesystem::path& folderPath) {

    if (!CheckExist(folderPath)) {
        filesystem::create_directory(folderPath);
    }
}
// Get status seen and not seen in status file
map <string, bool> ReadStatusFromFile(const filesystem::path& path) {

    map <string, bool> status;
    ifstream read(path / "buffer.txt", ios::binary);
    if (read.is_open()) {
        string name;
        bool temp;
        while (read >> name) {
            read >> temp;
            status[name] = temp;
        }
    }
    read.close();
    return status;
}
// Get filename of message in status file
vector <string> ReadFilenameFromFile(const filesystem::path& path) {

    vector <string> nameList;
    ifstream read(path / "buffer.txt", ios::binary);
    if (read.is_open()) {
        string name;
        bool status;
        while (read >> name) {
            read >> status;
            nameList.push_back(name);
        }
    }
    read.close();
    return nameList;
}
// Mark mail has seen
void MarkMail(string filename, const filesystem::path& path) {

    vector <string> nameList;
    vector <bool> statusList;

    ifstream read(path / "buffer.txt", ios::in | ios::binary);
    if (read.is_open() == true) {
        string name;
        bool status;
        while (read >> name) {
            nameList.push_back(name);
            read >> status;
            statusList.push_back(status);
        }
    }
    read.close();

    // Find position need to update status
    for (int i = 0; i < nameList.size(); i++) {
        if (nameList[i] == filename) {
            statusList[i] = true;
            break;
        }
    }

    ofstream outfile(path / "buffer.txt", ios::out | ios::binary);
    if (outfile.is_open() == true) {
        for (int i = 0; i < nameList.size(); i++) {
            outfile << nameList[i] << ' ' << statusList[i] << '\n';
        }
    }
    outfile.close();
    return;
}
// Update status seen mail
void UpdateStatusFile(string filename, const filesystem::path& path) {

    vector <string> nameList;
    vector <bool> statusList;

    ifstream read(path / "buffer.txt", ios::in | ios::binary);
    if (read.is_open() == true) {
        string name;
        bool status;
        while (read >> name) {
            nameList.push_back(name);
            read >> status;
            statusList.push_back(status);
        }
    }
    read.close();

    // Check file exist
    for (string str : nameList) {
        if (str == filename) {
            // If file existed, exit
            return;
        }
    }

    ofstream outfile(path / "buffer.txt", ios::app | ios::binary);
    if (outfile.is_open() == true) {
        outfile << filename << ' ' << false << '\n';
    }
    outfile.close();
    return;
}

void SaveFile(const vector <string>& names, const vector <string>& fileData, const filesystem::path& path) {

    for (int i = 0; i < names.size(); i++) {
        ofstream outfile(path / names[i], ios::out | ios::binary);
        size_t start = 0;
        while (start < fileData[i].length()) {
            size_t size = min(PACKET_SIZE, fileData[i].length() - start);
            outfile << Base64Decode(fileData[i].substr(start, size));
            start += PACKET_SIZE + 1;  // \r\n size 2
        }
        outfile.close();
    }
}

void SaveMail(string data, string filename, const filesystem::path& path) {

    if (filename != "temp" && CheckExist(path / (filename + ".msg")) == true) {
        return;
    }
    // Delete +OK .... \r\n
    while (data[0] != '\n') {
        data.erase(0, 1);
    }
    data.erase(0, 1);
    ofstream outfile(path / (filename + ".msg"), ios::out | ios::binary);
    outfile << data;
    outfile.close();
    return;
}

Config ReadConfig(const string& file) {

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file.c_str());

    if (!result) {
        std::cerr << "Khong the doc file cau hinh XML: " << result.description() << std::endl;
        // Check error
    }
    Config config;
    auto configNode = doc.child("General");

    config.username = configNode.child_value("Username");
    config.address = configNode.child_value("Address");
    config.password = configNode.child_value("Password");
    config.mailServer = configNode.child_value("MailServer");
    config.smtpPort = configNode.child("SMTP").text().as_int();
    config.pop3Port = configNode.child("POP3").text().as_int();
    config.autoLoad = configNode.child("Autoload").text().as_int();
    config.important = configNode.child_value("Important");
    config.work = configNode.child_value("Work");
    config.spam = configNode.child_value("Spam");
    return config;
}