//
// Created by parguet on 19/03/23.
//

#ifndef _MIA__P1_202010751_FILEMANAGER_H
#define _MIA__P1_202010751_FILEMANAGER_H
#include <string>
#include <vector>
using namespace std;
class FileManager {
public:
    FileManager();
    void mkfile(string path,bool r,int size,string count);
    void cat(vector<string> files);
    void remove(string path);
    void edit(string path,string cont);
    void rename(string path,string name);
    void mkdir(string path,bool p);
    void copy(string path,string destino);
    void move(string path,string destino);
    void find(string path,int tipo);
    void chown(string path,string usr,bool r);
    void chmod(string path,int ugo,bool r);
    void recovery(string id);

};


#endif //_MIA__P1_202010751_FILEMANAGER_H
