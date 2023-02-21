//
// Created by parguet on 20/02/23.
//

#include "Global.h"
#include <deque>
#include <vector>
#include <iostream>
using namespace std;

vector<string> loss_list;
struct Usuario usr_sesion;

int Buscar_Pmontada_name_path(string name,string path){
    int i_Pmontada = -1;
    for(int i=0; i< mounted_partitions.size() ; i++){
        if( mounted_partitions.at(i).name == name and  mounted_partitions.at(i).path == path){
            i_Pmontada = i;
            return  i_Pmontada;
        }
    }
    return i_Pmontada;
}

void mostrar_montadas(){
    for(int i=0; i< mounted_partitions.size() ; i++){
        cout<< mounted_partitions.at(i).id<<" -> "<<mounted_partitions.at(i).path<<" -> "<<mounted_partitions.at(i).name<<"\n";
    }
}

vector<string> split(string str, char del){
    string temp = "";
    vector<string> vector;
    for(int i=0; i<(int)str.size(); i++){
        if(str[i] != del){
            temp += str[i];
        }
        else{
            //cout << temp << endl;
            vector.push_back(temp);
            temp = "";
        }
    }
    vector.push_back(temp);
    return vector;
}

bool ID_loos(string id){
    for(int i=0; i< loss_list.size() ; i++){
        if( loss_list.at(i) == id ){
            return true;
        }
    }
    return false;
}