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


};


#endif //_MIA__P1_202010751_FILEMANAGER_H
