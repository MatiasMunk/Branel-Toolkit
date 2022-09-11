#include "install.h"

#include "../program_manager.h"
#include "../gui.h"

#include <iostream>
#include <thread>
#include <future>

namespace ActionHandlers::Install
{
    std::string installer_directory;
    std::vector<std::string> installed_programs;

    void Actor(Action &action, std::array<intptr_t, 4> data_ptr)
    {
        unsigned char what = 0;
        std::string install_args = " ";

        action >> what;
        action >> install_args;

        //Clear any previous install data
        installed_programs.clear();

        installer_directory = ProgramManager::GetLatestInstallerDirectory("C:\\Branel\\");
        std::cout << "Installer directory: " << installer_directory << std::endl;

        if(!installer_directory.empty())
        {
            Program install = (Program)what;
            switch(install)
            {
                case Program::PROGRAM_SQLSERVER19:
                {
                    std::cout << "Installing SQL Server 2019..." << std::endl;

                    std::future<bool> result = std::async(&InstallSQLServer19, install_args);

                    if(result.get() == true)
                    {
                        installed_programs.push_back("SQL Server 2019");
                        std::cout << "SQL Server 2019 installed!" << std::endl;
                    }
                    else
                    {
                        std::cout << "SQL Server 2019 installation failed!" << std::endl;
                    }

                    GUI().OpenPopup("Result", installed_programs, "Install");
                    break;
                }
                case Program::PROGRAM_SQLCU:
                {
                    std::cout << "Installing Cumulative Update (KB5011644) for SQL Server 2019..." << std::endl;

                    std::future<bool> result = std::async(&InstallGeneric, install_args, "SQLServer2019-KB5011644-x64.exe", "x64");

                    if(result.get() == true)
                    {
                        installed_programs.push_back("Cumulative Update (KB 5011644)");
                        std::cout << "SQL Server 2019 installed!" << std::endl;
                    }
                    else
                    {
                        std::cout << "SQL Server 2019 installation failed!" << std::endl;
                    }

                    GUI().OpenPopup("Result", installed_programs, "Install");
                    break;
                }
                case Program::PROGRAM_SSMS:
                {
                    std::cout << "Installing SQL Server Management Studio 18..." << std::endl;

                    std::future<bool> result = std::async(&InstallGeneric, install_args, "SSMS-Setup-ENU.exe", "");

                    if(result.get() == true)
                    {
                        //Create public desktop shortcut of SSMS
                        ProgramManager::CreateLink( L"C:\\Program Files (x86)\\Microsoft SQL Server Management Studio 18\\Common7\\IDE\\Ssms.exe",
                                                    L"C:\\Users\\Public\\Desktop\\Microsoft SQL Server Management Studio 18.lnk",
                                                    L"",
                                                    L"");

                        installed_programs.push_back("SQL Server Management Studio 18");
                        std::cout << "SQL Server Management Studio 18 installed!" << std::endl;
                    }
                    else
                    {
                        std::cout << "SQL Server Management Studio 18 installation failed!" << std::endl;
                    }
                    
                    GUI().OpenPopup("Result", installed_programs, "Install");
                    break;
                }
                case Program::PROGRAM_TEAMVIEWER:
                {
                    std::cout << "Installing TeamViewer..." << std::endl;

                    std::future<bool> result = std::async(&InstallGeneric, install_args, "TeamViewer-Host", "");

                    if(result.get() == true)
                    {
                        installed_programs.push_back("TeamViewer");
                        std::cout << "TeamViewer installed!" << std::endl;
                    }
                    else
                    {
                        std::cout << "TeamViewer installation failed!" << std::endl;
                    }
                    
                    GUI().OpenPopup("Result", installed_programs, "Install");
                    break;
                }
                case Program::PROGRAM_ULTRAVNC:
                {
                    std::cout << "Installing UltraVNC..." << std::endl;

                    std::future<bool> result = std::async(&InstallGeneric, install_args, "UltraVNC_", "X64");

                    if(result.get() == true)
                    {
                        installed_programs.push_back("UltraVNC");
                        std::cout << "UltraVNC installed!" << std::endl;
                    }
                    else
                    {
                        std::cout << "UltraVNC installation failed!" << std::endl;
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
                    break; // Compiler warning
                }
            }
        }
    }

    bool InstallGeneric(std::string install_args, std::string executable, std::string optional)
    {
        std::thread::id this_id = std::this_thread::get_id();
        std::cout << "Thread " << this_id << " started." << std::endl;

        std::string generic_path = ProgramManager::GetLatestInstaller(installer_directory, executable, optional);

        if(generic_path.empty())
            return false;

        generic_path.append(install_args);

        std::cout << "Install cmd: " << generic_path << std::endl;

        if(ProgramManager::StartProcess(generic_path) == 0)
            return false;

        return true;
    }

    bool InstallSQLServer19(std::string install_args)
    {
        std::thread::id this_id = std::this_thread::get_id();
        std::cout << "Thread " << this_id << " started." << std::endl;

        //Specify manually because there are a bunch of installers with the same name higher up in the folder hierarchy
        std::string mssql_path = installer_directory + "\\Microsoft\\Microsoft SQL Server\\Microsoft SQL Server 2019 Express\\SQLEXPR_x64_ENU\\Express_ENU\\SETUP.EXE";
        std::string config_file = std::filesystem::current_path().string() + "\\data\\ConfigurationFile_Install.ini";

        //The service account name is different depending on the operating system's language.
        std::string default_install_cmd = mssql_path + " /SAPWD=Br5015edt /ConfigurationFile=" + config_file + " /IAcceptSQLServerLicenseTerms";
        std::string english_install_cmd = mssql_path + " /SAPWD=Br5015edt /ConfigurationFile=" + config_file + " /AGTSVCACCOUNT=NT AUTHORITY\\NETWORKSERVICE /IAcceptSQLServerLicenseTerms";

        //Workaround hack to determine Windows system language for the SQL Agent Account
        //If Danish, it's specified in the configuration file
        //If English, it's specified in the command line
        NetUserManager user_manager;
        if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Brugere")) == 0)
        {
            if(ProgramManager::StartProcess(default_install_cmd) != 0)
                return false;
        }
        else if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Users")) == 0)
        {
            if(ProgramManager::StartProcess(english_install_cmd) != 0)
                return false;
        }
        else return false;

        //Set Shared Memory=Disabled & SQL Ports=1434,1433
        std::ofstream file;
        file.open("data/ConfigureSQLPorts.ps1");

        if(!file.is_open()) return false;

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

        return true;
    }
}