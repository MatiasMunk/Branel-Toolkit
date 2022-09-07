#include "gui.h"

//#include "util.h"
//#include "config.h"

#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_allegro5.h>


bool GUI::initialized_ = false;
std::string GUI::open_window;
std::string GUI::open_popup;
std::string GUI::popup_message;
int GUI::input_focus;
bool GUI::did_backup;

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

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

    if(!this->open_window.empty())
    {
        if(this->open_window == "uninstall_mssql")
        {
            this->UninstallMSSQL();
        }
        /*else if(this->open_window == "settings")
        {
            this->Settings();
        }
        else if(this->open_window == "about")
        {
            this->About();
        }*/
    }

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

        this->uninstalled_programs.clear();
    }

    if(ImGui::BeginPopupModal("Backup", NULL, flags))
    {
        ImGui::Text("Data will be removed during this process.");
        ImGui::Text("Did you remember to backup?");

        if(ImGui::Button("Yes, continue."))
        {
            did_backup = true;
            UninstallMSSQL();
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::Button("Cancel"))
        {
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
    ImGui::SetNextWindowSize(ImVec2(190.f, 200.f));
    if(!ImGui::Begin("Index", NULL, flags))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Branel Toolkit", 0);

    std::string action = "";
    if(ImGui::Button("Uninstall MSSQL"))
    {
        action = "uninstall_mssql";
    }

    if(action == "uninstall_mssql")
    {
        this->OpenPopup("Backup");
    }

    ImGui::End();
}

void GUI::UninstallMSSQL()
{
    if(did_backup)
    {
        //this->OpenWindow("uninstall_mssql");
        std::vector<Software>* list = InstalledPrograms::GetInstalledPrograms(false);
        for(vector<Software>::iterator iter = list->begin(); iter!=list->end(); iter++)
        {
            if(iter->DisplayName.find(L"Compact") != std::string::npos)
            {
                continue;
            }
            else if((iter->DisplayName.find(L"SQL Server") != std::string::npos) || (iter->DisplayName.find(L"T-SQL") != std::string::npos) || (iter->DisplayName.find(L"ODBC Driver") != std::string::npos))
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

                std::string uninstaller(iter->UninstallString.begin(), iter->UninstallString.end());
                
                ReplaceStringInPlace(uninstaller, "MsiExec.exe /X", "MsiExec.exe /qn /X");
                ReplaceStringInPlace(uninstaller, "MsiExec.exe /I", "MsiExec.exe /qn /X");

                ReplaceStringInPlace(uninstaller, "x64\\SetupARP.exe", "Setup.exe");
                if(uninstaller.find("Setup.exe") != std::string::npos)
                {
                    ///QS /ACTION=UNINSTALL /FEATURES=SQL,AS,RS,IS,Tools,SQLENGINE,REPLICATION,FULLTEXT,CONN,IS,BC,SDK,SSMS,ADV_SSMS /INSTANCENAME=\"BRANEL\" /QUIET /IACCEPTSQLSERVERLICENSETERMS
                    uninstaller = uninstaller + " /ACTION=UNINSTALL /FEATURES=SQL,AS,RS,IS,Tools,SQLENGINE,REPLICATION,FULLTEXT,CONN,IS,BC,SDK /INSTANCENAME=BRANEL /QUIET /IACCEPTSQLSERVERLICENSETERMS";
                }
                else if(uninstaller.find("SSMS-Setup-ENU.exe") != std::string::npos)
                {
                    uninstaller = uninstaller + " /quiet /norestart";
                }

                std::cout << "Uninstaller: " << uninstaller << std::endl;
                
                //Create Uninstaller process and wait for it to finish
                STARTUPINFOA si;
                PROCESS_INFORMATION pi;
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                //Start the child process.
                if(!CreateProcessA(NULL,   // No module name (use command line)
                    &uninstaller[0],        // Command line
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
                    return;
                }

                //Wait until child process exits.
                WaitForSingleObject(pi.hProcess, INFINITE);

                //Close process and thread handles.
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);

                std::string temp(iter->DisplayName.begin(), iter->DisplayName.end());
                this->uninstalled_programs.push_back(temp);
            }
        }

        std::rename("C:\\Program Files\\Microsoft SQL Server", "C:\\Program Files\\Microsoft SQL Server.old");
        std::rename("C:\\Program Files (x86)\\Microsoft SQL Server", "C:\\Program Files (x86)\\Microsoft SQL Server.old");

        this->OpenPopup("Uninstaller");

        did_backup = false;
    }
}