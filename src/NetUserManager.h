#ifndef NETUSERMANAGER_H
#define NETUSERMANAGER_H

#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <lm.h>
#include <CommCtrl.h>

class NetUserManager
{
public:
	~NetUserManager(void);
	
    NET_API_STATUS CreateLocalUser(LPWSTR lpszDomain, LPWSTR lpszUser, LPWSTR lpszPassword, LPWSTR lpszComment, LPWSTR lpszLocalGroup);
    NET_API_STATUS GetLocalGroup(LPWSTR lpszLocalGroup);
    NET_API_STATUS CreateLocalGroup(LPWSTR lpszLocalGroup);
    NET_API_STATUS AddUserToGroup(LPWSTR lpszUser, LPWSTR lpszLocalGroup);

    bool CreateBranelUsers();

private:
};

#endif // NETUSERMANAGER_H