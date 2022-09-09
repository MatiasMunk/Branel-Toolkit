#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <filesystem>

#include <allegro5/allegro5.h>

#include "ProgramManager.h"
#include "NetUserManager.h"

// to put all gui forms together and process it / render (monostate)
class GUI
{
private:
    static bool initialized_;
    static std::string open_window;
    static std::string open_popup;
    static std::string popup_message;
    static int input_focus;
    static bool did_backup;
    static bool need_restart;

    std::vector<std::string> uninstalled_programs;
    std::vector<std::string> installed_programs;
    std::vector<std::string> program_installers;

public:
    enum Program
    {
        All,
        TeamViewer,
        SQLServer19,
        SQLCU,
        SQLManagementStudio
    };

    GUI();
    void Process();
    void Render();
    void OpenWindow(std::string id);
    void OpenPopup(std::string id, std::string message = "");
    void SetFocus(int widget);

    void Index(); //Toolkit index
    void CreateBranelUsers(); //Create Branel Users
    void UninstallMSSQL(); //Uninstall MSSQL
    void InstallSoftware(Program what); //Install Software
};

#endif // GUI_H
