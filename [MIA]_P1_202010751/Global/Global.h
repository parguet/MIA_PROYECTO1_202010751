//
// Created by parguet on 20/02/23.
//

#ifndef _MIA__P1_202010751_GLOBAL_H
#define _MIA__P1_202010751_GLOBAL_H
#include "string"
#include <deque>
#include <vector>
using namespace std;

struct MountedPartition {
    string name;
    string path;
    char type;
    string id;
    int start;
    int size;
    char status;
};

struct Usuario {
    int uid = -1;
    int gid = -1;
    string grp;
    string usr;
    string pass;
    string pid;
};

extern struct Usuario usr_sesion;
extern deque<MountedPartition> mounted_partitions;
extern vector<string> loss_list;

bool ID_loos(string id);
int Buscar_Pmontada_id(string id);
int Buscar_Pmontada_name_path(string name,string path);
void mostrar_montadas();
vector<string> split(string str, char del);

#endif //_MIA__P1_202010751_GLOBAL_H
