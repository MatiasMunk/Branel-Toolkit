#ifndef APPLICATION_H
#define APPLICATION_H

#include "action_handler.h"
#include "action.h"

#include <memory>
#include <array>

// for managing the application and action handling (monostate)
class Application
{
private:
    static bool busy;
    static std::vector<Action> action_queue;

public:
    static ActionHandler action_handler;

    Application();
    bool Initialize();
    void Act();
    void Instruct(Action &packet);
};

#endif // APPLICATION_H