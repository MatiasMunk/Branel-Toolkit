#include "ProgramManager.h"
#include <algorithm>
#include <regex>

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

static std::wstring GetInstallerKeyNameFromGuid(std::wstring GuidName);
static void AddToList(std::vector<Software>* TheList, Software software);

Software::Software(std::wstring name, std::wstring version, std::wstring location, std::wstring uninstall, std::wstring icon, Arch arch): DisplayName(name), Version(version), InstallLocation(location), UninstallString(uninstall), Icon(icon), Architecture(arch)
{
}

ProgramManager::ProgramManager(void)
{
}

ProgramManager::~ProgramManager(void)
{
}

std::string ProgramManager::GetLatestInstallerDirectory(std::string path_to_search)
{
	std::vector<std::string> folders;
	for (auto& p : std::filesystem::directory_iterator(path_to_search))
	{
		if (p.is_directory())
		{
			std::string temp_folder(p.path().string());
			if(temp_folder.find("Install") != std::string::npos)
			{
				folders.push_back(p.path().string());
			}
		}
	}

	std::string most_recent_install_folder = "";
	if(!folders.empty())
	{
		most_recent_install_folder = folders.at(0);
		for(auto& folder : folders)
		{
			//String comparison works fine, if the format is the same.
			if(folder > most_recent_install_folder)
			{
				most_recent_install_folder = folder;
			}
		}
	}

	return most_recent_install_folder;
}

std::string ProgramManager::GetLatestInstaller(std::string path_to_search, std::string expected_installer_name, std::string optional)
{
	std::cout << "Searching for latest installer in " << path_to_search << std::endl;
	std::vector<std::string> software_installers;
	for(auto& p : std::filesystem::recursive_directory_iterator(path_to_search))
	{
		std::string temp(p.path().string());
		if(temp.find(expected_installer_name) != std::string::npos)
		{
			if(!optional.empty() && temp.find(optional) != std::string::npos)
			{
				software_installers.push_back(temp);
			}
			else
			{
				software_installers.push_back(temp);
			}
		}
	}

	std::string newest = software_installers.at(0);
	for(auto& folder : software_installers)
	{
		if(folder > newest)
		{
			newest = folder;
		}
	}

	return newest;
}

//Start process which takes std::string as the argument
bool ProgramManager::StartProcess(std::string cmd)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//Start the child process.
	if(!CreateProcessA(NULL,   // No module name (use command line)
		&cmd[0],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
	)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 1;
	}

	//Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	//Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}

std::vector<std::string> ProgramManager::UninstallMSSQL()
{
	std::vector<std::string> uninstalled_programs;

	std::vector<Software>* list = ProgramManager::GetInstalledPrograms(false);
	for(vector<Software>::iterator iter = list->begin(); iter!=list->end(); iter++)
	{
		if(iter->DisplayName.find(L"Compact") != std::string::npos)
		{
			continue;
		}
		else if((iter->DisplayName.find(L"SQL Server") != std::string::npos)
				|| (iter->DisplayName.find(L"T-SQL") != std::string::npos)
				|| (iter->DisplayName.find(L"ODBC Driver") != std::string::npos))
		{
			std::wcout << L"Found: " << iter->DisplayName << std::endl;
			if(iter->InstallLocation.empty())
			{
				std::wcout << L"Location: " << iter->Icon << std::endl;
			}
			else
			{
				std::wcout << L"Location: " << iter->InstallLocation << std::endl;
			}

			std::string uninstall_cmd(iter->UninstallString.begin(), iter->UninstallString.end());
			
			if(uninstall_cmd.find("MsiExec.exe /X") != std::string::npos)
			{
				ReplaceStringInPlace(uninstall_cmd, "MsiExec.exe /X", "MsiExec.exe /qn /X");
			}
			else if(uninstall_cmd.find("MsiExec.exe /I") != std::string::npos)
			{
				ReplaceStringInPlace(uninstall_cmd, "MsiExec.exe /I", "MsiExec.exe /qn /X");
			}
			else if(uninstall_cmd.find("x64\\SetupArp.exe") != std::string::npos)
			{
				ReplaceStringInPlace(uninstall_cmd, "x64\\SetupARP.exe", "Setup.exe");
			}
			
			if(uninstall_cmd.find("Setup.exe") != std::string::npos)
			{
				uninstall_cmd = uninstall_cmd + " /ACTION=UNINSTALL /FEATURES=SQL,AS,RS,IS,Tools,SQLENGINE,REPLICATION,FULLTEXT,CONN,IS,BC,SDK /INSTANCENAME=BRANEL /QUIET /IACCEPTSQLSERVERLICENSETERMS";
			}
			else if(uninstall_cmd.find("SSMS-Setup-ENU.exe") != std::string::npos)
			{
				uninstall_cmd = uninstall_cmd + " /quiet /norestart";
			}

			std::cout << "Uninstall command: " << uninstall_cmd << std::endl;

			//Start uninstall process from the installer and wait for it to finish
			//Returns 0 if successful
			if(ProgramManager::StartProcess(&uninstall_cmd[0]) == 0)
			{
				std::string uninstalled_program_name(iter->DisplayName.begin(), iter->DisplayName.end());

				uninstalled_programs.push_back(uninstalled_program_name);
			}
			else
			{
				std::wcout << "Failed to uninstall " << iter->DisplayName << std::endl;
			}
		}

		//Wait 500 milliseconds before starting the next uninstall process
		std::this_thread::sleep_for(std::chrono::milliseconds(500ms));
	}

	return uninstalled_programs;
}

