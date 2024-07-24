#include "SupportFunc.h"

wstring GetMimeTypeFromString(const wstring& str) {

    // Source: https://stackoverflow.com/questions/20634666/get-a-mime-type-from-a-extension-in-c
    LPWSTR pwzMimeOut = NULL;
    HRESULT hr = FindMimeFromData(NULL, str.c_str(), NULL, 0, NULL, FMFD_URLASFILENAME, &pwzMimeOut, 0x0);
    if (SUCCEEDED(hr)) {
        wstring strResult(pwzMimeOut);
        // Despite the documentation stating to call operator delete, the
        // returned string must be cleaned up using CoTaskMemFree
        CoTaskMemFree(pwzMimeOut);
        return strResult;
    }

    return L"";
}

string GetTime() {

    // Source: Chat GPT
    const auto timeNow = std::chrono::system_clock::now();
    auto timePoint = std::chrono::system_clock::to_time_t(timeNow);
    char timeStr[100];
    ctime_s(timeStr, sizeof(timeStr), &timePoint);

    return string(timeStr);
}

int GetLengthNumber(size_t n) {

    int count = 0;
    while (n > 0) {
        count++;
        n /= 10;
    }
    return count;
}

vector <size_t> GetSizeOfMails(string data, size_t nMails) {

    vector <size_t> ret;
    size_t pos = 0, endLine = 0;

    for (int i = 0; i < nMails; i++) {
        pos = data.find(' ', endLine);
        endLine = data.find("\r\n", pos);
        int tmp = stoi(data.substr(pos, endLine - pos)) + AddMemory;
        ret.push_back(tmp);
    }
    return ret;
}

string GenerateUUID() {

    UUID uuid;
    UuidCreate(&uuid);
    RPC_CSTR str;
    UuidToStringA(&uuid, &str);
    string ret = reinterpret_cast<const char*>(str);
    RpcStringFreeA(&str);
    return ret;
}

vector <string> ReadInfo() {

    vector <string> ret;
    string temp;
    std::cin.ignore();
    while (true) {
        char c = std::cin.get();
        if (c == '\n') {
            std::cin.putback(c);
            break;
        }
        std::cin.putback(c);
        std::cin >> temp;
        ret.push_back(temp);
    }
    return ret;
}