#pragma once
#include <string>
#include <Windows.h>
#include <WinCred.h>
namespace CredentialManager {
	void Login(std::string, std::string, char*);
}