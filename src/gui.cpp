#include "gui.h"

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_allegro5.h>

#include "application.h"

std::string GUI::open_popup;
std::string GUI::popup_message;
std::vector<std::string> GUI::popup_data;
int GUI::input_focus;
bool GUI::did_backup;
bool GUI::need_restart;

GUI::GUI()
{
    this->open_popup = "";
    this->popup_message = "";
    this->input_focus = 0;
    this->did_backup = false;
    this->need_restart = false;
}

GUI::GUI(Application* application)
{
    this->open_popup = "";
    this->popup_message = "";
    this->input_focus = 0;
    this->did_backup = false;
    this->need_restart = false;

    this->application = application;
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

    if(ImGui::BeginPopupModal("Insufficient privileges", NULL, flags))
    {
        ImGui::Separator();
        ImGui::Text(popup_message.c_str());
        if(ImGui::Button("Close."))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    if(ImGui::BeginPopupModal("Create users", NULL, flags))
    {
        ImGui::Text("Do you want to create the users?");
        ImGui::Text("BrAdmin");
        ImGui::Text("BrGuest");

        if(ImGui::Button("Yes, continue."))
        {
            Action action;

            action << (unsigned char)ActionID::ACTION_CREATE_USERS;
            action << "Username";
            action << "Password";

            application->Instruct(action);
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("No, cancel."))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    if(ImGui::BeginPopupModal("Install", NULL, flags))
    {
        if(popup_message.find("SQLServer19") != std::string::npos)
        {
            ImGui::Text("You are about to install the software(s)");
            ImGui::Separator();
            ImGui::Text("Microsoft SQL Server 2019");
            ImGui::Separator();
            ImGui::Text("WARNING:");
            ImGui::Text("This can cause potential data loss");
            ImGui::Text("of existing databases - backup recommended.");
            ImGui::Separator();
            ImGui::Text("Do you want to proceed?");

            if(ImGui::Button("Yes, continue."))
            {
                Action action;

                action << (unsigned char)ActionID::ACTION_INSTALL;
                action << (unsigned char)Program::PROGRAM_SQLSERVER19;
                action << "";

                ImGui::CloseCurrentPopup();
            }
        }
        else if(popup_message.find("SQLCU") != std::string::npos)
        {
            ImGui::Text("You are about to install the software(s)");
            ImGui::Separator();
            ImGui::Text("SQL Server Hotfix (KB5011644)");
            ImGui::Separator();
            ImGui::Text("Do you want to proceed?");

            if(ImGui::Button("Yes, continue."))
            {
                Action action;

                action << (unsigned char)ActionID::ACTION_INSTALL;
                action << (unsigned char)Program::PROGRAM_SQLCU;
                action << "";

                ImGui::CloseCurrentPopup();
            }
        }
        else if(popup_message.find("SQLManagementStudio") != std::string::npos)
        {
            if(this->need_restart)
            {
                ImGui::Text("Your computer needs to be restarted, before");
                ImGui::Text("you can install this program!");

                if(ImGui::Button("OK"))
                {
                    ImGui::CloseCurrentPopup();
                }
            }
            else
            {
                ImGui::Text("You are about to install the software(s)");
                ImGui::Separator();
                ImGui::Text("Microsoft SQL Server Management Studio");
                ImGui::Separator();
                ImGui::Text("Do you want to proceed?");

                if(ImGui::Button("Yes, continue."))
                {
                    //this->InstallSoftware(Program::SQLManagementStudio);
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        else if(popup_message.find("TeamViewer") != std::string::npos)
        {
            ImGui::Text("You are about to install the software(s)");
            ImGui::Separator();
            ImGui::Text("TeamViewer");
            ImGui::Separator();
            ImGui::Text("Do you want to proceed?");

            if(ImGui::Button("Yes, continue."))
            {
                Action action;

                action << (unsigned char)ActionID::ACTION_INSTALL;
                action << uint8_t(Program::PROGRAM_TEAMVIEWER);
                action << " /S";

                application->Instruct(action);
                
                ImGui::CloseCurrentPopup();
            }
        }
        else if(popup_message.find("All") != std::string::npos)
        {
            ImGui::Text("You are about to install the software(s)");
            ImGui::Separator();
            ImGui::Text("UltraVNC");
            ImGui::Text("TeamViewer");
            ImGui::Separator();
            ImGui::Text("Do you want to proceed?");

            if(ImGui::Button("Yes, continue."))
            {
                Action action;

                action << (unsigned char)ActionID::ACTION_INSTALL;
                action << uint8_t(Program::PROGRAM_ULTRAVNC);
                action << " /Silent";

                application->Instruct(action);
                
                ImGui::CloseCurrentPopup();
            }
        }
        
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    if(ImGui::BeginPopupModal("Result", NULL, flags))
    {
        if(popup_message == "Install")
        {
            if(this->popup_data.size() > 0)
            {
                ImGui::Text("The following software has been installed.");
                ImGui::Separator();
                for(auto& program : this->popup_data)
                {
                    ImGui::Text(program.c_str());
                    if(program == "SQL Server 2019" || program == "Cumulative Update (KB 5011644)")
                    {
                        //SQL SSMS needs restart after either of these softwares.
                        this->need_restart = true;
                    }
                }
            }
            else
            {
                ImGui::Text("No software has been installed.");
            }
        }
        else if(popup_message == "Uninstall")
        {
            if(this->popup_data.size() > 0)
            {
                ImGui::Text("The following software has been uninstalled.");
                ImGui::Separator();
                for(auto& program : this->popup_data)
                {
                    ImGui::Text(program.c_str());
                }
            }
            else
            {
                ImGui::Text("No software has been uninstalled.");
            }
        }

        if(ImGui::Button("OK"))
        {
            this->popup_data.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GUI::Render()
{
    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}

void GUI::OpenPopup(std::string id, std::string message)
{
    this->open_popup = id;
    this->popup_message = message;
}

void GUI::OpenPopup(std::string id, std::vector<std::string> data)
{
    this->open_popup = id;
    this->popup_data = data;
}

void GUI::OpenPopup(std::string id, std::vector<std::string> data, std::string message)
{
    this->open_popup = id;
    this->popup_message = message;
    this->popup_data = data;
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
        //Miscellaneous tab
        if(ImGui::BeginTabItem("Misc."))
        {
            if(ImGui::Button("Create users"))
            {
                this->OpenPopup("Create users");
            }
            ImGui::EndTabItem();
        }
        //Installers tab
        if(ImGui::BeginTabItem("Installers"))
        {
            if(ImGui::Button("SQL Server 19"))
            {
                this->OpenPopup("Install", "SQLServer19");
            }
            if(ImGui::Button("CU for SQL Server 19"))
            {
                this->OpenPopup("Install", "SQLCU");
            }
            if(ImGui::Button("SQL Management Studio 18"))
            {
                this->OpenPopup("Install", "SQLManagementStudio");
            }
            ImGui::EndTabItem();
        }
        //Uninstallers tab
        if(ImGui::BeginTabItem("Uninstallers"))
        {
            if(ImGui::Button("SQL Server"))
            {
                this->OpenPopup("Uninstall", "SQLServer");
            }
            ImGui::EndTabItem();
        }
        //Updaters tab
        if(ImGui::BeginTabItem("Updaters"))
        {
            if(ImGui::Button("Update software"))
            {
                this->OpenPopup("Install", "All");
            }

            if(ImGui::Button("Update TeamViewer"))
            {
                this->OpenPopup("Install", "TeamViewer");
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}