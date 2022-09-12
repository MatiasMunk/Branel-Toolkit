#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shlobj.h>

#include "registry_key.h"

class Software
{
public:
	std::wstring DisplayName;
	std::wstring InstallLocation;
	std::wstring UninstallString;
	std::wstring Version;
	Arch Architecture; // 32 or 64
	std::wstring Icon;
	Software(std::wstring, std::wstring, std::wstring, std::wstring, std::wstring, Arch);
};

class ProgramManager
{
public:
	ProgramManager(void);
	~ProgramManager(void);

	static DWORD StartProcess(std::string cmd);

	static HRESULT CreateLink(LPCWSTR lpszPathObj1, LPCWSTR lpszPathLink, LPCWSTR lpszDesc, LPCWSTR lpszarg);

	static std::string GetLatestInstallerDirectory(std::string path_to_search);
	static std::string GetLatestInstaller(std::string path_to_search, std::string search_query, std::string include_in_search = "", std::vector<std::string> omit_files = {});

	static std::vector<Software>* GetInstalledPrograms(bool IncludeUpdates);

private:
	static std::vector<Software>* GetInstalledProgramsImp(bool IncludeUpdates);
	static std::vector<Software>* GetUninstallKeyPrograms(RegistryKey* UninstallKey, RegistryKey* ClassesKey, std::vector<Software>*, bool IncludeUpdates);
	static std::vector<Software>* GetUserInstallerKeyPrograms(RegistryKey* uInstallerKey, std::vector<Software>* ExistingProgramList);
};

#endif // PROGRAM_MANAGER_H