std::vector<Software>* ProgramManager::GetInstalledPrograms(bool IncludeUpdates)
{
	return GetInstalledProgramsImp(IncludeUpdates);
}

std::vector<Software>* ProgramManager::GetInstalledProgramsImp(bool IncludeUpdates)
{
    std::vector<Software>* SoftwareList = new std::vector<Software>();
	
	RegistryKey* ClassesKey = RegistryKey::HKLM().OpenSubKey(L"Software\\Classes\\Installer\\Products");

	// The Classes\\Installer\\Products key is shared 
	// Documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/aa384253(v=vs.85).aspx

	RegistryKey* Wow64UninstallKey = RegistryKey::HKLM().OpenSubKey32(L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	SoftwareList = GetUninstallKeyPrograms(Wow64UninstallKey, ClassesKey, SoftwareList, IncludeUpdates);

	RegistryKey* UninstallKey = RegistryKey::HKLM().OpenSubKey64(L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	SoftwareList = GetUninstallKeyPrograms(UninstallKey, ClassesKey, SoftwareList, IncludeUpdates);

	std::vector<std::wstring> subkeys = RegistryKey::HKU().GetSubKeyNames();
	
	for(std::vector<std::wstring>::iterator it = subkeys.begin(); it!=subkeys.end(); it++)
	{
		// These Two Keys are Both Shared
		// Documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/aa384253(v=vs.85).aspx

		std::wstring uninstallsubs = (*it) + L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
		RegistryKey* UninstallKey = RegistryKey::HKU().OpenSubKey(uninstallsubs);
		SoftwareList = GetUninstallKeyPrograms(UninstallKey, ClassesKey, SoftwareList, IncludeUpdates);
		if(UninstallKey)
			delete UninstallKey;

		std::wstring installersubs = (*it) + L"\\Software\\Microsoft\\Installer\\Products";
		RegistryKey* InstallerKey = RegistryKey::HKU().OpenSubKey(installersubs);
		SoftwareList = GetUserInstallerKeyPrograms(InstallerKey, SoftwareList);
		if(InstallerKey)
			delete InstallerKey;
	}

	if(UninstallKey)
		delete UninstallKey;
	if(Wow64UninstallKey)
		delete Wow64UninstallKey;
	if(ClassesKey)
		delete ClassesKey;

	std::sort(SoftwareList->begin(), SoftwareList->end(), [](Software s1, Software s2)->bool{std::transform(s1.DisplayName.begin(), s1.DisplayName.end(), s1.DisplayName.begin(), ::tolower);std::transform(s2.DisplayName.begin(), s2.DisplayName.end(), s2.DisplayName.begin(), ::tolower);return s1.DisplayName.compare(s2.DisplayName)<0;} );
	
	return SoftwareList;
}

std::vector<Software>* ProgramManager::GetUserInstallerKeyPrograms(RegistryKey* uInstallerKey, std::vector<Software>* ExistingProgramList)
{
	if(uInstallerKey==NULL)
		return ExistingProgramList;

	RegistryKey * UserData32 = RegistryKey::HKLM().OpenSubKey32(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData");
	RegistryKey * UserData64 = RegistryKey::HKLM().OpenSubKey64(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData");

	if(UserData32==NULL && UserData64==NULL)
		return ExistingProgramList;

	RegistryKey* UserDatas[2] = {UserData32, UserData64};
	for(int two = 0; two < 2; two++)
	{
		RegistryKey * UserData = UserDatas[two];
		if(UserData==NULL)
			continue;

		std::vector<std::wstring> subkeys = uInstallerKey->GetSubKeyNames();
		for(std::vector<std::wstring>::iterator it = subkeys.begin(); it!=subkeys.end(); it++)
		{
			std::vector<std::wstring> userDataSubKeys = UserData->GetSubKeyNames();
			bool ProductFound = false;
			for(std::vector<std::wstring>::iterator userdatait = userDataSubKeys.begin(); userdatait!=userDataSubKeys.end(); userdatait++) 
			{
				//Ignore the LocalSystem account
				//Docs: https://docs.microsoft.com/en-us/windows/win32/secauthz/well-known-sids
				if (userdatait->compare(L"S-1-5-18")==0) {
					continue;
				}
			
				RegistryKey * ProductsKey = RegistryKey::HKLM().OpenSubKey(std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\").append(*userdatait).append(L"\\Products"),UnKnown);
                
				if ((ProductsKey != NULL)) 
				{
					std::vector<std::wstring> LmProductGuids = ProductsKey->GetSubKeyNames();
					for(std::vector<std::wstring>::iterator productit = LmProductGuids.begin(); productit!=LmProductGuids.end(); productit++) 
					{
						if (productit->compare(*it)==0) 
						{	
							RegistryKey * UserDataProgramKey = RegistryKey::HKLM().OpenSubKey(std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\").append(*userdatait).append(L"\\Products\\").append(*productit).append(L"\\InstallProperties"),UnKnown);
							if(UserDataProgramKey!=NULL)
							{
								if ( UserDataProgramKey->GetValue(L"SystemComponent").compare(L"")==0 || _wtoi(UserDataProgramKey->GetValue(L"SystemComponent").c_str())!=1 ) 
								{
									RegistryKey * temp = uInstallerKey->OpenSubKey(*it, uInstallerKey->KeyArch);

									// Name 
									std::wstring Name1 = temp->GetValue(L"ProductName");
									std::wstring Name2 = UserDataProgramKey->GetValue(L"DisplayName");
									std::wstring Name = L"";
									if(Name1.compare(L"")==0)
										Name = Name2;
									else
										Name = Name1;

									// Version
									std::wstring ProgVersion = UserDataProgramKey->GetValue(L"DisplayVersion");

									// InstallLocation
									std::wstring InstallLocation = UserDataProgramKey->GetValue(L"InstallLocation");

									// UninstallString
									std::wstring UninstallString = UserDataProgramKey->GetValue(L"UninstallString");

									// Icon
									std::wstring Icon1 = temp->GetValue(L"ProductIcon");
									std::wstring Icon2 = UserDataProgramKey->GetValue(L"DisplayIcon");
									std::wstring Icon = L"";
									if(Icon1.compare(L"")==0)
										Icon = Icon2;
									else
										Icon = Icon1;

									if ( Name.compare(L"")!=0 ) 
									{
										AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,UninstallString,Icon,UserData->KeyArch));
										ProductFound = true;
									}
									delete temp;
								}
								delete UserDataProgramKey;
							}
							break;
						}
					}
					if(ProductFound) 
					{
						break; // TODO: might not be correct. Was : Exit For
					}
					delete ProductsKey;
				}
			}
		}
		delete UserData;
	}
    return ExistingProgramList;
}

std::vector<Software>* ProgramManager::GetUninstallKeyPrograms(RegistryKey* UninstallKey, RegistryKey* ClassesKey, std::vector<Software>* ExistingProgramList, bool IncludeUpdates)
{
    //Make sure the key exists
    if (UninstallKey != NULL)
	{
        //Loop through all subkeys (each one represents an installed program)
		std::vector<std::wstring> UninstallSubKeys = UninstallKey->GetSubKeyNames();
		for(std::vector<std::wstring>::iterator SubKeyName = UninstallSubKeys.begin(); SubKeyName!=UninstallSubKeys.end(); SubKeyName++) 
		{
			RegistryKey * CurrentSubKey = UninstallKey->OpenSubKey(*SubKeyName, UninstallKey->KeyArch);
			if (CurrentSubKey == NULL)
				continue;

            int IsSystemComponent = 1;
            bool ErrorCheckingSystemComponent = false;
            
			std::wstring issystemcomponent = CurrentSubKey->GetValue(L"SystemComponent");
			if( issystemcomponent.compare(L"")==0 || _wtoi(issystemcomponent.c_str())!=1 )
				IsSystemComponent = 0;
            
            if (IsSystemComponent != 1) 
			{
                //If the WindowsInstaller flag is set then add the key name to our list of Windows Installer GUIDs
                if (CurrentSubKey->GetValue(L"WindowsInstaller").compare(L"")==0 || _wtoi(CurrentSubKey->GetValue(L"WindowsInstaller").c_str())!= 1) 
				{
                    std::wregex WindowsUpdateRegEx(L"KB[0-9]{6}$");
                    
					// Release Type
					std::wstring ProgramReleaseType = CurrentSubKey->GetValue(L"ReleaseType");

					// Version
                    std::wstring ProgVersion = CurrentSubKey->GetValue(L"DisplayVersion");
					// Name 
					std::wstring Name = CurrentSubKey->GetValue(L"DisplayName");
					// InstallLocation
					std::wstring InstallLocation = CurrentSubKey->GetValue(L"InstallLocation");
					// UninstallString
					std::wstring UninstallString = CurrentSubKey->GetValue(L"UninstallString");
                    // Icon
					std::wstring Icon = CurrentSubKey->GetValue(L"DisplayIcon");

					//Check to see if this program is classed as an update
					if (std::regex_match(*SubKeyName, WindowsUpdateRegEx) == true || \
						CurrentSubKey->GetValue(L"ParentKeyName").compare(L"") != 0 ||\
						ProgramReleaseType.compare(L"Security Update")==0 ||\
						ProgramReleaseType.compare(L"Update Rollup")==0 ||\
						ProgramReleaseType.compare(L"Hotfix")==0 )
					{
                        if (IncludeUpdates) 
						{
                            //Add the program to our list if we are including updates in this search
                            if ( Name.compare(L"")!=0 ) 
							{
                                AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,UninstallString,Icon,UninstallKey->KeyArch));
                            }
                        }
                    } 
					else 
					{
						std::wstring uninstallvalue = CurrentSubKey->GetValue(L"UninstallString");
						if (uninstallvalue.compare(L"")!=0) 
						{
                            if ( Name.compare(L"")!=0 ) 
							{
								AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,UninstallString,Icon,UninstallKey->KeyArch));
                            }
                        }
                    }
                } 

				//If WindowsInstaller
				else 
				{
                    // Name
					std::wstring Name1 = L"";
					std::wstring  Icon1 = L"";
                    std::wstring MsiKeyName = GetInstallerKeyNameFromGuid(*SubKeyName);
					RegistryKey * CrGuidKey = ClassesKey->OpenSubKey(MsiKeyName,ClassesKey->KeyArch);
                    if (CrGuidKey != NULL)
					{
                        Name1 = CrGuidKey->GetValue(L"ProductName");
						Icon1 = CrGuidKey->GetValue(L"ProductIcon");
						delete CrGuidKey;
                    }
					std::wstring Name2 = CurrentSubKey->GetValue(L"DisplayName");
					std::wstring Name = L"";
					if(Name1.compare(L"")==0)
						Name = Name2;
					else
						Name = Name1;

                    // Version
					std::wstring ProgVersion = CurrentSubKey->GetValue(L"DisplayVersion");

					// InstallLocation
					std::wstring InstallLocation = CurrentSubKey->GetValue(L"InstallLocation");

					// UninstallString
					std::wstring UninstallString = CurrentSubKey->GetValue(L"UninstallString");

					// Icon
					std::wstring Icon2 = CurrentSubKey->GetValue(L"DisplayIcon");
					std::wstring Icon = L"";
					if(Icon1.compare(L"")==0)
						Icon = Icon2;
					else
						Icon = Icon1;

					if (Name.compare(L"")!=0)
					{
                        AddToList(ExistingProgramList, Software(Name,ProgVersion,InstallLocation,UninstallString,Icon,UninstallKey->KeyArch));
                    }
                }
            }
        }
    }
    return ExistingProgramList;
}

