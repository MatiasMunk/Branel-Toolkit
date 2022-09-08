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
        else if(this->open_window == "create_branel_users")
        {
            this->CreateBranelUsers();
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
            this->InstallMSSQL();
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
        ImGui::Text("Do you want to update software?");

        if(ImGui::Button("Yes, continue."))
        {
            did_backup = true;
            this->UpdateSoftware();
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

    ImGui::Text("Branel Toolkit - v0.1.8", 0);

    std::string action = "";
    if(ImGui::Button("Create Branel Users"))
    {
        action = "create_branel_users";
    }

    if(ImGui::Button("Uninstall MSSQL"))
    {
        action = "uninstall_mssql";
    }

    if(ImGui::Button("Install MSSQL"))
    {
        action = "install_mssql";
    }

    if(ImGui::Button("Update software"))
    {
        action = "update_software";
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

    ImGui::End();
}

void GUI::CreateBranelUsers()
{
    /*
    LPWSTR lpszDomain,
    LPWSTR lpszUser,
    LPWSTR lpszPassword,
    LPWSTR lpszLocalGroup
    */
   /*
    wchar_t name[] = L"BrAdmin";
    wchar_t pass[] = L"Fiber.5015";
    */

    try
    {
        std::filesystem::remove("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.png");
        std::filesystem::remove("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.bmp");

        std::filesystem::copy("data/user.png", "C:\\ProgramData\\Microsoft\\User Account Pictures\\user.png", std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy("data/user.bmp", "C:\\ProgramData\\Microsoft\\User Account Pictures\\user.bmp", std::filesystem::copy_options::overwrite_existing);

        LocalUser user;

        if(user.GetLocalGroup(const_cast<wchar_t*>(L"Brugere")) == 0)
        {
            //Create BrAdmin
            user.CreateLocalUser(const_cast<wchar_t*>(L""), const_cast<wchar_t*>(L"BrAdmin"), const_cast<wchar_t*>(L"Fiber.5015"), const_cast<wchar_t*>(L"Branel IT"), const_cast<wchar_t*>(L"Brugere"));

            user.AddUserToGroup(const_cast<wchar_t*>(L"BrAdmin"), const_cast<wchar_t*>(L"Brugere"));
            user.AddUserToGroup(const_cast<wchar_t*>(L"BrAdmin"), const_cast<wchar_t*>(L"Administratorer"));

            //Create BrGuest
            user.CreateLocalUser(const_cast<wchar_t*>(L""), const_cast<wchar_t*>(L"BrGuest"), const_cast<wchar_t*>(L"Fiber.5015"), const_cast<wchar_t*>(L"Branel IT"), const_cast<wchar_t*>(L"Brugere"));

            user.AddUserToGroup(const_cast<wchar_t*>(L"BrGuest"), const_cast<wchar_t*>(L"Brugere"));
            user.AddUserToGroup(const_cast<wchar_t*>(L"BrGuest"), const_cast<wchar_t*>(L"Administratorer"));
        }
        else if(user.GetLocalGroup(const_cast<wchar_t*>(L"Users")) == 0)
        {
            //Create BrAdmin
            user.CreateLocalUser(const_cast<wchar_t*>(L""), const_cast<wchar_t*>(L"BrAdmin"), const_cast<wchar_t*>(L"Fiber.5015"), const_cast<wchar_t*>(L"Branel IT"), const_cast<wchar_t*>(L"Users"));

            user.AddUserToGroup(const_cast<wchar_t*>(L"BrAdmin"), const_cast<wchar_t*>(L"Users"));
            user.AddUserToGroup(const_cast<wchar_t*>(L"BrAdmin"), const_cast<wchar_t*>(L"Administrators"));

            //Create BrGuest
            user.CreateLocalUser(const_cast<wchar_t*>(L""), const_cast<wchar_t*>(L"BrGuest"), const_cast<wchar_t*>(L"Fiber.5015"), const_cast<wchar_t*>(L"Branel IT"), const_cast<wchar_t*>(L"Users"));

            user.AddUserToGroup(const_cast<wchar_t*>(L"BrGuest"), const_cast<wchar_t*>(L"Users"));
            user.AddUserToGroup(const_cast<wchar_t*>(L"BrGuest"), const_cast<wchar_t*>(L"Administrators"));
        }
        else
        {
            std::cout << "There was an error adding the users!" << std::endl;
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

void GUI::InstallMSSQL()
{
    //this->OpenWindow("install_mssql");
}

void GUI::UpdateSoftware()
{
    std::vector<std::string> r;
    for(auto& p : std::filesystem::directory_iterator("C:\\Branel\\"))
    {
        if (p.is_directory())
        {
            //Check if folder name contains "install"
            std::string temp(p.path().string());
            if(temp.find("Install") != std::string::npos)
            {
                r.push_back(temp);
            }
        }
    }
    
    if(r.size() > 1)
    {
        std::cout << "More than one install folder found!" << std::endl;
        return;
    }
    else if(r.size() == 0)
    {
        std::cout << "No install folder found!" << std::endl;
        return;
    }
    else
    {
        //Start finding software folders in the Install folder and install them
        std::vector<std::string> software_to_check;
        software_to_check.push_back("UltraVNC");
        software_to_check.push_back("TeamViewer");

        std::vector<std::string> software_folders;
        std::vector<std::string> temp_newest;
        std::vector<std::string> software_list;
        //Loop through software_to_check
        for(auto& folder_name : software_to_check)
        {
            for(auto& p : std::filesystem::directory_iterator(r.at(0) + "\\" + folder_name + "\\"))
            {
                if (p.is_directory())
                {
                    //Check if folder contains UltraVNC version folders
                    std::string temp(p.path().string());
                    if(temp.find(folder_name) != std::string::npos)
                    {
                        software_folders.push_back(temp);
                    }
                }
            }
        }
        

        //Print all elements in software vector
        for(auto& folder : software_folders)
        {
            temp_newest.push_back(folder);
            std::cout << folder << std::endl;

            /*size_t pos = folder.find_last_of("UltraVNC") + 1;
            std::string temp = folder.substr(pos);
            std::cout << temp << std::endl;*/
        }

        //Check which of temp_newest is greater
        std::string newest = temp_newest.at(0);
        for(auto& folder : temp_newest)
        {
            if(folder > newest)
            {
                newest = folder;
            }
        }
        std::cout << "Latest version of UltraVNC is: " << newest << std::endl;

        //Find file in folder and install it
        for(auto& p : std::filesystem::directory_iterator(newest))
        {
            if (p.is_regular_file())
            {
                std::string temp(p.path().string());
                if(temp.find(".exe") != std::string::npos)
                {
                    std::cout << "Found installer: " << temp << std::endl;
                    software_list.push_back(temp);
                }
            }
        }

        std::cout << software_list.at(0) << std::endl;
    }

    /*
    for (auto& folder : r)
    {
        std::cout << "Found folder: " << folder << std::endl;

        size_t pos = folder.find_last_of("Install ") + 1;
        std::string temp = folder.substr(pos);
        std::cout << temp << std::endl;
    }
    */
}