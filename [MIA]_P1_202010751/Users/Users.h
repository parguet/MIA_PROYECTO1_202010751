//
// Created by parguet on 16/03/23.
//

#ifndef _MIA__P1_202010751_USERS_H
#define _MIA__P1_202010751_USERS_H
#include <iostream>
using namespace std;

class Users {
public:
    Users();

    void login(string usr, string pass, string id);
    void logout();
    void mkgrp(string name);
    void rmgrp(string name);
    void mkusr(string usr, string pass, string grp);
    void rmusr(string name);
    void chgrp(string user,string grp);
};

#endif //_MIA__P1_202010751_USERS_H
