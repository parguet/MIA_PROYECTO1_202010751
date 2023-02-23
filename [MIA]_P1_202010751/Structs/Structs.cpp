//
// Created by parguet on 16/02/23.
//

#include "Structs.h"
#include "../Extras/Extras.h"
#include "../Global/Global.h"
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <regex>
using namespace std;
Structs::Structs(){
}


int bucando_inodo(Structs::Inodes InodoBuscado,vector<string> path_separado,Structs::Superblock superblock , FILE *disk_file);


//---auxiliares
string obtener_caracteres(string cadena,int cantidad);

//----------------------------- Metodos con codigo ------------------------------------------

int Structs::BuscarInodo(string path,MountedPartition partition_montada,Superblock superblock,FILE *disk_file){
    vector<string> path_separado = split(path,'/');
    path_separado.erase(std::remove(path_separado.begin(), path_separado.end(), ""), path_separado.end());

    //leendo primer inodo
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start, SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    if(InodoBuscado.i_size == -1){
        return -1;
    }

    //comenzando busqueda en ruta
    int indice_inodo = bucando_inodo(InodoBuscado,path_separado,superblock,disk_file);
    return indice_inodo;
}


int bucando_inodo(Structs::Inodes InodoBuscado,vector<string> path_separado,Structs::Superblock superblock , FILE *disk_file){
    int contador = 0;
    if(path_separado.size() == 0){
        return  contador;
    }

    string actual = path_separado.at(0);
    path_separado.erase(path_separado.begin());
    for (int i : InodoBuscado.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Folderblock carpeta;
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                for(Structs::Content x: carpeta.b_content){
                    if(x.b_inodo != -1 and x.b_name == actual){
                        if(path_separado.size() == 0){
                            return  x.b_inodo;
                        }else{
                            //leendo sig inodo
                            Structs::Inodes Inodo_sig;
                            fseek(disk_file, superblock.s_inode_start +x.b_inodo * sizeof (Structs::Inodes), SEEK_SET);
                            fread(&Inodo_sig, sizeof(Structs::Inodes), 1, disk_file);
                            return bucando_inodo(Inodo_sig,path_separado,superblock,disk_file);
                        }
                    }
                }
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }
    return -1;
}


string Structs::LecturaInodo(int indice_inodo, Structs::Superblock superblock,FILE *disk_file){
    string retorno;

    //leer inode de archivo
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + indice_inodo *sizeof(Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    if(InodoBuscado.i_size == -1){
        return retorno;
    }

    //comanzando a iterar el apuntadores de archivo
    int contador = 0;
    for (int i : InodoBuscado.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Fileblock);
                Structs::Fileblock archivo;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fread(&archivo, sizeof(Structs::Fileblock), 1, disk_file);
                string lectura64 = archivo.b_content;
                lectura64 = obtener_caracteres(lectura64,64);
                retorno += lectura64;
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }
    return retorno;
}


string obtener_caracteres(string cadena,int cantidad){
    string retorno;
    int cantidad_c = cadena.size();
    if( cantidad_c < cantidad){
        retorno= cadena;
    }else{
        for (int j = 0; j < cantidad; ++j) {
            retorno += cadena[0];
            cadena.erase(cadena.begin());
        }
    }
    return retorno;
}