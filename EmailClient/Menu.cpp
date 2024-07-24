#include "Menu.h"

void ShowListReceivedMails(string username, string address, const filesystem::path& path) {

    // Check path exist
    if (!filesystem::exists(path) || !filesystem::is_directory(path)) {
        return;
    }
    map <string, bool> status = ReadStatusFromFile(path);
    size_t numberMails = status.size();
    int index = 0;
    int width0 = GetLengthNumber(numberMails);

    std::cout << "User: " << username << ' ' << address << "   |   " << "Number of mails: " << numberMails << "\n\n";
    for (int i = 0; i < width0 - 1; i++) {
        std::cout << ' ';
    }
    std::cout << "      Status   |                 Sender                             |"
       << "                         Subject                      |                  Time\n";
    for (int i = 0; i < Width3 - 1; i++) {
        std::cout << '_';
    }
    std::cout << '\n';

    // Traverse all message in current folder
    vector <string> fileMessName = ReadFilenameFromFile(path);
    for (string str: fileMessName) {
        Mail mail = GetInfoFromMsgFile(str + ".msg", path);
        // Print on console
        int len = GetLengthNumber(index + 1);
        std::cout << index + 1;
        for (int j = 0; j < width0 - len; j++) {
            std::cout << ' ';
        }
        std::cout << (status[str] ? " |  Da doc    |   " : " |  Chua doc  |   ");

        if (mail.sender.length() > Width1 - 4) {
            for (int j = 0; j < Width1 - 7; j++) {
                std::cout << mail.sender[j];
            }
            std::cout << "...";
        }
        else {
            std::cout << mail.sender;
            for (int j = 0; j < Width1 - 4 - mail.sender.length(); j++) {
                std::cout << ' ';
            }
        }
        std::cout << "   |   ";
        len = Width1 + 3;
        if (len + mail.subject.length() > Width2 - 4) {
            for (int j = 0; j < Width2 - 7 - len; j++) {
                std::cout << mail.subject[j];
            }
            std::cout << "...";
        }
        else {
            std::cout << mail.subject;
            for (int j = 0; j < Width2 - 4 - len - mail.subject.length(); j++) {
                std::cout << ' ';
            }
        }
        std::cout << "   |   Date: " << mail.date << '\n';
        index++;
    }
    return;
}

bool ShowMail(size_t mailNumber, const filesystem::path& path) {

    mailNumber--;
    vector <string> filename = ReadFilenameFromFile(path);
    if (mailNumber < 0 || mailNumber >= filename.size()) {
        return 1;
    }
    // Clear console
    system("cls");
    Mail mail = GetInfoFromMsgFile(filename[mailNumber] + ".msg", path);

    for (int i = 0; i < Width3 - 1; i++) cout << '_';
    std::cout << "\n         |";
    for (int i = 10; i < Width3 - 1; i++) cout << ' ';
    cout << '\n';
    std::cout << "Sender   |  " << mail.sender << "\n";
    std::cout << "_________|"; for (int i = 0; i < Width3 - 11; i++) std::cout << '_'; std::cout << "\n         |\n";
    std::cout << "Date     |  " << mail.date << "\n";
    std::cout << "_________|"; for (int i = 0; i < Width3 - 11; i++) std::cout << '_'; std::cout << "\n         |\n";
    std::cout << "Subject  | ";

    size_t cur = 12;
    size_t first = 0, last = 0;
    while (first < mail.subject.length()) {
        last = mail.subject.find_first_of(' ', first);
        if (last == -1) {
            last = mail.subject.length();
        }
        if (cur + (last - first) + 1 > Width3) {
            std::cout << "\n         |  " << mail.subject.substr(first, last - first);
            cur = 12 + last - first;
        }
        else {
            std::cout << ' ' << mail.subject.substr(first, last - first);
            cur += 1 + last - first;
        }
        first = last + 1;
    }
    std::cout << '\n';
    std::cout << "_________|"; for (int i = 0; i < Width3 - 11; i++) std::cout << '_'; std::cout << "\n         |\n";
    std::cout << "Content  | ";
    cur = 12;
    first = last = 0;
    while (first < mail.content.length()) {
        last = mail.content.find_first_of(' ', first);
        if (last == -1) {
            last = mail.content.length();
        }
        if (cur + (last - first) + 1 > Width3) {
            std::cout << "\n         |  " << mail.content.substr(first, last - first);
            cur = 12 + last - first;
        }
        else {
            std::cout << ' ' << mail.content.substr(first, last - first);
            cur += 1 + last - first;
        }
        first = last + 1;
    }
    std::cout << '\n';
    std::cout << "_________|"; for (int i = 0; i < Width3 - 11; i++) std::cout << '_'; std::cout << "\n\n";

    if (!mail.names.empty()) {
        std::cout << "Mail co " << mail.names.size() << " file dinh kem.\n";
        for (int i = 0; i < mail.names.size(); i++) {
            std::cout << mail.names[i] << '\n';
        }
        int chosen = 0;
        std::cout << "\nMail nay co file dinh kem. Ban cuon muon tai ve khong?\n";
        std::cout << "1, Co\n2, Khong\n";
        std::cout << "Lua chon cua ban la: ";
        std::cin >> chosen;
        if (chosen == 1) {
            do {
                std::cout << "Nhap duong dan ban muon luu: ";
                string path;
                cin >> path;
                filesystem::path filepath(path);
                if (CheckExist(filepath) == false) {
                    std::cout << "Duong dan khong hop le. Vui long nhap lai!\n";
                    continue;
                }
                SaveFile(mail.names, mail.fileData, filepath);
                cout << "Tai file dinh kem thanh cong.\n";
                for (int i = 0; i < 5e8; i++);
                break;
            } while (true);
        }
    }
    else {
        std::cout << "Mail khong co file dinh kem.";
    }
    // Mark mail has seen
    MarkMail(filename[mailNumber], path);
    return 0;
}

