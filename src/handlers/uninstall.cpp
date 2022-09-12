#include "uninstall.h"

#include "../program_manager.h"
#include "../gui.h"

#include <iostream>

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

namespace ActionHandlers::Uninstall
{
    std::vector<std::string> uninstalled_programs;

    void Actor(Action &action, std::array<intptr_t, 4> data_ptr)
    {
        //Clear any previous uninstall data
        uninstalled_programs.clear();

        //Uninstall SQL
        UninstallSQL();
        //Run it again, because the first time it excludes some MSSQL programs
        UninstallSQL();
    }

    bool UninstallSQL()
    {
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
                else if(uninstall_cmd.find("x64\\SetupARP.exe") != std::string::npos)
                {
                    ReplaceStringInPlace(uninstall_cmd, "x64\\SetupARP.exe", "Setup.exe");
                }
                
                if(uninstall_cmd.find("Setup.exe") != std::string::npos)
                {
                    //uninstall_cmd = uninstall_cmd + " /ACTION=UNINSTALL /FEATURES=SQL,AS,RS,IS,Tools,SQLENGINE,REPLICATION,FULLTEXT,CONN,IS,BC,SDK /INSTANCENAME=BRANEL /QUIET /IACCEPTSQLSERVERLICENSETERMS";
                    std::string config_file = std::filesystem::current_path().string() + "\\data\\ConfigurationFile_Uninstall.ini";

                    uninstall_cmd = uninstall_cmd + " /ConfigurationFile=" + config_file + " /IAcceptSQLServerLicenseTerms";
                }
                else if(uninstall_cmd.find("SSMS-Setup-ENU.exe") != std::string::npos)
                {
                    uninstall_cmd = uninstall_cmd + " /quiet /norestart";
                }

                std::cout << "Uninstall cmd: " << uninstall_cmd << std::endl;

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

        //Delete old SQL Server folders
        if(std::filesystem::exists("C:\\Program Files\\Microsoft SQL Server.old"))
        {
            std::filesystem::remove_all("C:\\Program Files\\Microsoft SQL Server.old");
        }
        if(std::filesystem::exists("C:\\Program Files (x86)\\Microsoft SQL Server.old"))
        {
            std::filesystem::remove_all("C:\\Program Files (x86)\\Microsoft SQL Server.old");
        }
        
        //Rename current SQL Server folders to old
        if(std::filesystem::exists("C:\\Program Files\\Microsoft SQL Server"))
        {
            std::filesystem::rename("C:\\Program Files\\Microsoft SQL Server", "C:\\Program Files\\Microsoft SQL Server.old");
        }
        if(std::filesystem::exists("C:\\Program Files (x86)\\Microsoft SQL Server"))
        {
            std::filesystem::rename("C:\\Program Files (x86)\\Microsoft SQL Server", "C:\\Program Files (x86)\\Microsoft SQL Server.old");
        }

        GUI().OpenPopup("Result", uninstalled_programs, "Uninstall");
    }
}