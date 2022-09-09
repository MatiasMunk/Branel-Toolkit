#ifndef HANDLERS_CREATE_USERS_H
#define HANDLERS_CREATE_USERS_H

#include <array>

#include "../action_handler.h"

namespace ActionHandlers::CreateUsers
{
    void Actor(Action &action, std::array<intptr_t, 4> data_ptr);
};

#endif // HANDLERS_CREATE_USERS_H