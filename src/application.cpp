#include "application.h"

#include "handlers/create_users.h"
#include "handlers/install.h"
#include "handlers/uninstall.h"

#include "gui.h"

#include <iostream>

std::vector<Action> Application::action_queue;

ActionHandler Application::action_handler;
bool Application::busy;

Application::Application()
{
    this->busy = false;
    this->Initialize();
}

bool Application::Initialize()
{
    //Check if data files exist in data/ directory
    //if non existant, notify user.

    std::array<intptr_t, 4> ptr;
    ptr[0] = (intptr_t)this;

    this->action_handler.Register(ActionID::ACTION_CREATE_USERS, ActionHandlers::CreateUsers::Actor, ptr);
    this->action_handler.Register(ActionID::ACTION_INSTALL, ActionHandlers::Install::Actor, ptr);
    this->action_handler.Register(ActionID::ACTION_UNINSTALL, ActionHandlers::Uninstall::Actor, ptr);

    std::cout << "Application started" << std::endl;

    return true;
}

void Application::Act()
{
    Action action;

    if(!this->action_queue.empty())
    {
        action = this->action_queue[0];
        this->action_queue.erase(this->action_queue.begin());

        this->action_handler.Execute(action);
    }
}

//Build action and add to queue
//Call Instruct() in e.g. a GUI event handler
void Application::Instruct(Action &action)
{
    if(!this->busy)
    {
        this->action_queue.push_back(action);
    }
    else
    {
        GUI().OpenPopup("Error", "Another setup is already in progress.\nPlease wait for it to finish.");
    }
}