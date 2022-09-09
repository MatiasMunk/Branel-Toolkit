#include "application.h"
#include "handlers/create_users.h"
#include "handlers/install.h"
#include "handlers/uninstall.h"

#include <iostream>

bool Application::busy;
std::vector<Action> Application::action_queue;

ActionHandler Application::action_handler;

Application::Application()
{
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
        //TODO:
        //Once code is ported to instantiate a separate thread for installing,
        //this->busy should be passed as a parameter to the action handler
        //to prevent multiple installations from running at the same time
        //which is not allowed by some installers.
        this->action_queue.push_back(action);
    }
}