// Util Functions
static void AddToList(std::vector<Software>* TheList, Software software)
{
	int index = -1;
	for(int i = 0; i<TheList->size(); i++)
	{
		if(TheList->at(i).DisplayName.compare(software.DisplayName)==0)
		{
			index = i;
			break;
		}
	}
	if(index == -1)
		TheList->push_back(software);
	else
	{
		Software duplicate = TheList->at(index);
		
		// Merge Architecture
		if( software.Architecture!=UnKnown && duplicate.Architecture!=UnKnown && duplicate.Architecture!=software.Architecture )
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if(software.Architecture==UnKnown)
				software.Architecture = duplicate.Architecture;
		}

		// Merge Icon
		if(software.Icon.compare(L"")!=0 && duplicate.Icon.compare(L"")!=0 && software.Icon.compare(duplicate.Icon)!=0)
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if(software.Icon.compare(L"")==0)
				software.Icon = duplicate.Icon;
		}

		// Merge Location
		if(software.InstallLocation.compare(L"")!=0 && duplicate.InstallLocation.compare(L"")!=0 && software.InstallLocation.compare(duplicate.InstallLocation)!=0)
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if(software.InstallLocation.compare(L"")==0)
				software.InstallLocation = duplicate.InstallLocation;
		}

		// Merge Version
		if(software.Version.compare(L"")!=0 && duplicate.Version.compare(L"")!=0 && software.Version.compare(duplicate.Version)!=0)
		{
			TheList->push_back(software);
			return;
		}
		else
		{
			if(software.Version.compare(L"")==0)
				software.Version = duplicate.Version;
		}
		TheList->erase(TheList->begin()+index);
		TheList->push_back(software);
	}
}

