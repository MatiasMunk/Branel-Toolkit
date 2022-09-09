#include "install.h"

#include "../program_manager.h"
#include "../gui.h"

#include <iostream>

namespace ActionHandlers::Install
{
    void Actor(Action &action, std::array<intptr_t, 4> data_ptr)
    {
        unsigned char what = 0;
        std::string install_args = " ";

        action >> what;
        action >> install_args;

        std::vector<std::string> installed_programs;
        std::string installer_directory = ProgramManager::GetLatestInstallerDirectory("C:\\Branel\\");
        std::cout << "Installer directory: " << installer_directory << std::endl;
        if(!installer_directory.empty())
        {
            Program install = (Program)what;
            switch(install)
            {
                //This could probably be split into functions instead.
                //Too lazy.
                case Program::PROGRAM_SQLSERVER19:
                {
                    //Specify manually because there are a bunch of installers with the same name higher up in the folder hierarchy
                    std::string mssql_path = installer_directory + "\\Microsoft\\Microsoft SQL Server\\Microsoft SQL Server 2019 Express\\SQLEXPR_x64_ENU\\Express_ENU\\SETUP.EXE";

                    std::string configuration_file = std::filesystem::current_path().string() + "\\data\\ConfigurationFile_Install.ini";
                    std::cout << "Configuration: " << configuration_file << "" << std::endl;

                    //Workaround hack to determine Windows system language for the SQL Agent Account
                    //If Danish, it's specified in the configuration file
                    //If English, it's specified in the command line
                    NetUserManager user_manager;
                    if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Brugere")) == 0)
                    {
                        std::cout << mssql_path + " /SAPWD=Br5015edt /ConfigurationFile=" + configuration_file + " /IAcceptSQLServerLicenseTerms" << std::endl;
                        if(ProgramManager::StartProcess(mssql_path + " /SAPWD=Br5015edt /ConfigurationFile=" + configuration_file + " /IAcceptSQLServerLicenseTerms") == 0)
                        {
                            installed_programs.push_back("SQL Server 2019");
                        }
                    }
                    else
                    {
                        std:: cout << mssql_path + " /SAPWD=Br5015edt /ConfigurationFile=" + configuration_file + " /AGTSVCACCOUNT=NT AUTHORITY\\NETWORKSERVICE /IAcceptSQLServerLicenseTerms" << std::endl;
                        if(ProgramManager::StartProcess(mssql_path + " /SAPWD=Br5015edt /ConfigurationFile=" + configuration_file + " /AGTSVCACCOUNT=NT AUTHORITY\\NETWORKSERVICE /IAcceptSQLServerLicenseTerms") == 0)
                        {
                            installed_programs.push_back("SQL Server 2019");
                        }
                    }

                    //Set Shared Memory=Disabled & SQL Ports=1434,1433
                    std::ofstream file;
                    file.open("data/ConfigureSQLPorts.ps1");

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

                    GUI().OpenPopup("Result", installed_programs, "Success");

                    break;
                }
                case Program::PROGRAM_SQLCU:
                {
                    //Cumulative Update (KB5011644) for SQL Server 2019
                    std::string cumulative_update_path = ProgramManager::GetLatestInstaller(installer_directory, "SQLServer2019-KB5011644-x64.exe", "x64");

                    cumulative_update_path.append(install_args);

                    std::cout << "Cumulative Update Path: " << cumulative_update_path << std::endl;

                    if(ProgramManager::StartProcess(cumulative_update_path + install_args) == 0)
                    {
                        installed_programs.push_back("Cumulative Update (KB 5011644)");
                    }

                    GUI().OpenPopup("Result", installed_programs, "Success");
                    break;
                }
                case Program::PROGRAM_SSMS:
                {
                    std::string sql_management_studio_path = ProgramManager::GetLatestInstaller(installer_directory, "SSMS-Setup-ENU.exe");
                    std::cout << "Cumulative Update Path: " << sql_management_studio_path << std::endl;

                    if(ProgramManager::StartProcess(sql_management_studio_path + " /install /quiet /norestart") == 0)
                    {
                        installed_programs.push_back("SQL Server Management Studio 18");
                    }

                    //Create public desktop shortcut of SSMS
                    ProgramManager::CreateLink(L"C:\\Program Files (x86)\\Microsoft SQL Server Management Studio 18\\Common7\\IDE\\Ssms.exe", L"C:\\Users\\Public\\Desktop\\Microsoft SQL Server Management Studio 18.lnk", L"", L"");

                    GUI().OpenPopup("Result", installed_programs, "Success");
                    break;
                }
                case Program::PROGRAM_TEAMVIEWER:
                {
                    std::string teamviewer_path = ProgramManager::GetLatestInstaller(installer_directory, "TeamViewer-Host");

                    teamviewer_path.append(install_args);

                    std::cout << "TeamViewer Path: " << teamviewer_path << std::endl;

                    if(teamviewer_path.empty())
                        return;

                    if(ProgramManager::StartProcess(teamviewer_path + install_args) == 0)
                    {
                        std::cout << "TeamViewer Installed!" << std::endl;
                        installed_programs.push_back("TeamViewer");
                        GUI().OpenPopup("Result", installed_programs, "Success");
                    }
                    break;
                }
                case Program::PROGRAM_ULTRAVNC:
                {
                    std::string uvnc_path = ProgramManager::GetLatestInstaller(installer_directory, "UltraVNC_", "X64");
                    uvnc_path.append(install_args);

                    std::cout << "UltraVNC Path: " << uvnc_path << std::endl;

                    if(uvnc_path.empty())
                        return;

                    if(ProgramManager::StartProcess(uvnc_path) == 0)
                    {
                        std::cout << "UltraVNC Installed!" << std::endl;
                        installed_programs.push_back("UltraVNC");
                        GUI().OpenPopup("Result", installed_programs, "Install");
                    }
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
}