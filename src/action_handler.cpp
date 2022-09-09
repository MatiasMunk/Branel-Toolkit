#include "action_handler.h"

ActionHandler::Handler::Handler()
{
    this->action_id = ActionID::ACTION_VOID;
    this->func = NULL;
}

ActionHandler::Handler::Handler(ActionID action_id, HandlerFunction func, std::array<intptr_t, 4> data_ptr)
{
    this->action_id = action_id;
    this->func = func;
    this->data_ptr = data_ptr;
}

void ActionHandler::Handler::Execute(Action &action)
{
    if(this->func != NULL) this->func(action, this->data_ptr);
    else throw std::runtime_error("Unknown action detected.");
}

void ActionHandler::Register(ActionID action_id, HandlerFunction func, std::array<intptr_t, 4> data_ptr)
{
    Handler handler(action_id, func, data_ptr);
    this->handlers.insert(std::pair<ActionID, Handler>(action_id, handler));
}

void ActionHandler::Unregister(ActionID action_id)
{
    auto it = this->handlers.find(action_id);

    if(it != this->handlers.end())
    {
        this->handlers.erase(it);
    }
}

void ActionHandler::Clear()
{
    this->handlers.clear();
}

void ActionHandler::Execute(Action &action)
{
    unsigned char id;
    ActionID action_id;

    action >> id;
    action_id = static_cast<ActionID>(id);

    auto it = this->handlers.find(action_id);
    if(it != this->handlers.end())
    {
        this->handlers[action_id].Execute(action);
    }
}