static std::wstring ReverseString(std::wstring input);
static void replaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to);
static std::vector<std::wstring> split( const std::wstring& text, wchar_t delimiter );

static std::wstring GetInstallerKeyNameFromGuid(std::wstring GuidName)
{
	replaceAll(GuidName, L"{", L"");
	replaceAll(GuidName, L"}", L"");
	std::vector<std::wstring> MsiNameParts = split(GuidName, L'-');
    std::wstring MsiName;
    //Just reverse the first 3 parts
    for (int i = 0; i <= 2; i++) {
        MsiName.append(ReverseString(MsiNameParts[i]));
    }
    //For the last 2 parts, reverse each character pair
    for (int j = 3; j <= 4; j++) {
        for (int i = 0; i <= MsiNameParts[j].length() - 1; i++) {
            MsiName.append(std::wstring(1,MsiNameParts[j].c_str()[i + 1]));
            MsiName.append(std::wstring(1,MsiNameParts[j].c_str()[i]));
            i += 1;
        }
    }
    return MsiName;
}

static std::wstring ReverseString(std::wstring input)
{
	std::reverse(input.begin(), input.end());
	return input;
}

static void replaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) 
{
    if(from.empty())
        return;
    std::size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::wstring::npos) 
	{
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

static std::vector<std::wstring> split( const std::wstring& text, wchar_t delimiter )
{
    std::vector<std::wstring> result;
 
    std::wstring::size_type start = 0;
    std::wstring::size_type end   = text.find( delimiter, start );
 
    while( end != std::wstring::npos )
    {
        std::wstring token = text.substr( start, end - start );
 
        result.push_back( token );
 
        start = end + 1;
        end   = text.find( delimiter, start );
    }
 
    result.push_back( text.substr( start ) );
 
    return result;
}