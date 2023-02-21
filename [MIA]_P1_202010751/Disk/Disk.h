//
// Created by parguet on 16/02/23.
//

#ifndef _MIA__P1_202010751_DISK_H
#define _MIA__P1_202010751_DISK_H
#include <string>
using namespace std;

class Disk{
public:
    Disk();
    void mkdisk(int size, char fit, char unit, string path);
    void rmdisk(string path);
    void fdisk(int s , char u,string path,char t,char f,string del,string name,int add);
    void rep_mbr(string id,string path);
    void rep_disk(string id, string path);
};


#endif //_MIA__P1_202010751_DISK_H