void Login() {

    Config config = ReadConfig("XMLFile.xml");
    string account, pass;
    do {
        cout << "Ten dang nhap: ";
        cin >> account;
        cout << "Mat khau: ";
        cin >> pass;
        if (account != config.address || pass != config.password) {
            cout << "Ten dang nhap hoac mat khau khong chinh xac.\nVui long nhap lai!\n\n";
            continue;
        }
        break;
    } while (true);
    return;
}

vector <string> AttachmentMenu() {

    vector <string> filePath;
    std::cout << "Ban co muon dinh kem file khong (1, Co ; 2, Khong) : ";
    int selection;
    std::cin >> selection;
    if (selection == 1) {
        do {
            int num = 0;
            bool overCapacity = false;
            std::cout << "So file ban muon gui: ";
            std::cin >> num;
            if (num < 0) {
                std::cout << "So dile dinh kem khong hop le\n";
            }
            if (num == 0) {
                break;
            }
            std::cout << "Duong dan toi file cua ban:\n ";
            for (int i = 0; i < num; i++) {
                size_t totalSize = 0;
                do {
                    std::cout << i + 1 << ". ";
                    string path;
                    std::cin >> path;
                    filesystem::path filepath(path);
                    if (CheckExist(filepath) == false) {
                        std::cout << "Duong dan ban nhap khong dung. Vui long nhap lai!\n";
                        continue;
                    }
                    // Check capacity
                    ifstream attachment(filepath, ios::binary);
                    if (attachment.is_open() == true) {

                        attachment.seekg(0, ios::end);
                        size_t fileSize = attachment.tellg();
                        totalSize += fileSize;
                    }
                    attachment.close();

                    if (totalSize > MAX_SIZE_ATTACHMENT) {
                        std::cout << "Tong dung luong file dinh kem qua lon.";
                        std::cout << "Ban chi co the dinh kem voi dung luong khong qua 3MB.\n";
                        std::cout << "Vui long nhap lai!";
                        overCapacity = true;
                        break;
                    }
                    filePath.push_back(path);
                    break;
                } while (true);
                if (overCapacity == true) {
                    break;
                }
            }
            if (overCapacity == true) {
                continue;
            }
            break;
        } while (true);
    }
    return filePath;
    
}

