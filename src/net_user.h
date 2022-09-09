#ifndef NET_USER_H
#define NET_USER_H

#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <lm.h>
#include <CommCtrl.h>

class NetUserManager
{
public:
	~NetUserManager(void);
	
    NET_API_STATUS CreateLocalUser(LPWSTR lpszUser, LPWSTR lpszPassword, LPWSTR lpszComment);
    NET_API_STATUS GetLocalGroup(LPWSTR lpszLocalGroup);
    NET_API_STATUS CreateLocalGroup(LPWSTR lpszLocalGroup);
    NET_API_STATUS AddUserToGroup(LPWSTR lpszUser, LPWSTR lpszLocalGroup);

private:
};

#endif // NET_USER_H