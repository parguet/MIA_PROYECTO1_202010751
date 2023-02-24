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

struct creacion_bloque{
    int no_bloque =-1;
    int tipo = 0;
    Structs::Fileblock archivo;
    Structs::Folderblock carpeta;
};

Structs::Structs(){
}


int bucando_inodo(Structs::Inodes InodoBuscado,vector<string> path_separado,Structs::Superblock superblock , FILE *disk_file);
creacion_bloque crear_bloque(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file,int tipo);

//---auxiliares
string obtener_caracteres(string cadena,int cantidad);
string quitar_caracteres(string cadena,int cantidad);
void update_superblock(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file);
void update_inodo_ccr(Structs::Superblock superblock,FILE *disk_file,Structs::Inodes inodo_ccr,int indice_inodo);


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

int Structs::EscrituraInodo(MountedPartition partition_montada,int indice_inodo, Structs::Superblock superblock,string cadena,FILE *disk_file ){
    //leer inode de archivo
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + indice_inodo *sizeof(Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    if(InodoBuscado.i_size == -1){
        return -1;
    }

    //obtener cantidad de bloques de archivo necesarios
    int size_cadena = cadena.size() ;
    int no_bloques  = floor(size_cadena/64) + 1;

    //comanzando a iterar el apuntadores de archivo
    int bloques_asignados = 0;
    int contador = 0;
    for (int i : InodoBuscado.i_block){
        if(no_bloques == bloques_asignados)
            break;

        if(i != -1){
            //remplazar los bloques
            if(contador<12){
                //bloques directos
                //ler blqque de archivo
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Fileblock);
                Structs::Fileblock archivo;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fread(&archivo, sizeof(Structs::Fileblock), 1, disk_file);

                //actualizar su contenido
                memset(archivo.b_content, 0, 64);
                string cadena64 = obtener_caracteres(cadena,64);
                strcpy( archivo.b_content, cadena64.c_str());
                cadena = quitar_caracteres(cadena,64);
                string text = archivo.b_content;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fwrite(&archivo, sizeof(Structs::Fileblock), 1, disk_file);
                bloques_asignados ++;
            }else{
                //bloques indirectos
            }
        }
        else{
            //crear nuevos bloques
            if(contador<12){
                //bloques directos
                //creando nuevo blqoue y actulizando la cantidad de bloques
                creacion_bloque archivo_new = crear_bloque(partition_montada,superblock,disk_file,1);
                superblock.s_free_blocks_count--;
                InodoBuscado.i_block[contador] = archivo_new.no_bloque;
                string cadena64 = obtener_caracteres(cadena,64);
                strcpy( archivo_new.archivo.b_content, cadena64.c_str());
                cadena = quitar_caracteres(cadena,64);
                string text = archivo_new.archivo.b_content;

                //escribir nuevo bloque
                int desplazamineto_bloques = superblock.s_block_start + archivo_new.no_bloque * sizeof(Structs::Fileblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fwrite(&archivo_new.archivo, sizeof(Structs::Fileblock), 1, disk_file);
                bloques_asignados ++;

                //actualizar superbloque
                update_superblock(partition_montada,superblock,disk_file);

                //actualizanr inodo
                update_inodo_ccr(superblock,disk_file,InodoBuscado,indice_inodo);
            }else{
                //blqoues indirectos
            }
        }
        contador++;
    }

    if (no_bloques != bloques_asignados){
        return  -1;
    }
    return 0;
}

//-------------------------Auxiliares---------------------------------------
string quitar_caracteres(string cadena,int cantidad){
    int cantidad_c = cadena.size();
    if( cantidad_c < cantidad){
        cadena = "";
    }else{
        for (int j = 0; j < cantidad; ++j) {
            cadena.erase(cadena.begin());
        }
    }
    return cadena;
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

void update_superblock(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file){
    fseek(disk_file, partition_montada.start, SEEK_SET);
    fwrite(&superblock, sizeof(Structs::Superblock), 1, disk_file);
}
void update_inodo_ccr(Structs::Superblock superblock,FILE *disk_file,Structs::Inodes inodo_ccr,int indice_inodo){
    fseek(disk_file, superblock.s_inode_start +indice_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fwrite(&inodo_ccr, sizeof(Structs::Inodes), 1, disk_file);
}

creacion_bloque crear_bloque(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file,int tipo){
    creacion_bloque bloque_nuevo;
    bloque_nuevo.no_bloque = superblock.s_blocks_count - superblock.s_free_blocks_count;
    bloque_nuevo.tipo = tipo;

    //actualizando bm de blqoues
    char uno = '1';
    fseek(disk_file, superblock.s_bm_block_start +  bloque_nuevo.no_bloque, SEEK_SET);
    fwrite(&uno, sizeof(uno), 1, disk_file);

    memset(  bloque_nuevo.archivo.b_content, 0, 64);
    return bloque_nuevo;
}