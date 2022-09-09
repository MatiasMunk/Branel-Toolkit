#ifndef HANDLERS_INSTALL_H
#define HANDLERS_INSTALL_H

#include <array>

#include "../action_handler.h"

namespace ActionHandlers::Install
{
    void Actor(Action &action, std::array<intptr_t, 4> data_ptr);
};

#endif // HANDLERS_INSTALL_H