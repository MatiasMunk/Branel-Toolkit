#ifndef PROGRAMMANAGER_H
#define PROGRAMMANAGER_H

#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>
#include <Windows.h>

#include "RegistryKey.h"

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

	static std::string GetLatestInstallerDirectory(std::string path_to_search);
	static std::string GetLatestInstaller(std::string path_to_search, std::string expected_installer_name, std::string optional = "");

	static bool StartProcess(std::string cmd);

	static std::vector<std::string> UninstallMSSQL();

	static std::vector<Software>* GetInstalledPrograms(bool IncludeUpdates);

private:
	static std::vector<Software>* GetInstalledProgramsImp(bool IncludeUpdates);
	static std::vector<Software>* GetUninstallKeyPrograms(RegistryKey* UninstallKey, RegistryKey* ClassesKey, std::vector<Software>*, bool IncludeUpdates);
	static std::vector<Software>* GetUserInstallerKeyPrograms(RegistryKey* uInstallerKey, std::vector<Software>* ExistingProgramList);
};

#endif // PROGRAMMANAGER_H