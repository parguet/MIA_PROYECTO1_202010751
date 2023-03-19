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
    void mount(string path, string name);
    void unmount(string id);
    void bm_inode(string id,string path);
    void bm_block(string id,string path);
    void inode(string id,string path);
    void block(string id,string path);
    void sb(string id,string path);
    void tree(string id,string path);
    void journaling(string id,string path);
    void file(string id, string path, string ruta);
    void ls(string id, string path, string ruta);

    void rep_mbr(string id,string path);
    void rep_disk(string id, string path);
};


#endif //_MIA__P1_202010751_DISK_H
