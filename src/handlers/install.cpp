#include "install.h"

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
    std::string installer_directory;
    std::vector<std::string> omit_files;
    std::vector<std::string> installed_programs;

    void Actor(Action &action, std::array<intptr_t, 4> data_ptr)
    {
        unsigned char program = 0;
        std::string name = "";
        std::string search_query = "";
        std::string include_in_search = "";
        std::string install_args = "";

        action >> program;
        action >> name;
        action >> search_query;
        action >> include_in_search;
        action >> install_args;

        //Clear any previous install data
        omit_files.clear();
        installed_programs.clear();

        installer_directory = ProgramManager::GetLatestInstallerDirectory("C:\\Branel\\");
        std::cout << "Installer directory: " << installer_directory << std::endl;

        if(!installer_directory.empty())
        {
            switch((Program)program)
            {
                case Program::PROGRAM_SQLSERVER19:
                {
                    printf("Installing %s...\n", name.c_str());
                    std::future<bool> result;

                    omit_files.push_back("SETUP.EXE.CONFIG");
                    omit_files.push_back("RSETUP.EXE");

                    std::string config_file = std::filesystem::current_path().string() + "\\data\\ConfigurationFile_Install.ini";

                    //The service account name is different depending on the operating system's language.
                    std::string default_install_args = " /SAPWD=Br5015edt /ConfigurationFile=" + config_file + " /IAcceptSQLServerLicenseTerms";
                    std::string english_install_args = " /SAPWD=Br5015edt /ConfigurationFile=" + config_file + " /AGTSVCACCOUNT=NT AUTHORITY\\NETWORKSERVICE /IAcceptSQLServerLicenseTerms";

                    //Workaround hack to determine Windows system language for the SQL Agent Account
                    //If Danish, it's specified in the configuration file
                    //If English, it's specified in the command line
                    NetUserManager user_manager;
                    if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Brugere")) == 0)
                    {
                        result = std::async(&InstallGeneric, default_install_args, search_query, include_in_search, omit_files);
                    }
                    else if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Users")) == 0)
                    {
                        result = std::async(&InstallGeneric, english_install_args, search_query, include_in_search, omit_files);
                    }

                    if(result.get() == true)
                    {
                        installed_programs.push_back(name);
                        GUI().OpenPopup("Result", installed_programs, "Install");
                        printf("%s installed!\n", name.c_str());

                        //Set Shared Memory=Disabled & SQL Ports=1434,1433
                        std::ofstream file;
                        file.open("data/ConfigureSQLPorts.ps1");

                        if(!file.is_open())
                        {
                            printf("Failed to open file!\n");
                            return;
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

                    break;
                }
                case Program::PROGRAM_SQLCU:
                {
                    printf("Installing %s...\n", name.c_str());
                    std::future<bool> result;

                    //Extract the CU installer
                    result = std::async(&InstallGeneric, install_args, search_query, include_in_search, omit_files);

                    if(result.get() != true)
                        return;

                    //Clear this data because it just extracted the Cumulative Update
                    installed_programs.clear();

                    //Actually patch MSSQL with the Cumulative Update
                    installer_directory = ".\\temp";
                    search_query = "SETUP.EXE";
                    omit_files.push_back("SETUP.EXE.CONFIG");
                    omit_files.push_back("RSETUP.EXE");
                    omit_files.push_back("MSMPISETUP.EXE");
                    result = std::async(&InstallGeneric, "/Action=Patch /InstanceName=BRANEL /quiet /IAcceptSQLServerLicenseTerms", search_query, include_in_search, omit_files);

                    if(result.get() == true)
                    {
                        installed_programs.push_back(name);
                        printf("%s installed!\n", name.c_str());
                    }

                    //Remove extracted CU files
                    if(std::filesystem::exists(".\\temp"))
                        std::filesystem::remove_all(".\\temp");

                    GUI().OpenPopup("Result", installed_programs, "Install");
                    break;
                }
                case Program::PROGRAM_SSMS:
                {
                    printf("Installing %s...\n", name.c_str());
                    std::future<bool> result;

                    result = std::async(&InstallGeneric, install_args, search_query, include_in_search, omit_files);

                    if(result.get() == true)
                    {
                        installed_programs.push_back(name);
                        printf("%s installed!\n", name.c_str());

                        //Create public desktop shortcut of SSMS
                        ProgramManager::CreateLink( L"C:\\Program Files (x86)\\Microsoft SQL Server Management Studio 18\\Common7\\IDE\\Ssms.exe",
                                                    L"C:\\Users\\Public\\Desktop\\Microsoft SQL Server Management Studio 18.lnk",
                                                    L"",
                                                    L"");
                    }

                    GUI().OpenPopup("Result", installed_programs, "Install");
                    break;
                }
                case Program::PROGRAM_ALL:
                {
                    break;
                }
                default:
                {
                    printf("Installing %s...\n", name.c_str());
                    std::future<bool> result;

                    result = std::async(&InstallGeneric, install_args, search_query, include_in_search, omit_files);

                    if(result.get() == true)
                    {
                        installed_programs.push_back(name);
                        printf("%s installed!\n", name.c_str());
                    }

                    GUI().OpenPopup("Result", installed_programs, "Install");
                    break;
                }
            }
        }
    }

    bool InstallGeneric(std::string install_args, std::string search_query, std::string include_in_search, std::vector<std::string> omit_files)
    {
        std::thread::id this_id = std::this_thread::get_id();
        std::cout << "Thread " << this_id << " started." << std::endl;

        std::string generic_path = ProgramManager::GetLatestInstaller(installer_directory, search_query, include_in_search, omit_files) + " ";

        if(generic_path.empty())
            return false;

        generic_path.append(install_args);

        std::cout << "Install cmd: " << generic_path << std::endl;

        DWORD error;
        error = ProgramManager::StartProcess(generic_path);

        switch(error)
        {
            case ERROR_FILE_NOT_FOUND:
            {
                std::cout << "Installation failed! File not found." << std::endl;
                GUI().OpenPopup("File not found", "The specified file could not be found.");
                return false;
                break;
            }
            case ELEVATION_REQUIRED:
            {
                std::cout << "Installation failed! Access denied." << std::endl;
                GUI().OpenPopup("Insufficient privileges", "Did run the program as administrator?");
                return false;
                break;
            }
            default:
            {
                break; // Compiler warning
            }
        }

        return true;
    }
}