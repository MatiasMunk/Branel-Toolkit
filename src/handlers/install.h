#ifndef HANDLERS_INSTALL_H
#define HANDLERS_INSTALL_H

#include <array>

#include "../action_handler.h"

namespace ActionHandlers::Install
{
    void Actor(Action &action, std::array<intptr_t, 4> data_ptr);

    bool InstallGeneric(std::string install_args, std::string executable, std::string optional);
    bool InstallSQLServer19(std::string install_args);
};

#endif // HANDLERS_INSTALL_H