#include "create_users.h"

#include "../net_user.h"
#include "../gui.h"

#include <iostream>
#include <filesystem>

namespace ActionHandlers::CreateUsers
{
    void Actor(Action &action, std::array<intptr_t, 4> data_ptr)
    {
        wchar_t* user_group;
        wchar_t* admin_group;
        std::string password = "";

        action >> password;

        try
        {
            //Remove standard profile picture from Windows User Account Data folder
            if(std::filesystem::exists("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.png"))
                std::filesystem::remove("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.png");
            if(std::filesystem::exists("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.bmp"))
                std::filesystem::remove("C:\\ProgramData\\Microsoft\\User Account Pictures\\user.bmp");

            //Copy Branel profile picture to Windows User Account Data folder
            std::filesystem::copy("data/user.png", "C:\\ProgramData\\Microsoft\\User Account Pictures\\user.png", std::filesystem::copy_options::overwrite_existing);
            std::filesystem::copy("data/user.bmp", "C:\\ProgramData\\Microsoft\\User Account Pictures\\user.bmp", std::filesystem::copy_options::overwrite_existing);

            NetUserManager user_manager;
            
            if(user_manager.GetLocalGroup(const_cast<wchar_t*>(L"Brugere")) == 0)
            {
                user_group = const_cast<wchar_t*>(L"Brugere");
                admin_group = const_cast<wchar_t*>(L"Administratorer");
            }
            else
            {
                user_group = const_cast<wchar_t*>(L"Users");
                admin_group = const_cast<wchar_t*>(L"Administrators");
            }

            wchar_t bradmin_user[] = L"BrAdmin";
            wchar_t bradmin_pass[] = L"Fiber.5015";

            wchar_t brguest_user[] = L"BrGuest";
            //Convert the password to a LPCWSTR
            std::wstring wpassword = std::wstring(password.begin(), password.end());
            //Convert the const wide string to a LPWSTR
            wchar_t* brguest_pass = const_cast<wchar_t*>(wpassword.c_str());

            wchar_t shared_desc[] = L"Branel IT";

            //BrAdmin
            user_manager.CreateLocalUser(bradmin_user, bradmin_pass, shared_desc);
            user_manager.AddUserToGroup(bradmin_user, user_group);
            user_manager.AddUserToGroup(bradmin_user, admin_group);

            //BrGuest
            user_manager.CreateLocalUser(brguest_user, brguest_pass, shared_desc);
            user_manager.AddUserToGroup(brguest_user, user_group);
            user_manager.AddUserToGroup(brguest_user, admin_group);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            GUI().OpenPopup("Insufficient privileges", "Did run the program as administrator?");
        }
    }
}