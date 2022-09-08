#include "LocalUser.h"

LocalUser::~LocalUser(void)
{
}

//CreateLocalUser
NET_API_STATUS LocalUser::CreateLocalUser(  LPWSTR lpszDomain,
                                            LPWSTR lpszUser,
                                            LPWSTR lpszPassword,
                                            LPWSTR lpszComment,
                                            LPWSTR lpszLocalGroup )
{
    USER_INFO_1                 user_info;
    LOCALGROUP_INFO_1           localgroup_info;
    LOCALGROUP_MEMBERS_INFO_3   localgroup_members;
    LPWSTR                      lpszPrimaryDC = NULL;
    NET_API_STATUS              nStatus = 0;
    DWORD                       dwLevel = 1;
    DWORD                       dwError = 0;

    ZeroMemory(&user_info, sizeof(user_info));

    user_info.usri1_name         = lpszUser;            //UserName
    user_info.usri1_password     = lpszPassword;        //PassWord
    //user_info.usri1_full_name    = (LPWSTR)"Branel Admin";
    user_info.usri1_comment      = lpszComment;
    user_info.usri1_priv         = USER_PRIV_USER;
    user_info.usri1_home_dir     = NULL;
    user_info.usri1_flags        = UF_SCRIPT | UF_DONT_EXPIRE_PASSWD | UF_PASSWD_CANT_CHANGE;
    user_info.usri1_script_path  = NULL;

    nStatus = NetUserAdd(NULL, dwLevel, (LPBYTE)&user_info, &dwError);

    switch(nStatus) 
    {
        case NERR_Success:
        {
            fprintf(stderr, "User added successfully!\n");
            break;
        }
        case NERR_InvalidComputer:
        {
            fprintf(stderr, "A system error has occurred: NERR_InvalidComputer\n");
            break;
        }
        case NERR_NotPrimary:
        {
            fprintf(stderr, "A system error has occurred: NERR_NotPrimary\n");
            break;
        }
        case NERR_GroupExists:
        {
            fprintf(stderr, "A system error has occurred: NERR_GroupExists\n");
            break;
        }
        case NERR_UserExists:
        {
            printf("User already exists.\n");
            nStatus = 0;
            break;
        }
        case NERR_PasswordTooShort:
        {
            fprintf(stderr, "A system error has occurred: NERR_PasswordTooShort\n");
            break;
        }
        case ERROR_INVALID_PARAMETER:
        {
            printf("Invalid parameter error adding user; parameter index = %d\n", dwError);
            NetApiBufferFree( lpszPrimaryDC );
            return nStatus;
            break;
        }
        case ERROR_ACCESS_DENIED:
        {
            fprintf(stderr, "A system error has occurred: ERROR_ACCESS_DENIED. Parameter index = %d\n", dwError);
            NetApiBufferFree( lpszPrimaryDC );
            return nStatus;
            break;
        }
        default:
        {
            printf("Error adding user: %d\n", nStatus);
            NetApiBufferFree( lpszPrimaryDC );
            return nStatus;
            break;
        }
    }

    NetApiBufferFree( lpszPrimaryDC );
    return( nStatus );
}

NET_API_STATUS LocalUser::GetLocalGroup(LPWSTR lpszLocalGroup)
{
    TCHAR szGroupName[UNLEN];
    INT iItem;
    HWND hwndLV;

    LOCALGROUP_INFO_1           localgroup_info;
    LOCALGROUP_MEMBERS_INFO_3   localgroup_members;
    LPWSTR                      lpszPrimaryDC = NULL;
    NET_API_STATUS              err = 0;
    DWORD                       dwLevel = 1;
    DWORD                       dwError = 0;

    err = NetLocalGroupGetInfo(NULL, lpszLocalGroup, dwLevel, (LPBYTE*)&localgroup_info);

    switch(err)
    {
        case NERR_Success:
        {
            wprintf(L"Local group %ls exists.\n", lpszLocalGroup);
            break;
        }
        case NERR_GroupNotFound:
        {
            printf("Local group %s does not exist: %d\n", lpszLocalGroup, err);
            break;
        }
        default:
        {
            printf("Error getting local group info: %d\n", err);
            return err;
            break;
        }
    }
    
    NetApiBufferFree( lpszPrimaryDC );
    return( err );
}

NET_API_STATUS LocalUser::CreateLocalGroup( LPWSTR lpszLocalGroup )
{
    LOCALGROUP_INFO_1           localgroup_info;
    LPWSTR                      lpszPrimaryDC = NULL;
    NET_API_STATUS              err = 0;
    DWORD                       dwLevel = 1;
    DWORD                       dwError = 0;

    memset(&localgroup_info, 0, sizeof(localgroup_info));

    localgroup_info.lgrpi1_name         = lpszLocalGroup;            //LocalGroupName

    err=NetLocalGroupAdd(NULL, dwLevel, (LPBYTE)&localgroup_info, &dwError);

    switch(err) 
    {
        case NERR_Success:
        {
            fprintf(stderr, "Local Group added successfully!\n");
            break;
        }
        case NERR_InvalidComputer:
        {
            fprintf(stderr, "A system error has occurred: NERR_InvalidComputer\n");
            break;
        }
        case NERR_NotPrimary:
        {
            fprintf(stderr, "A system error has occurred: NERR_NotPrimary\n");
            break;
        }
        case ERROR_ALIAS_EXISTS:
        {
            printf("Local Group already exists.\n");
            break;
        }
        case NERR_GroupExists:
        {
            printf("Local Group already exists.\n");
            err = 0;
            break;
        }
        case ERROR_INVALID_PARAMETER:
        {
            printf("Invalid parameter error adding Local Group; parameter index = %d\n", dwError);
            NetApiBufferFree( lpszPrimaryDC );
            return err;
            break;
        }
        case ERROR_ACCESS_DENIED:
        {
            fprintf(stderr, "A system error has occurred: ERROR_ACCESS_DENIED. Parameter index = %d\n", dwError);
            NetApiBufferFree( lpszPrimaryDC );
            return err;
            break;
        }
        default:
        {
            printf("Error adding Local Group: %d\n", err);
            NetApiBufferFree( lpszPrimaryDC );
            return err;
            break;
        }
    }

    NetApiBufferFree( lpszPrimaryDC );
    return( err );
}

NET_API_STATUS LocalUser::AddUserToGroup( LPWSTR lpszUser,
                                          LPWSTR lpszLocalGroup )
{
    LOCALGROUP_MEMBERS_INFO_3   localgroup_members;
    LPWSTR                      lpszPrimaryDC = NULL;
    NET_API_STATUS              err = 0;
    DWORD                       dwLevel = 3;
    DWORD                       dwError = 0;

    ZeroMemory(&localgroup_members, sizeof(localgroup_members));

    localgroup_members.lgrmi3_domainandname = lpszUser;            //UserName

    err=NetLocalGroupAddMembers(NULL, lpszLocalGroup, dwLevel, (LPBYTE)&localgroup_members, 1);

    switch(err) 
    {
        case NERR_Success:
        {
            fprintf(stderr, "User added to Local Group successfully!\n");
            break;
        }
        case ERROR_MEMBER_IN_ALIAS:
        {
            printf("User already exists in Local Group.\n");
            err = 0;
            break;
        }
        default:
        {
            printf("Error adding user: %d\n", err);
            NetApiBufferFree( lpszPrimaryDC );
            return err;
            break;
        }
    }

    NetApiBufferFree( lpszPrimaryDC );
    return( err );
}