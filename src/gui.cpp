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

    if(ImGui::BeginPopupModal("Uninstaller", NULL, flags))
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

    if(ImGui::BeginPopupModal("admin_privileges", NULL, flags))
    {
        ImGui::Text("Could not complete action.");
        ImGui::Text("Did run the program as administrator?");
        if(ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("create_branel_users", NULL, flags))
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
        ImGui::Text("This will remove MSSQL.");
        ImGui::Text("Data will be removed during this process.");
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
    else if(ImGui::BeginPopupModal("install_mssql", NULL, flags))
    {
        ImGui::Text("Do you want to install MSSQL?");

        if(ImGui::Button("Yes, continue."))
        {
            did_backup = true;
            //this->InstallMSSQL();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("update_software", NULL, flags))
    {
        ImGui::Text("Do you want to update the following software?");
        ImGui::Separator();
        ImGui::Text("UltraVNC");
        ImGui::Text("TeamViewer");

        if(ImGui::Button("Yes, continue."))
        {
            this->UpdateSoftware(Program::All);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("update_teamviewer", NULL, flags))
    {
        ImGui::Text("Do you want to update TeamViewer?");

        if(ImGui::Button("Yes, continue."))
        {
            this->UpdateSoftware(Program::TeamViewer);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("install_result", NULL, flags))
    {
        if(this->installed_programs.size() > 0)
        {
            ImGui::Text("The following software has been updated.");
            ImGui::Separator();
            for(auto& program : this->installed_programs)
            {
                ImGui::Text(program.c_str());
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
            action = "create_branel_users";
        }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Installers"))
        {
            if(ImGui::Button("Install MSSQL"))
            {
                action = "install_mssql";
            }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Uninstallers"))
        {
            if(ImGui::Button("Uninstall MSSQL"))
            {
                action = "uninstall_mssql";
            }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Updaters"))
        {
            if(ImGui::Button("Update software"))
            {
                action = "update_software";
            }

            if(ImGui::Button("Update TeamViewer"))
            {
                action = "update_teamviewer";
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if(action == "create_branel_users")
    {
        this->OpenPopup("create_branel_users");
    }
    else if(action == "uninstall_mssql")
    {
        this->OpenPopup("Backup");
    }
    else if(action == "install_mssql")
    {
        this->OpenPopup("install_mssql");
    }
    else if(action == "update_software")
    {
        this->OpenPopup("update_software");
    }
    else if(action == "update_teamviewer")
    {
        this->OpenPopup("update_teamviewer");
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
        this->OpenPopup("admin_privileges");
    }
}

void GUI::UninstallMSSQL()
{
    if(did_backup)
    {
        

        std::rename("C:\\Program Files\\Microsoft SQL Server", "C:\\Program Files\\Microsoft SQL Server.old");
        std::rename("C:\\Program Files (x86)\\Microsoft SQL Server", "C:\\Program Files (x86)\\Microsoft SQL Server.old");

        this->OpenPopup("Uninstaller");

        did_backup = false;
    }
}

void GUI::InstallMSSQL()
{
    //this->OpenWindow("install_mssql");
}

void GUI::UpdateSoftware(Program what)
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

            this->OpenPopup("install_sucess");
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

            this->OpenPopup("install_result");
        }
    }
}