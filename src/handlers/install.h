#ifndef HANDLERS_INSTALL_H
#define HANDLERS_INSTALL_H

#include <array>

#include "../action_handler.h"

namespace ActionHandlers::Install
{
    void Actor(Action &action, std::array<intptr_t, 4> data_ptr);

    bool InstallGeneric(std::string install_args, std::string search_query, std::string include_in_search = "", std::vector<std::string> omit_files = {});
};

#endif // HANDLERS_INSTALL_H