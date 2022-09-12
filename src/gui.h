#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>

#include <allegro5/allegro5.h>

#include "application.h"
#include "net_user.h"

// to put all gui forms together and process it / render (monostate)
class GUI
{
private:
    static std::string open_popup;
    static std::string popup_message;
    static std::vector<std::string> popup_data;
    static int input_focus;
    static bool did_backup;
    static bool just_installed_sql_server;
    static bool just_uninstalled;

    Application* application;

public:
    GUI();
    GUI(Application* application);
    void Process();
    void Render();
    void OpenPopup(std::string id, std::string message = "");
    void OpenPopup(std::string id, std::vector<std::string> data);
    void OpenPopup(std::string id, std::vector<std::string> data, std::string message = "");
    void SetFocus(int widget);

    void Index(); //Toolkit index
};

#endif // GUI_H
