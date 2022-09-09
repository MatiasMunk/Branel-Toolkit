#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <array>
#include <map>
#include <functional>
#include <stdexcept>

#include "action.h"

typedef std::function<void(Action &action, std::array<intptr_t, 4> data_ptr)> HandlerFunction;

class ActionHandler
{
public:
    struct Handler
    {
        ActionID action_id;
        HandlerFunction func;
        std::array<intptr_t, 4> data_ptr;

        Handler();
        Handler(ActionID action_id, HandlerFunction func, std::array<intptr_t, 4> data_ptr);
        void Execute(Action &action);
    };

public:
    std::map<ActionID, Handler> handlers;

    void Register(ActionID id, HandlerFunction func, std::array<intptr_t, 4> data_ptr);
    void Unregister(ActionID id);
    void Clear();
    void Execute(Action &action);
};

#endif // ACTION_HANDLER_H