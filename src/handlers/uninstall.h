#ifndef HANDLERS_UNINSTALL_H
#define HANDLERS_UNINSTALL_H

#include <array>

#include "../action_handler.h"

namespace ActionHandlers::Uninstall
{
    void Actor(Action &action, std::array<intptr_t, 4> data_ptr);

    void UninstallSQL();
};

#endif // HANDLERS_UNINSTALL_H