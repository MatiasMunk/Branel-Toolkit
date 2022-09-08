#include "gui.h"

//#include "util.h"
//#include "config.h"

#include <iostream>

#include <Windows.h>
#include <lm.h>
/*#include <LMaccess.h>
#include <lmerr.h>
#include <lmapibuf.h>*/

#include <imgui/imgui.h>
#include <imgui/imgui_impl_allegro5.h>


bool GUI::initialized_ = false;
std::string GUI::open_window;
std::string GUI::open_popup;
std::string GUI::popup_message;
int GUI::input_focus;
bool GUI::did_backup;

GUI::GUI()
{
    if(!this->initialized_)
    {
        this->open_popup = "";
        this->popup_message = "";
        this->input_focus = 0;
        this->did_backup = false;

        this->initialized_ = true;
    }
}

void GUI::Process()
{
    this->Index();

    if(!this->open_popup.empty())
    {
        ImGui::OpenPopup(this->open_popup.c_str());
        this->open_popup.clear();
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;

    if(ImGui::BeginPopupModal("Uninstall result", NULL, flags))
    {
        if(this->uninstalled_programs.size() > 0)
        {
            ImGui::Text("The software has been uninstalled.");
            ImGui::Separator();
            for(auto& program : this->uninstalled_programs)
            {
                ImGui::Text(program.c_str());
            }
            if(ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
                this->uninstalled_programs.clear();
            }
        }
        else
        {
            ImGui::Text("No software has been uninstalled.");
            if(ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }

    if(ImGui::BeginPopupModal("No rights!", NULL, flags))
    {
        ImGui::Text("Could not complete action.");
        ImGui::Text("Did run the program as administrator?");
        if(ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Create Branel Users", NULL, flags))
    {
        ImGui::Text("Do you want to create the users?");
        ImGui::Text("BrAdmin");
        ImGui::Text("BrGuest");

        if(ImGui::Button("Yes, continue."))
        {
            this->CreateBranelUsers();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Backup", NULL, flags))
    {
        ImGui::Text("Do you want to remove all SQL Server programs?");
        ImGui::Text("WARNING:");
        ImGui::Text("Data will be removed during this process.");
        ImGui::Separator();
        ImGui::Text("Did you remember to backup?");

        if(ImGui::Button("Yes, continue."))
        {
            did_backup = true;
            this->UninstallMSSQL();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Install SQL Server 19", NULL, flags))
    {
        ImGui::Text("Do you want to install SQL Server 19?");
        ImGui::Text("WARNING:");
        ImGui::Text("This can cause potential data loss");
        ImGui::Text("of existing databases.");
        ImGui::Separator();
        ImGui::Text("Did you remember to backup?");

        if(ImGui::Button("Yes, continue."))
        {
            did_backup = true;
            this->InstallSoftware(Program::SQLServer19);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Install CU for SQL Server 19", NULL, flags))
    {
        ImGui::Text("Do you want to update the following software?");
        ImGui::Separator();
        ImGui::Text("Cumulative Update Package (KB5011644)");
        ImGui::Text("for SQL Server 19");

        if(ImGui::Button("Yes, continue."))
        {
            this->InstallSoftware(Program::SQLCU);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Install SQL Management Studio", NULL, flags))
    {
        ImGui::Text("Do you want to update the following software?");
        ImGui::Separator();
        ImGui::Text("SQL Server Management Studio 18");

        if(ImGui::Button("Yes, continue."))
        {
            this->InstallSoftware(Program::SQLManagementStudio);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Update Software", NULL, flags))
    {
        ImGui::Text("Do you want to update the following software?");
        ImGui::Separator();
        ImGui::Text("UltraVNC");
        ImGui::Text("TeamViewer");

        if(ImGui::Button("Yes, continue."))
        {
            this->InstallSoftware(Program::All);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Update TeamViewer", NULL, flags))
    {
        ImGui::Text("Do you want to update TeamViewer?");

        if(ImGui::Button("Yes, continue."))
        {
            this->InstallSoftware(Program::TeamViewer);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("Install Result", NULL, flags))
    {
        if(this->installed_programs.size() > 0)
        {
            ImGui::Text("The following software has been installed.");
            ImGui::Separator();
            for(auto& program : this->installed_programs)
            {
                ImGui::Text(program.c_str());
                if(program == "SQL Server 2019")
                {
                    ImGui::Separator();
                    ImGui::Text("Don't forget to configure SQL Server!");
                    ImGui::Text("C:\\Windows\\SysWOW64\\SQLServerManager15.msc");
                }
            }
            if(ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
                this->installed_programs.clear();
            }
        }
        else
        {
            ImGui::Text("No software has been updated.");
            if(ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void GUI::Render()
{
    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}

void GUI::OpenWindow(std::string id)
{
    this->open_window = id;
}

void GUI::OpenPopup(std::string id, std::string message)
{
    this->open_popup = id;
    this->popup_message = message;
}

void GUI::SetFocus(int widget)
{
    this->input_focus = widget;
}

void GUI::Index()
{
    ImGui::NewFrame();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(400.f, 200.f));
    if(!ImGui::Begin("Index", NULL, flags))
    {
        ImGui::End();
        return;
    }

    std::string action = "";

    if(ImGui::BeginTabBar("Options", 0))
    {
        if(ImGui::BeginTabItem("Misc."))
        {
            if(ImGui::Button("Create Branel Users"))
        {
            action = "Create Branel Users";
        }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Installers"))
        {
            if(ImGui::Button("Install SQL Server 19"))
            {
                action = "Install SQL Server 19";
            }
            if(ImGui::Button("Install CU for SQL Server 19"))
            {
                action = "Install CU for SQL Server 19";
            }
            if(ImGui::Button("Install SQL Management Studio"))
            {
                action = "Install SQL Management Studio";
            }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Uninstallers"))
        {
            if(ImGui::Button("Uninstall SQL Server"))
            {
                action = "Uninstall SQL Server";
            }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Updaters"))
        {
            if(ImGui::Button("Update software"))
            {
                action = "Update Software";
            }

            if(ImGui::Button("Update TeamViewer"))
            {
                action = "Update TeamViewer";
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if(action == "Create Branel Users")
    {
        this->OpenPopup("Create Branel Users");
    }
    else if(action == "Uninstall SQL Server")
    {
        this->OpenPopup("Backup");
    }
    else if(action == "Install SQL Server 19")
    {
        this->OpenPopup("Install SQL Server 19");
    }
    else if(action == "Install CU for SQL Server 19")
    {
        this->OpenPopup("Install CU for SQL Server 19");
    }
    else if(action == "Install SQL Management Studio")
    {
        this->OpenPopup("Install SQL Management Studio");
    }
    else if(action == "Update Software")
    {
        this->OpenPopup("Update Software");
    }
    else if(action == "Update TeamViewer")
    {
        this->OpenPopup("Update TeamViewer");
    }

    ImGui::End();
}

void GUI::CreateBranelUsers()
{
   /*
    wchar_t name[] = L"BrAdmin";
    wchar_t pass[] = L"Fiber.5015";
    */

    try
    {
        //Remove standard profile picture from Windows User Account Data folder
        std::filesystem::remove("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.png");
        std::filesystem::remove("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.bmp");

        //Copy Branel profile picture to Windows User Account Data folder
        std::filesystem::copy("data/user.png", "C:\\ProgramData\\Microsoft\\User Account Pictures\\user.png", std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("data/user.bmp", "C:\\ProgramData\\Microsoft\\User Account Pictures\\user.bmp", std::filesystem::copy_options::overwrite_existing);

        NetUserManager user_manager;
        if(user_manager.CreateBranelUsers() == 0)
        {
            std::cout << "Branel users created successfully." << std::endl;
        }
        else
        {
            std::cout << "Failed to create Branel users." << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        //Open popup
        this->OpenPopup("No rights!");
    }
}

void GUI::UninstallMSSQL()
{
    if(did_backup)
    {
        this->uninstalled_programs = ProgramManager::UninstallMSSQL();

        this->OpenPopup("Uninstall result");

        did_backup = false;
    }
}

void GUI::InstallSoftware(Program what)
{
    std::string installer_directory = ProgramManager::GetLatestInstallerDirectory("C:\\Branel\\");

    std::cout << "Installer directory: " << installer_directory << std::endl;

    if(!installer_directory.empty())
    {
        if(what == Program::TeamViewer)
        {
            std::string teamviewer_path = ProgramManager::GetLatestInstaller(installer_directory, "TeamViewer-Host");
            std::cout << "TeamViewer Path: " << teamviewer_path << std::endl;
            if(ProgramManager::StartProcess(teamviewer_path + " /S") == 0)
            {
                installed_programs.push_back("TeamViewer");
                std::cout << "TeamViewer Installed!" << std::endl;
            }

            this->OpenPopup("Install Result");
        }
        else if(what == Program::SQLServer19)
        {
            if(did_backup)
            {
                //Specify manually because there are a bunch of installers with the same name higher up in the folder hierarchy
                std::string mssql_path = installer_directory + "\\Microsoft\\Microsoft SQL Server\\Microsoft SQL Server 2019 Express\\SQLEXPR_x64_ENU\\Express_ENU\\SETUP.EXE";

                if(ProgramManager::StartProcess(mssql_path + " /ACTION=INSTALL /INDICATEPROGRESS /FEATURES=SQL,AS,RS,IS,Tools,SQLENGINE,REPLICATION,FULLTEXT,CONN,IS,BC,SDK /INSTANCENAME=BRANEL /BROWSERSVCSTARTUPTYPE=Automatic /SQLCOLLATION=Danish_Norwegian_CI_AS /SECURITYMODE=SQL /SAPWD=Br5015edt /NPENABLED=0 /TCPENABLED=1 /QUIET /IACCEPTSQLSERVERLICENSETERMS") == 0)
                {
                    installed_programs.push_back("SQL Server 2019");
                }

                this->OpenPopup("Install Result");

                did_backup = false;
            }
        }
        else if(what == Program::SQLCU)
        {
            //Cumulative Update (KB5011644) for SQL Server 2019
            std::string cumulative_update_path = ProgramManager::GetLatestInstaller(installer_directory, "SQLServer2019-KB5011644-x64.exe", "x64");
            std::cout << "Cumulative Update Path: " << cumulative_update_path << std::endl;

            if(ProgramManager::StartProcess(cumulative_update_path + " /qs /IAcceptSQLServerLicenseTerms /Action=Patch /InstanceName=BRANEL") == 0)
            {
                installed_programs.push_back("CU (KB5011644) for SQL Server 2019");
            }

            this->OpenPopup("Install Result");
        }
        else if(what == Program::SQLManagementStudio)
        {
            //SQL Management Studio
            std::string sql_management_studio_path = ProgramManager::GetLatestInstaller(installer_directory, "SSMS-Setup-ENU.exe");
            std::cout << "Cumulative Update Path: " << sql_management_studio_path << std::endl;

            if(ProgramManager::StartProcess(sql_management_studio_path + " /install /quiet /norestart") == 0)
            {
                installed_programs.push_back("SQL Server Management Studio 18");
            }

            //Create public desktop shortcut of SSMS
            ProgramManager::CreateLink(L"C:\\Program Files (x86)\\Microsoft SQL Server Management Studio 18\\Common7\\IDE\\Ssms.exe", L"C:\\Users\\Public\\Desktop\\Microsoft SQL Server Management Studio 18.lnk", L"", L"");

            this->OpenPopup("Install Result");
        }
        else
        {
            //UltraVNC
            std::string uvnc_path = ProgramManager::GetLatestInstaller(installer_directory, "UltraVNC_", "X64");
            std::cout << "UltraVNC Path: " << uvnc_path << std::endl;
            if(ProgramManager::StartProcess(uvnc_path + " /Silent") == 0)
            {
                installed_programs.push_back("UltraVNC");
                std::cout << "UltraVNC Installed!" << std::endl;
            }

            //Teamviewer
            std::string teamviewer_path = ProgramManager::GetLatestInstaller(installer_directory, "TeamViewer-Host");
            std::cout << "TeamViewer Path: " << teamviewer_path << std::endl;
            if(ProgramManager::StartProcess(teamviewer_path + " /S") == 0)
            {
                installed_programs.push_back("TeamViewer");
                std::cout << "TeamViewer Installed!" << std::endl;
            }

            this->OpenPopup("Install Result");
        }
    }
}