int SendMenu() {

    system("cls");
    string subject, content;
    vector <string> filePath, toList, ccList, bccList;

    std::cout << "Day la thong tin soan email (An Enter neu ban muon khong muon dien)\n";
    std::cout << "Cac dia chi nguoi nhan cach nhau boi mot khoang trang.\n";

    // Get recipient addresses
    do {
        std::cout << "To : ";
        toList = ReadInfo();
        std::cout << "Cc : ";
        ccList = ReadInfo();
        std::cout << "Bcc: ";
        bccList = ReadInfo();

        if (toList.size() + ccList.size() + bccList.size() == 0) {
            std::cout << "Ban can dien it nhat mot dia chi nguoi nhan. Vui long nhap lai!\n";
            continue;
        }
        break;
    } while (true);

    // Get subject, content
    std::cout << "Noi dung subject va content ket thuc bang cach an Enter.\n";
    std::cout << "Subject: ";
    vector <string> temp;
    temp = ReadInfo();
    for (string str : temp) {
        subject += " " + str;
    }
    subject.erase(0, 1);

    std::cout << "Content: ";
    temp = ReadInfo();
    for (string str : temp) {
        content += " " + str;
    }
    content.erase(0, 1);

    // Attach file
    filePath = AttachmentMenu();
    std::cout << "Ban da dien day du thong tin. Ban chac chan muon gui di (1, Co ; 2, Dien lai thong tin): ";
    int selection;
    std::cin >> selection;
    if (selection == 1) {
        SendMail(toList, ccList, bccList, subject, content, filePath);
    }
    for (int i = 0; i < 5e8; i++);
    return selection;
}

int SelectMailMenu(const filesystem::path& path) {

    Config config = ReadConfig("XMLFile.xml");
    do {
        system("cls");
        ShowListReceivedMails(config.username, config.address, path);
        cout << '\n';
        bool ok = 0;
        do {
            int selection = 0;
            std::cout << "Ban muon doc email thu may (hoac chon 0 de quay lai): ";
            std::cin >> selection;
            // Return main menu
            if (selection == 0) {
                return 2;
            }
            ok = ShowMail(selection, path);
            if (ok == true) {
                std::cout << "Email ban chon khong ton tai. Vui long chon lai\n";
            }
            else {
                std::cout << "\n\n";
                std::cout << "Chon 1 de quay lai danh sach\n";
                std::cout << "Chon 2 de xem email trong thu muc khac\n";
                std::cout << "Chon 3 de quay lai menu chinh\n";
                cout << "Lua chon cua ban la: ";
                int exit;
                std::cin >> exit;
                switch (exit) {
                    case 1:
                        break;
                    case 2: 
                        return exit;
                        break;
                    case 3:
                        return exit;
                        break;
                    default:
                        break;
                }
            }
        } while (ok != 0);
    } while (true);
    return 0;
}

void SelectFolderMenu() {

    Config config = ReadConfig("XMLFile.xml");
    ReceiveMail(config.username, config.password);

    do {
        system("cls");
        std::cout << "Day la cac thu muc trong mailbox ban:\n";
        std::cout << "1. Inbox\n";
        std::cout << "2. Project\n";
        std::cout << "3. Important\n";
        std::cout << "4. Work\n";
        std::cout << "5. Spam\n";
        std::cout << "6. Quay lai menu chinh\n";
        std::cout << "Ban muon xem trong thu muc nao: ";
        int selection = 0;
        std::cin >> selection;
        int temp;

        switch (selection) {
            case 1:
                temp = SelectMailMenu(GetPath() / "Inbox");
                break;
            case 2:
                temp = SelectMailMenu(GetPath() / "Project");
                break;
            case 3:
                temp = SelectMailMenu(GetPath() / "Important");
                break;
            case 4:
                temp = SelectMailMenu(GetPath() / "Work");
                break;
            case 5:
                temp = SelectMailMenu(GetPath() / "Spam");
                break;
            default:
                return;
                break;   
        }
        if (temp == 3) {
            return;
        }
    }
    while (true);
    return;
}

void Menu() {

    int temp;
    do {
        system("cls");
        std::cout << "Chon chuc nang ban muon thuc hien:\n";
        std::cout << "1. Gui email\n";
        std::cout << "2. Xem danh sach cac email\n";
        std::cout << "3. Thoat\n";
        std::cout << "Lua chon cua ban: ";
        int selection = 0;
        std::cin >> selection;
        switch (selection) {
            case 1:
                do {
                    temp = SendMenu();
                } while (temp == 2);
                break;
            case 2:
                SelectFolderMenu();
                break;
            case 3:
                return;
                break;
            default:
                break;
        }
    } 
    while (true);
    return;
}