#include "Header.h"
#include "ReceiveMailFunc.h"
#include "SendMailFunc.h"
#include "Config.h"
#include "Menu.h"
#include "FileFunc.h"
#include <condition_variable>

std::mutex shouldExitMutex;
std::condition_variable shouldExitCV;
bool shouldExit = false;

void AutoLoadMailPeriodically(const std::string& username, const std::string& password) {
    Config config = ReadConfig("XMLFile.xml");
    while (true) {
        {
            // Đồng bộ hóa thời gian chạy chương trình con
            std::unique_lock<std::mutex> lock(shouldExitMutex);
            if (shouldExitCV.wait_for(lock, std::chrono::seconds(config.autoLoad), [] { return shouldExit; })) {
                break;
            }
        }
        ReceiveMail(username, password);
    }
}

int main() {
    Config config = ReadConfig("XMLFile.xml");

    // Autoload email from Mailbox
    InitFolder();
    Login();
    thread mailThread(AutoLoadMailPeriodically, config.username, config.password);


    while (true) {
        Menu();
        {
            std::unique_lock<std::mutex> lock(shouldExitMutex);
            shouldExit = true;
        }
        shouldExitCV.notify_one();
        break;
    }

    mailThread.join();

    return 0;
}
