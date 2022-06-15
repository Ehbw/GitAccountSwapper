#include "pch.h"
#include "main.h"
#include <tchar.h>
#include "CredentialManager.h"
#include <unordered_map>
#include <algorithm>

std::regex userRegex("(name|user)( =|=)( |)");
std::regex passRegex("(key|pass|password)( =|=)( |)");
std::regex urlRegex("(url)( =|=)( |)");

struct GitUser {
    std::string url;
    std::string user;
    std::string key;
};

void LOG(const char* message) {
    std::cout << message << std::endl;
}

LPWSTR ConvertString(const std::string& instr)
{
    int bufferlen = ::MultiByteToWideChar(0, 0, instr.c_str(), instr.size(), NULL, 0);

    if (bufferlen == 0)
    {
        return 0;
    }

    LPWSTR widestr = new WCHAR[bufferlen + 1];
    ::MultiByteToWideChar(0, 0, instr.c_str(), instr.size(), widestr, bufferlen);
    widestr[bufferlen] = 0;
    return widestr;
}

std::string Combine(const std::string& a, const std::string& b) {
    return a + b;
}

bool IsStringPath(const std::string& str) {
    std::smatch match;
    return std::regex_search(str, match, std::regex("[a-zA-Z]:((/|\\|)[a-zA-Z0-9]*(/|\\|))*"));
}

void ReadConfig(std::string& line, GitUser& user) {
    std::smatch match;
    if (std::regex_search(line, match, userRegex))
    {
        std::string usr = std::regex_replace(line, userRegex, "");
        std::transform(usr.begin(), usr.end(), usr.begin(), ::tolower);
        user.user = usr;
    }

    if (std::regex_search(line, match, passRegex))
    {
        user.key = std::regex_replace(line, passRegex, "");
    }

    if (std::regex_search(line, match, urlRegex)) 
    {
        user.url = std::regex_replace(line, urlRegex, "");
    }
}

int main(int argc, char *argv[])
{
    bool found = false;
    auto userProfileENV = std::getenv("USERPROFILE");
    if (userProfileENV == nullptr) 
    {
        LOG("'USERPROFILE' env does not exist");
        return 1;
    }

    if (IsStringPath(argv[(argc - 1)])) 
    {
        LOG("Invalid Paramater");
        return 1;
    }

    std::string account(argv[(argc - 1)]);
    std::transform(account.begin(), account.end(), account.begin(), ::tolower);

    std::ifstream accountsFile (Combine(userProfileENV, "/.gitAccounts"));
    while (accountsFile) 
    {
        std::string line;
        std::smatch match;
        std::getline(accountsFile, line);

        if (std::regex_search(line, match, std::regex("\"[a-zA-Z0-9]+\"")))
        {
            std::string accountName;
            for (auto& i : match) 
            {
                accountName = std::regex_replace(std::string(i), std::regex("\""), "");
            }

            if (accountName == account) {
                std::smatch userMatch, keyMatch;
                GitUser gitUser;

                std::getline(accountsFile, line);
                ReadConfig(line, gitUser);
                std::getline(accountsFile, line);
                ReadConfig(line, gitUser);
                std::getline(accountsFile, line);
                ReadConfig(line, gitUser);

                if (gitUser.url.empty())
                {
                    gitUser.url = "git:https://github.com";
                }
                found = true;
                CredentialManager::Login(gitUser.url, gitUser.user, const_cast<char*>(gitUser.key.c_str()));
                break;
            }
        }
    }

    if (!found) {
        LOG("No account with that name exists in config");
        return 1;
    }
    return 0;
}