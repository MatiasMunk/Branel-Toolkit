#include "install.h"

#include "../application.h"
#include "../program_manager.h"
#include "../gui.h"

#include <iostream>
#include <thread>
#include <future>
#include <system_error>

enum ErrorCodes
{
    FILE_NOT_FOUND = 2L,
    ELEVATION_REQUIRED = 740L
};

namespace ActionHandlers::Install
{
    Application* application;

    std::string installer_directory;
    std::vector<std::string> omit_files;

    unsigned char program = 0;
    std::string program_name = "";
    std::string search_query = "";
    std::string include_in_search = "";
    std::string install_args = "";

    void Actor(Action &action, std::array<intptr_t, 4> data_ptr)
    {
        application = (Application*)data_ptr[0];

        action >> program;
        action >> program_name;
        action >> search_query;
        action >> include_in_search;
        action >> install_args;

        //Clear any previous install data
        application->modified_programs.clear();
        omit_files.clear();

        installer_directory = ProgramManager::GetLatestInstallerDirectory("C:\\Branel\\");
        std::cout << "Installer directory: " << installer_directory << std::endl;

        if(!installer_directory.empty())
        {
            switch((Program)program)
            {
                case Program::PROGRAM_SQLSERVER19:
                {
                    omit_files.push_back("SETUP.EXE.CONFIG");
                    omit_files.push_back("RSETUP.EXE");

                    std::string config_file = std::filesystem::current_path().string() + "\\data\\ConfigurationFile_Install.ini";

                    //The service account name is different depending on the operating system's language.
                    //If Danish, it's specified in the configuration file
                    //If English, it's specified and thus overwritten in the command line
                    //Workaround hack to determine Windows system language for the SQL Agent Account
                    NetUserManager user_manager;
                    if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Brugere")) == 0)
                    {
                        install_args = " /SAPWD=Br5015edt /ConfigurationFile=" + config_file + " /IAcceptSQLServerLicenseTerms";
                    }
                    else if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Users")) == 0)
                    {
                        install_args = " /SAPWD=Br5015edt /ConfigurationFile=" + config_file + " /AGTSVCACCOUNT=NT AUTHORITY\\NETWORKSERVICE /IAcceptSQLServerLicenseTerms";
                    }
                    break;
                }
                default:
                {
                    break; // Compiler warning
                }
            }

            std::thread install(&InstallGeneric, program_name, install_args, search_query, include_in_search, omit_files);
            install.detach();
        }
    }

    void InstallGeneric(std::string program_name, std::string install_args, std::string search_query, std::string include_in_search, std::vector<std::string> omit_files)
    {
        std::thread::id this_id = std::this_thread::get_id();
        std::cout << "Thread " << this_id << " started." << std::endl;

        std::cout << "Installing " << program_name << "..." << std::endl;

        application->busy = true;

        std::string generic_path = ProgramManager::GetLatestInstaller(installer_directory, search_query, include_in_search, omit_files) + " ";

        if(generic_path.empty())
        {
            application->busy = false;
            return;
        }
        generic_path.append(install_args);

        std::cout << "Install cmd: " << generic_path << std::endl;

        DWORD error;
        error = ProgramManager::StartProcess(generic_path);

        if(error == FILE_NOT_FOUND)
        {
            std::cout << "Installation failed! File not found." << std::endl;
            GUI().OpenPopup("Error", "The installer file could not be found.");
            application->busy = false;
            return;
        }
        else if(error == ELEVATION_REQUIRED)
        {
            std::cout << "Installation failed! Access denied." << std::endl;
            GUI().OpenPopup("Error", "Elevation is required to install this program.");
            application->busy = false;
            return;
        }

        switch(program)
        {
            case int(Program::PROGRAM_SQLSERVER19):
            {
                ConfigureSQL();
                application->modified_programs.push_back(program_name);
                GUI().OpenPopup("Result", application->modified_programs, "Install");
                break;
            }
            case int(Program::PROGRAM_SSMS):
            {
                //Create public desktop shortcut of SSMS
                ProgramManager::CreateLink( L"C:\\Program Files (x86)\\Microsoft SQL Server Management Studio 18\\Common7\\IDE\\Ssms.exe",
                                            L"C:\\Users\\Public\\Desktop\\Microsoft SQL Server Management Studio 18.lnk",
                                            L"",
                                            L"");

                application->modified_programs.push_back(program_name);
                GUI().OpenPopup("Result", application->modified_programs, "Install");
                break;
            }
            case int(Program::PROGRAM_SQLCU_EXTRACT):
            {
                //Clear this data because it just extracted the Cumulative Update
                application->modified_programs.clear();

                //Actually patch MSSQL with the Cumulative Update
                program = (unsigned char)Program::PROGRAM_SQLCU;
                installer_directory = std::filesystem::current_path().string() + "\\temp";
                search_query = "SETUP.EXE";
                omit_files.push_back("SETUP.EXE.CONFIG");
                omit_files.push_back("RSETUP.EXE");
                omit_files.push_back("MSMPISETUP.EXE");
                std::thread install(&InstallGeneric, program_name, "/Action=Patch /InstanceName=BRANEL /quiet /IAcceptSQLServerLicenseTerms", search_query, include_in_search, omit_files);
                install.detach();
                break;
            }
            case int(Program::PROGRAM_SQLCU):
            {
                //Remove extracted CU files
                if(std::filesystem::exists(std::filesystem::current_path().string() + "\\temp"))
                    std::filesystem::remove_all(std::filesystem::current_path().string() + "\\temp");

                application->modified_programs.push_back(program_name);
                GUI().OpenPopup("Result", application->modified_programs, "Install");
                break;
            }
            default:
            {
                application->modified_programs.push_back(program_name);
                GUI().OpenPopup("Result", application->modified_programs, "Install");
                break; // Compiler warning
            }
        }

        application->busy = false;
    }

    void ConfigureSQL()
    {
        //Set Shared Memory=Disabled & SQL Ports=1434,1433
        std::ofstream file;
        file.open("data/ConfigureSQLPorts.ps1");

        if(!file.is_open())
        {
            std::cout << "Failed to open file!" << std::endl;
        }

        std::string powershell;
        powershell = "\"C:\\Program Files (x86)\\Microsoft SQL Server\\150\\Tools\\Binn\\SQLPS.exe\"\n";
        powershell += "import-module sqlps;\n";
        powershell += "$MachineObject = new-object ('Microsoft.SqlServer.Management.Smo.WMI.ManagedComputer') $env:COMPUTERNAME\n";
        powershell += "$instance = $MachineObject.getSmoObject(\n";
        powershell += "   \"ManagedComputer[@Name='$env:COMPUTERNAME']/\" + \n";
        powershell += "   \"ServerInstance[@Name='BRANEL']\"\n";
        powershell += ")\n";
        powershell += "$instance.ServerProtocols['Sm'].IsEnabled = $false\n";
        powershell += "$instance.ServerProtocols['Sm'].Alter()\n";
        powershell += "$instance.ServerProtocols['Tcp'].IPAddresses['IPAll'].IPAddressProperties['TcpDynamicPorts'].Value = \"1434\"\n";
        powershell += "$instance.ServerProtocols['Tcp'].IPAddresses['IPAll'].IPAddressProperties['TcpPort'].Value = \"1433\"\n";
        powershell += "$instance.ServerProtocols['Tcp'].Alter()";

        file << powershell << endl;

        file.close();

        system("powershell -ExecutionPolicy Unrestricted -F data/ConfigureSQLPorts.ps1");

        std::remove("data/ConfigureSQLPorts.ps1");
    }
}