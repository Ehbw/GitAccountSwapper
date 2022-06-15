#include "CredentialManager.h"
#include <string>
#include "pch.h"
#include "main.h"
#pragma comment(lib, "Advapi32.lib")

namespace CredentialManager {
	bool DoesCredentialExist(std::string url) {
		PCREDENTIALW creds;
		return CredRead(ConvertString(url), 1, 0, &creds);
	}

	void DeleteCredentials(std::string url) {
		CredDelete(ConvertString(url), 1, 0);
	}

	void Login(std::string url, std::string username, char* password)
	{
		if(CredentialManager::DoesCredentialExist(url))
		{
			CredentialManager::DeleteCredentials(url);
		}
		CREDENTIAL credentialToAdd = {0};
		credentialToAdd.Persist = CRED_PERSIST_LOCAL_MACHINE;
		credentialToAdd.TargetName = ConvertString(url);
		credentialToAdd.UserName = ConvertString(username);
		credentialToAdd.CredentialBlob = (LPBYTE) password;
		credentialToAdd.CredentialBlobSize = strlen(password) + 1;
		credentialToAdd.Type = CRED_TYPE_GENERIC;
		bool bRet = ::CredWrite(&credentialToAdd, 0);
		std::cout << bRet << std::endl;
	}

}

