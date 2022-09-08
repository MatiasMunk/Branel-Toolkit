#ifndef LOCALUSER_H
#define LOCALUSER_H

#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <lm.h>
#include <CommCtrl.h>

class LocalUser
{
public:
	~LocalUser(void);
	
    NET_API_STATUS CreateLocalUser(LPWSTR lpszDomain, LPWSTR lpszUser, LPWSTR lpszPassword, LPWSTR lpszComment, LPWSTR lpszLocalGroup);
    NET_API_STATUS GetLocalGroup(LPWSTR lpszLocalGroup);
    NET_API_STATUS CreateLocalGroup(LPWSTR lpszLocalGroup);
    NET_API_STATUS AddUserToGroup(LPWSTR lpszUser, LPWSTR lpszLocalGroup);

private:
};

#endif //LOCALUSER_H