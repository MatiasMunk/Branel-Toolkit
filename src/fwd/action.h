#ifndef FWD_ACTION_H
#define FWD_ACTION_H

enum class ActionID : unsigned char
{
    ACTION_VOID,
    ACTION_BUSY, //Will implement later, when I'll have time to implement separate threading instances for installers
    ACTION_CREATE_USERS,
    ACTION_INSTALL,
    ACTION_UNINSTALL
};

enum class Program : unsigned char
{
    PROGRAM_NONE,
    PROGRAM_SQLSERVER19,
    PROGRAM_SQLCU,
    PROGRAM_SSMS,
    PROGRAM_TEAMVIEWER,
    PROGRAM_ULTRAVNC,
    PROGRAM_ALL
};

#endif // FWD_ACTION_H