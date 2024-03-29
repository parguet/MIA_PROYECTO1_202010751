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

struct creacion_inodo{
    int no_inodo =-1;
    int tipo = 0;
    Structs::Inodes inodo;
};

Structs::Structs(){
}


int bucando_inodo(Structs::Inodes InodoBuscado,vector<string> path_separado,Structs::Superblock superblock , FILE *disk_file);
creacion_bloque crear_bloque(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file,int tipo);
int buscar_crear_inodo(vector<string> path_separado , FILE *disk_file,MountedPartition partition_montada, int no_inodo);
creacion_inodo crear_inodo(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file,int tipo,int permisos);
void eliminar_todo(Structs::Inodes inodo_ccr, Structs::Superblock superblock , FILE *disk_file,int no_indoo);
void  eliminar_archivo(Structs::Inodes inodo_ccr, Structs::Superblock superblock , FILE *disk_file,int no_indoo);
void copiar_todo(Structs::Inodes inodo_ccr, creacion_inodo nuevo_inodo, Structs::Superblock superblock , FILE *disk_file,MountedPartition mounted_partitions,int padre );
void copiar_archivo(Structs::Inodes inodo_ccr, creacion_inodo nuevo_inodo, Structs::Superblock superblock , FILE *disk_file,MountedPartition mounted_partitions );




//---auxiliares
string obtener_caracteres(string cadena,int cantidad);
string quitar_caracteres(string cadena,int cantidad);
void update_superblock(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file);
void update_inodo_ccr(Structs::Superblock superblock,FILE *disk_file,Structs::Inodes inodo_ccr,int indice_inodo);
void update_folder_ccr(Structs::Superblock superblock,FILE *disk_file,Structs::Folderblock bloque,int indice_bloque);
Structs::Superblock crear_apuntador_directo(Structs::Superblock superblock,FILE *disk_file,MountedPartition partition_montada,Structs::Inodes InodoBuscado,int contador, int indice_inodo);
void update_archivo_ccr(Structs::Superblock superblock,FILE *disk_file,Structs::Fileblock bloque,int indice_bloque);
string ls_rep(string permisos,string Propietario,string Grupo,string Tamano,string Fecha,string Tipo,string Name,string color);


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

int Structs::Buscar_CreandoInodo(std::string path, MountedPartition partition_montada, Structs::Superblock superblock,FILE *disk_file) {
    vector<string> path_separado = split(path,'/');
    path_separado.erase(std::remove(path_separado.begin(), path_separado.end(), ""), path_separado.end());

    //comenzando busqueda en ruta
    int indice_inodo = buscar_crear_inodo(path_separado,disk_file,partition_montada,0);
    return indice_inodo;
}

int buscar_crear_inodo(vector<string> path_separado, FILE *disk_file,MountedPartition partition_montada, int no_inodo){
    int contador = 0;
    if(path_separado.size() == 0){
        return  contador;
    }

    Structs::Superblock superblock;
    fseek(disk_file, partition_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, disk_file);

    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + no_inodo* sizeof (Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    if(InodoBuscado.i_size == -1){
        return -1;
    }

    string actual = path_separado.at(0);
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
                        path_separado.erase(path_separado.begin());
                        if(path_separado.size() == 0){
                            return  0;
                        }else{
                            return  buscar_crear_inodo(path_separado,disk_file,partition_montada,x.b_inodo);
                        }
                    }
                }
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }

    int retorno = Structs::Crear_folder_inodo(partition_montada,no_inodo,superblock,actual,disk_file);
    if(retorno == -1){
        printErr("Err en creacion recursiva");
        return -1;
    }
    buscar_crear_inodo(path_separado,disk_file,partition_montada,no_inodo);
}

int Structs::Crear_folder_inodo(MountedPartition partition_montada, int indice_inodo, Structs::Superblock superblock,string nombrefolder,FILE *disk_file) {
    //se supondria que estamos en la carpeta donde creeara la nueva carpera
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + indice_inodo *sizeof(Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    int contador = 0;
    bool seactualizo = false;
    //buscar un espacio donde crear el inodo de tipo carpeta y crear el primer bloque de folder
    for (int i : InodoBuscado.i_block){
        if(i != -1){
            //si tiene bloques de carptas
            if(contador<12){
                //bloques directos
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                Structs::Folderblock carpeta;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                for (int j = 0; j < 4; ++j) {
                    if(carpeta.b_content[j].b_inodo == -1 and !seactualizo){
                        creacion_inodo inodo_new = crear_inodo(partition_montada,superblock,disk_file,0,664);
                        inodo_new.inodo.i_size = 0;
                        superblock.s_free_inodes_count--;

                        creacion_bloque archivo_new = crear_bloque(partition_montada,superblock,disk_file,0);
                        inodo_new.inodo.i_size +=  sizeof (Structs::Folderblock);
                        inodo_new.inodo.i_block[0] = archivo_new.no_bloque;
                        superblock.s_free_blocks_count--;

                        //actuallizar temporal
                        carpeta.b_content[j].b_inodo = inodo_new.no_inodo;
                        memset(carpeta.b_content[j].b_name, 0, 12);
                        strcpy(carpeta.b_content[j].b_name,nombrefolder.c_str());

                        //escribir el inodo creado
                        update_inodo_ccr(superblock,disk_file,inodo_new.inodo,inodo_new.no_inodo);

                        //escribir el folder creado
                        strcpy(archivo_new.carpeta.b_content[0].b_name, ".");
                        archivo_new.carpeta.b_content[0].b_inodo = inodo_new.no_inodo;
                        strcpy(archivo_new.carpeta.b_content[1].b_name, "..");
                        archivo_new.carpeta.b_content[1].b_inodo = indice_inodo;
                        update_folder_ccr(superblock,disk_file,archivo_new.carpeta,archivo_new.no_bloque);

                        //actualizar superbloque
                        update_superblock(partition_montada,superblock,disk_file);
                        seactualizo = true;
                    }
                }

                if(seactualizo){
                    //actualizar el bloque de folder actual
                    fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                    fwrite(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);
                    break;
                }
            }else{
                //blooques indirectos
            }
        }
        else{
            //toca crear otro bloque de carpetas
            if(contador<12){
                //bloque directo
                superblock = crear_apuntador_directo(superblock,disk_file, partition_montada,InodoBuscado,contador,indice_inodo);
                return Structs::Crear_folder_inodo(partition_montada,indice_inodo,superblock,nombrefolder,disk_file);
            }else{
                //bloques indirectors
                printErr("Debe crear blqoue indirecto");
            }
        }
        contador++;
    }

    if(seactualizo){
        return 0;
    }else{
        return -1;
    }
    return 0;
}

int Structs::Crear_txt_inodo(MountedPartition partition_montada, int indice_inodo, Structs::Superblock superblock,string cadena_txt,string nombretxt,FILE *disk_file ) {
    //se supondria que estamos en la carpeta donde creeara el archivo
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + indice_inodo *sizeof(Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    int contador = 0;
    bool seactualizo = false;
    //buscar un espacio donde crear el inodo de tipo archivo y escribir el txt
    for (int i : InodoBuscado.i_block){
        if(i != -1){
            //si tiene bloques de carptas
            if(contador<12){
                //bloques directos
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                Structs::Folderblock carpeta;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                for (int j = 0; j < 4; ++j) {
                    if(carpeta.b_content[j].b_inodo == -1 and !seactualizo){
                        creacion_inodo inodo_new = crear_inodo(partition_montada,superblock,disk_file,1,664);
                        inodo_new.inodo.i_size = 0;
                        superblock.s_free_inodes_count--;

                        creacion_bloque archivo_new = crear_bloque(partition_montada,superblock,disk_file,1);
                        inodo_new.inodo.i_size +=  sizeof(cadena_txt.c_str()) + sizeof(Structs::Fileblock);
                        inodo_new.inodo.i_block[0] = archivo_new.no_bloque;
                        superblock.s_free_blocks_count--;

                        //actuallizar temporal
                        carpeta.b_content[j].b_inodo = inodo_new.no_inodo;
                        memset(carpeta.b_content[j].b_name, 0, 12);
                        strcpy(carpeta.b_content[j].b_name,nombretxt.c_str());

                        //escribir el inodo creado
                        update_inodo_ccr(superblock,disk_file,inodo_new.inodo,inodo_new.no_inodo);

                        //escribir el archivo creado
                        update_archivo_ccr(superblock,disk_file,archivo_new.archivo,archivo_new.no_bloque);

                        //actualizar superbloque
                        update_superblock(partition_montada,superblock,disk_file);

                        //mandamos a escribir el texto al inodo
                        Structs::EscrituraInodo(partition_montada,inodo_new.no_inodo,superblock,cadena_txt,disk_file);
                        seactualizo = true;
                    }
                }

                if(seactualizo){
                    //actualizar el bloque de folder actual
                    fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                    fwrite(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);
                    break;
                }
            }else{
                //blooques indirectos
            }
        }
        else{
            //toca crear otro bloque de carpetas
            if(contador<12){
                //bloque directo
                superblock = crear_apuntador_directo(superblock,disk_file, partition_montada,InodoBuscado,contador,indice_inodo);
                return Structs::Crear_txt_inodo(partition_montada,indice_inodo,superblock,cadena_txt,nombretxt,disk_file);
            }else{
                //bloques indirectors
                printErr("Debe crear blqoue indirecto");
            }
        }
        contador++;
    }

    if(seactualizo){
        return 0;
    }else{
        return -1;
    }
}

void Structs::Update_journaling(string operacion, string path, string content,MountedPartition partition_montada, Structs::Superblock superblock,FILE *disk_file) {
    if(superblock.s_filesystem_type == 3){
        Structs::Journaling  journaling;
        fseek(disk_file, partition_montada.start + sizeof(Structs::Superblock), SEEK_SET);
        fread(&journaling, sizeof(Structs::Journaling), 1, disk_file);

        //Structs::Content_J contenido;
        strcpy(journaling.contenido[journaling.ultimo+1].operation, operacion.c_str());
        strcpy(journaling.contenido[journaling.ultimo+1].path, path.c_str());
        strcpy(journaling.contenido[journaling.ultimo+1].content, content.c_str());
        string fecha = obtenerFecha();
        strcpy(journaling.contenido[journaling.ultimo+1].date, fecha.c_str());

        journaling.ultimo++;

        fseek(disk_file, partition_montada.start + sizeof(Structs::Superblock), SEEK_SET);
        fwrite(&journaling, sizeof(Structs::Journaling), 1, disk_file);
    }
}


bool Structs::permisos(int no_inodo, MountedPartition partition_montada, FILE *disk_file,char permiso_necesario) {
    Structs::Superblock superblock;
    fseek(disk_file, partition_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, disk_file);

    Structs::Inodes inodo_ccr;
    fseek(disk_file, superblock.s_inode_start + no_inodo* sizeof (Structs::Inodes), SEEK_SET);
    fread(&inodo_ccr, sizeof(Structs::Inodes), 1, disk_file);

    string permisos_inodo = convertToString(inodo_ccr.i_perm,3);
    int permiso_usuario = int(permisos_inodo[0]) - 48;
    int permiso_grupos = int(permisos_inodo[1]) - 48;
    int permiso_otros = int(permisos_inodo[2]) - 48;

    if(inodo_ccr.i_uid == usr_sesion.uid){
        if(permiso_necesario == 'r'){
            if(permiso_usuario == 7 or permiso_usuario == 6  or permiso_usuario == 5  or permiso_usuario == 4 ){
                return true;
            }
        }
        if(permiso_necesario == 'w'){
            if(permiso_usuario == 7 or permiso_usuario == 6  or permiso_usuario == 3  or permiso_usuario == 2 ){
                return true;
            }
        }
    }

    if(inodo_ccr.i_gid == usr_sesion.gid){
        if(permiso_necesario == 'r'){
            if(permiso_grupos == 7 or permiso_grupos == 6  or permiso_grupos == 5  or permiso_grupos == 4 ){
                return true;
            }
        }
        if(permiso_necesario == 'w'){
            if(permiso_grupos == 7 or permiso_grupos == 6  or permiso_grupos == 3  or permiso_grupos == 2 ){
                return true;
            }
        }
    }

    if(permiso_necesario == 'r'){
        if(permiso_otros == 7 or permiso_otros == 6  or permiso_otros == 5  or permiso_otros == 4 ){
            return true;
        }
    }
    if(permiso_necesario == 'w'){
        if(permiso_otros == 7 or permiso_otros == 6  or permiso_otros == 3  or permiso_otros == 2 ){
            return true;
        }
    }

    //si es usuario root
    if(usr_sesion.uid == 1 and usr_sesion.gid ==1 ){
        return true;
    }

    return false;
}

void Structs::Eliminar_updateinodos(FILE *disk_file, MountedPartition partition_montada, int no_inodo,Structs::Superblock superblock) {
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + no_inodo* sizeof (Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);
    if(InodoBuscado.i_type == 0){
        eliminar_todo(InodoBuscado,superblock,disk_file,no_inodo);
    }else{
        eliminar_archivo(InodoBuscado,superblock,disk_file,no_inodo);
    }
    update_superblock(partition_montada,superblock,disk_file);
}

void eliminar_todo(Structs::Inodes inodo_ccr, Structs::Superblock superblock , FILE *disk_file,int no_indoo){
    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Folderblock carpeta;
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                int contador_carpeta = 0;
                for(Structs::Content x: carpeta.b_content){
                    if(x.b_inodo != -1 ){
                        if(x.b_name[0] != '.'){
                            Structs::Inodes Inodo_sig;
                            fseek(disk_file, superblock.s_inode_start +x.b_inodo * sizeof (Structs::Inodes), SEEK_SET);
                            fread(&Inodo_sig, sizeof(Structs::Inodes), 1, disk_file);

                            if(Inodo_sig.i_type == 0){
                                eliminar_todo(Inodo_sig,superblock,disk_file,x.b_inodo);
                            }
                            else{
                                //eliminar archivo
                                eliminar_archivo(Inodo_sig,superblock,disk_file,x.b_inodo);
                            }
                        }
                    }
                    contador_carpeta++;
                }
                //actualizar bloques
                char zero = '0';
                fseek(disk_file, superblock.s_bm_block_start + i * sizeof(zero), SEEK_SET);
                fwrite(&zero, sizeof(zero), 1, disk_file);
                superblock.s_free_blocks_count++;

            }else{
                //blooques indirectos
            }
        }
        contador++;
    }

    //actualizar inodos
    char zero = '0';
    fseek(disk_file, superblock.s_bm_inode_start + no_indoo * sizeof(zero), SEEK_SET);
    fwrite(&zero, sizeof(zero), 1, disk_file);
    superblock.s_free_inodes_count++;
}

void eliminar_archivo(Structs::Inodes inodo_ccr, Structs::Superblock superblock , FILE *disk_file,int no_indoo){
    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Fileblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Fileblock archivo;
                fread(&archivo, sizeof(Structs::Fileblock), 1, disk_file);

                //actualizar bloques
                char zero = '0';
                fseek(disk_file, superblock.s_bm_block_start + i * sizeof(zero), SEEK_SET);
                fwrite(&zero, sizeof(zero), 1, disk_file);
                superblock.s_free_blocks_count++;
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }

    //actualizar inodos
    char zero = '0';
    fseek(disk_file, superblock.s_bm_inode_start + no_indoo * sizeof(zero), SEEK_SET);
    fwrite(&zero, sizeof(zero), 1, disk_file);
    superblock.s_free_inodes_count++;
}


int Structs::Eliminar_elemnto(string nombre,FILE *disk_file,MountedPartition partition_montada,int no_inodo) {
    Structs::Superblock superblock;
    fseek(disk_file, partition_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, disk_file);

    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + no_inodo* sizeof (Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    int contador = 0;
    bool actualizado = false;
    for (int i : InodoBuscado.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                Structs::Folderblock carpeta;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fread(&carpeta, sizeof(Structs::Folderblock), 1, disk_file);

                for (int j = 0; j < 4; ++j) {
                    if(carpeta.b_content[j].b_inodo != -1 and  carpeta.b_content[j].b_name == nombre){
                        carpeta.b_content[j].b_inodo = -1;
                        memset(  carpeta.b_content[j].b_name, 0, 12);
                        actualizado = true;
                        break;
                    }
                }

                if(actualizado){
                    fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                    fwrite(&carpeta, sizeof(Structs::Folderblock), 1, disk_file);
                    return 0;
                }
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }
    return -1;
}

int Structs::Update_namefolder_inodo(MountedPartition partition_montada, int indice_inodo, Structs::Superblock superblock,string name_buscado,string name_new, FILE *disk_file) {
    //leer inode a cambiar permisos
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + indice_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    if(InodoBuscado.i_size == -1){
        return -1;
    }
    //comanzando a iterar el apuntadores de archivo
    int contador = 0;
    bool actualizado = false;
    for (int i : InodoBuscado.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                Structs::Folderblock carpeta;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fread(&carpeta, sizeof(Structs::Folderblock), 1, disk_file);

                for (int j = 0; j < 4; ++j) {
                    if(carpeta.b_content[j].b_inodo != -1 and !actualizado){
                        if(carpeta.b_content[j].b_name == name_buscado){
                            memset(  carpeta.b_content[j].b_name, 0, 12);
                            strcpy(carpeta.b_content[j].b_name,name_new.c_str());
                            actualizado = true;
                            break;
                        }
                    }
                }

                if(actualizado){
                    fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                    fwrite(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);
                    break;
                }

            }else{
                //blooques indirectos
            }
        }
        contador++;
    }

    if(actualizado){
        return 0;
    }else{
        return -1;
    }
}

int Structs::Copiar_elemento(FILE *disk_file, MountedPartition partition_montada, int no_inodo,Structs::Superblock superblock,int padre,string nombre) {
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + no_inodo* sizeof (Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    creacion_inodo nuevo_inodo = crear_inodo(partition_montada,superblock,disk_file,InodoBuscado.i_type,664);
    superblock.s_free_inodes_count--;
    int retorno = nuevo_inodo.no_inodo;

    if(InodoBuscado.i_type == 0){
        //por si pide copiar una carpeta y su contenido
        copiar_todo(InodoBuscado,nuevo_inodo,superblock,disk_file,partition_montada,padre);
    }else{
        //por si pide copiar un txt
        copiar_archivo(InodoBuscado,nuevo_inodo,superblock,disk_file,partition_montada);
    }

    update_superblock(partition_montada,superblock,disk_file);

    return Structs::Agregar_elemnto(nombre,retorno,disk_file,partition_montada,padre);
}


void copiar_todo(Structs::Inodes inodo_ccr, creacion_inodo nuevo_inodo, Structs::Superblock superblock , FILE *disk_file,MountedPartition mounted_partitions,int padre ){
    nuevo_inodo.inodo.i_size = inodo_ccr.i_size;

    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Folderblock carpeta;
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                //crear nueva carpeta
                creacion_bloque bloque_new = crear_bloque(mounted_partitions,superblock,disk_file,0);
                superblock.s_free_blocks_count--;

                //agreagr el nuevo bloque al nuevo inodo
                nuevo_inodo.inodo.i_block[contador] = bloque_new.no_bloque;

                //escribir el folder creado
                strcpy(bloque_new.carpeta.b_content[0].b_name, ".");
                bloque_new.carpeta.b_content[0].b_inodo = nuevo_inodo.no_inodo;
                strcpy(bloque_new.carpeta.b_content[1].b_name, "..");
                bloque_new.carpeta.b_content[1].b_inodo = padre;

                int contador_carpeta = 0;
                for(Structs::Content x: carpeta.b_content){
                    if(x.b_inodo != -1 ){
                        if(x.b_name[0] != '.'){
                            strncpy(bloque_new.carpeta.b_content[contador_carpeta].b_name, x.b_name, 12);

                            Structs::Inodes Inodo_sig;
                            fseek(disk_file, superblock.s_inode_start +x.b_inodo * sizeof (Structs::Inodes), SEEK_SET);
                            fread(&Inodo_sig, sizeof(Structs::Inodes), 1, disk_file);

                            if(Inodo_sig.i_type == 0){
                                creacion_inodo nuevo_inodo_sig = crear_inodo(mounted_partitions,superblock,disk_file,0,664);
                                superblock.s_free_inodes_count--;
                                copiar_todo(Inodo_sig,nuevo_inodo_sig,superblock,disk_file,mounted_partitions,nuevo_inodo.no_inodo);

                                //poner el al inodo que apuntara
                                bloque_new.carpeta.b_content[contador_carpeta].b_inodo = nuevo_inodo_sig.no_inodo;
                            }
                            else{
                                //copiar archivo
                                creacion_inodo nuevo_inodo_sig = crear_inodo(mounted_partitions,superblock,disk_file,1,664);
                                superblock.s_free_inodes_count--;
                                copiar_archivo(Inodo_sig,nuevo_inodo_sig,superblock,disk_file,mounted_partitions);

                                //poner el al inodo que apuntara
                                bloque_new.carpeta.b_content[contador_carpeta].b_inodo = nuevo_inodo_sig.no_inodo;
                            }
                        }
                    }
                    contador_carpeta++;
                }

                //actualizar bloque carpeta creada
                fseek(disk_file, superblock.s_block_start + bloque_new.no_bloque * sizeof(Structs::Folderblock), SEEK_SET);
                fwrite(&bloque_new.carpeta, sizeof(Structs::Folderblock), 1, disk_file);

            }else{
                //blooques indirectos
            }
        }
        contador++;
    }

    fseek(disk_file, superblock.s_inode_start + nuevo_inodo.no_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fwrite(&nuevo_inodo.inodo, sizeof(Structs::Inodes), 1, disk_file);
}


void copiar_archivo(Structs::Inodes inodo_ccr, creacion_inodo nuevo_inodo, Structs::Superblock superblock , FILE *disk_file,MountedPartition mounted_partitions ){
    nuevo_inodo.inodo.i_size = inodo_ccr.i_size;

    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Fileblock archivo;
                fread(&archivo, sizeof(Structs::Fileblock), 1, disk_file);

                //crear nueva carpeta
                creacion_bloque bloque_new = crear_bloque(mounted_partitions,superblock,disk_file,1);
                superblock.s_free_blocks_count--;

                strncpy(bloque_new.archivo.b_content, archivo.b_content, 64);

                //actualizar bloque archivo creada
                fseek(disk_file, superblock.s_block_start + bloque_new.no_bloque * sizeof(Structs::Fileblock), SEEK_SET);
                fwrite(&bloque_new.archivo, sizeof(Structs::Fileblock), 1, disk_file);

                //actulizar i_block
                nuevo_inodo.inodo.i_block[contador] = bloque_new.no_bloque;
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }

    fseek(disk_file, superblock.s_inode_start + nuevo_inodo.no_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fwrite(&nuevo_inodo.inodo, sizeof(Structs::Inodes), 1, disk_file);
}


int Structs::Agregar_elemnto(string nombre,int inodo_agregar, FILE *disk_file, MountedPartition partition_montada, int no_inodo) {
    Structs::Superblock superblock;
    fseek(disk_file, partition_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, disk_file);

    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + no_inodo* sizeof (Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    int contador = 0;
    bool actualizado = false;
    for (int i : InodoBuscado.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                Structs::Folderblock carpeta;
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                fread(&carpeta, sizeof(Structs::Folderblock), 1, disk_file);

                for (int j = 0; j < 4; ++j) {
                    if(carpeta.b_content[j].b_inodo == -1){
                        carpeta.b_content[j].b_inodo = inodo_agregar;
                        memset(  carpeta.b_content[j].b_name, 0, 12);
                        strcpy(carpeta.b_content[j].b_name,nombre.c_str());
                        actualizado = true;
                        break;
                    }
                }

                if(actualizado){
                    fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                    fwrite(&carpeta, sizeof(Structs::Folderblock), 1, disk_file);
                    return 0;
                }
            }else{
                //blooques indirectos
            }
        }else{
            if(contador<12){
                //apuntadores directos
                superblock = crear_apuntador_directo(superblock,disk_file, partition_montada,InodoBuscado,contador,no_inodo);
                return Structs::Agregar_elemnto(nombre,inodo_agregar,disk_file,partition_montada,no_inodo);
            }
        }
        contador++;
    }
    return -1;
}


void Structs::recorrer_find(Structs::Inodes inodo_ccr,Structs::Superblock superblock , FILE *disk_file,int profundidad,int tipo) {
    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Folderblock carpeta;
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                for(Structs::Content x: carpeta.b_content){
                    if(x.b_inodo != -1 ){
                        if(x.b_name[0] != '.'){
                            string  espcio;
                            for (int j = 0; j < profundidad ; ++j) {
                                espcio += " ";
                            }
                            espcio += "|_";

                            regex txt("[.][t|T][x|X][T|t]" );
                            string name_temp = convertToString(x.b_name,12);

                            if (regex_search(name_temp,txt) == 1){
                                if(tipo == 1){
                                    if(x.b_name[1] == '.')
                                        print(espcio + x.b_name);

                                }else{
                                    print(espcio + x.b_name);
                                }
                            }else{
                                print(espcio + x.b_name);
                            }

                            Structs::Inodes Inodo_sig;
                            fseek(disk_file, superblock.s_inode_start +x.b_inodo * sizeof (Structs::Inodes), SEEK_SET);
                            fread(&Inodo_sig, sizeof(Structs::Inodes), 1, disk_file);
                            if(Inodo_sig.i_type == 0){
                                Structs::recorrer_find(Inodo_sig,superblock,disk_file,profundidad+1,tipo);
                            }
                        }
                    }
                }
            }else{
                //blooques indirectos
            }
            contador++;
        }
    }
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


creacion_inodo crear_inodo(MountedPartition partition_montada,Structs::Superblock superblock,FILE *disk_file,int tipo,int permisos){
    creacion_inodo inodo_nuevo;
    inodo_nuevo.no_inodo = superblock.s_inodes_count - superblock.s_free_inodes_count;
    inodo_nuevo.tipo = tipo;

    //actualizando bm de inodos
    char uno = '1';
    fseek(disk_file, superblock.s_bm_inode_start +  inodo_nuevo.no_inodo, SEEK_SET);
    fwrite(&uno, sizeof(uno), 1, disk_file);

    //escribir inicializacion del inodo
    inodo_nuevo.inodo.i_uid = usr_sesion.uid;
    inodo_nuevo.inodo.i_gid = usr_sesion.gid;
    //pendiente s
    string fecha = obtenerFecha();
    strcpy( inodo_nuevo.inodo.i_atime , fecha.c_str());
    strcpy( inodo_nuevo.inodo.i_ctime , fecha.c_str());
    strcpy( inodo_nuevo.inodo.i_mtime , fecha.c_str());
    //crear bloque
    inodo_nuevo.inodo.i_type = tipo;
    string permisos_string = to_string(permisos);
    inodo_nuevo.inodo.i_perm[0] = permisos_string[0];
    inodo_nuevo.inodo.i_perm[1] = permisos_string[1];
    inodo_nuevo.inodo.i_perm[2] = permisos_string[2];
    return  inodo_nuevo;
}

void update_folder_ccr(Structs::Superblock superblock,FILE *disk_file,Structs::Folderblock bloque,int indice_bloque){
    fseek(disk_file, superblock.s_block_start + indice_bloque * sizeof(Structs::Folderblock), SEEK_SET);
    fwrite(&bloque, sizeof(Structs::Folderblock), 1, disk_file);
}

Structs::Superblock crear_apuntador_directo(Structs::Superblock superblock,FILE *disk_file,MountedPartition partition_montada,Structs::Inodes InodoBuscado,int contador, int indice_inodo){
    //obtener el numero de inodo padre
    int desplazamineto_bloques = superblock.s_block_start + 0 * sizeof(Structs::Folderblock);
    Structs::Folderblock primer_carpeta;
    fseek(disk_file, desplazamineto_bloques, SEEK_SET);
    fread(&primer_carpeta, sizeof(Structs::Fileblock), 1, disk_file);

    creacion_bloque archivo_new = crear_bloque(partition_montada,superblock,disk_file,0);
    InodoBuscado.i_block[contador] = archivo_new.no_bloque;
    superblock.s_free_blocks_count--;

    //inicializar el bloque
    strcpy(archivo_new.carpeta.b_content[0].b_name, ".");
    archivo_new.carpeta.b_content[0].b_inodo = indice_inodo;
    strcpy(archivo_new.carpeta.b_content[1].b_name, "..");
    archivo_new.carpeta.b_content[1].b_inodo = primer_carpeta.b_content[1].b_inodo;
    update_folder_ccr(superblock,disk_file,archivo_new.carpeta,archivo_new.no_bloque);

    //actualizar superbloque
    update_superblock(partition_montada,superblock,disk_file);

    //actulizamos el inodo
    update_inodo_ccr(superblock,disk_file,InodoBuscado,indice_inodo);

    //escribimos el nuevo bloque crearo
    desplazamineto_bloques = superblock.s_block_start + archivo_new.no_bloque * sizeof(Structs::Folderblock);
    fseek(disk_file, desplazamineto_bloques, SEEK_SET);
    fwrite(&archivo_new.carpeta, sizeof(Structs::Folderblock), 1, disk_file);

    return superblock;
}

void update_archivo_ccr(Structs::Superblock superblock,FILE *disk_file,Structs::Fileblock bloque,int indice_bloque){
    fseek(disk_file, superblock.s_block_start + indice_bloque * sizeof(Structs::Fileblock), SEEK_SET);
    fwrite(&bloque, sizeof(Structs::Folderblock), 1, disk_file);
}

int Structs::Update_own_inodo(int indice_inodo, Structs::Superblock superblock,int usr, int grp,FILE *disk_file) {
    //leer inode a cambiar permisos
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + indice_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    if(InodoBuscado.i_size == -1){
        return -1;
    }
    InodoBuscado.i_uid = usr;
    InodoBuscado.i_gid = grp;
    update_inodo_ccr(superblock,disk_file,InodoBuscado,indice_inodo);
    return 0;
}

int Structs::cambiar_propietario(Structs::Superblock superblock , FILE *disk_file,int no_inodo,int usr, int grp){
    Structs::Inodes inodo_ccr;
    fseek(disk_file, superblock.s_inode_start + no_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fread(&inodo_ccr, sizeof(Structs::Inodes), 1, disk_file);

    int retorno = Structs::Update_own_inodo(no_inodo,superblock,usr,grp,disk_file);
    if(retorno == -1){
        printErr("Ocurrio un error al actaulizar propietario recursivamente");
        return -1;
    }

    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Folderblock carpeta;
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                for(Structs::Content x: carpeta.b_content){
                    if(x.b_inodo != -1 ){
                        if(x.b_name[0] != '.'){
                            Structs::Inodes Inodo_sig;
                            fseek(disk_file, superblock.s_inode_start +x.b_inodo * sizeof (Structs::Inodes), SEEK_SET);
                            fread(&Inodo_sig, sizeof(Structs::Inodes), 1, disk_file);
                            if(Inodo_sig.i_type == 0){
                                int retorno_axu = Structs::cambiar_propietario(superblock,disk_file,x.b_inodo,usr,grp);
                                if(retorno_axu == -1){
                                    printErr("Ocurrio un error al actaulizar propietario recursivamente");
                                    return -1;
                                }
                            }else{
                                int retorno_axu = Structs::Update_own_inodo(x.b_inodo,superblock,usr,grp,disk_file);
                                if(retorno_axu == -1){
                                    printErr("Ocurrio un error al actaulizar propietario recursivamente");
                                    return -1;
                                }
                            }
                        }
                    }
                }
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }
    return 0;
}

int Structs::Update_perm_inodo( int indice_inodo, Structs::Superblock superblock,int permisos,FILE *disk_file) {
    //leer inode a cambiar permisos
    Structs::Inodes InodoBuscado;
    fseek(disk_file, superblock.s_inode_start + indice_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fread(&InodoBuscado, sizeof(Structs::Inodes), 1, disk_file);

    if(InodoBuscado.i_size == -1){
        return -1;
    }
    string permisos_inodo = to_string(permisos);
    InodoBuscado.i_perm[0] = permisos_inodo[0];
    InodoBuscado.i_perm[1] = permisos_inodo[1];
    InodoBuscado.i_perm[2] = permisos_inodo[2];

    update_inodo_ccr(superblock,disk_file,InodoBuscado,indice_inodo);
    return 0;
}

int Structs::cambiar_permisos(Structs::Superblock superblock , FILE *disk_file,int no_inodo,int permisos){
    Structs::Inodes inodo_ccr;
    fseek(disk_file, superblock.s_inode_start + no_inodo * sizeof(Structs::Inodes), SEEK_SET);
    fread(&inodo_ccr, sizeof(Structs::Inodes), 1, disk_file);

    int retorno = Structs::Update_perm_inodo(no_inodo,superblock,permisos,disk_file);
    if(retorno == -1){
        printErr("Ocurrio un error al actaulizar permisos recursivamente");
        return -1;
    }

    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Folderblock carpeta;
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                for(Structs::Content x: carpeta.b_content){
                    if(x.b_inodo != -1 ){
                        if(x.b_name[0] != '.'){
                            Structs::Inodes Inodo_sig;
                            fseek(disk_file, superblock.s_inode_start +x.b_inodo * sizeof (Structs::Inodes), SEEK_SET);
                            fread(&Inodo_sig, sizeof(Structs::Inodes), 1, disk_file);
                            if(Inodo_sig.i_type == 0){
                                int retorno_axu = Structs::cambiar_permisos(superblock,disk_file,x.b_inodo,permisos);
                                if(retorno_axu == -1){
                                    printErr("Ocurrio un error al actaulizar permisos recursivamente");
                                    return -1;
                                }
                            }else{
                                int retorno_axu = Structs::Update_perm_inodo(x.b_inodo,superblock,permisos,disk_file);
                                if(retorno_axu == -1){
                                    printErr("Ocurrio un error al actaulizar permisos recursivamente");
                                    return -1;
                                }
                            }
                        }
                    }
                }
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }
    return 0;
}


//rep

string Structs::rep_ls(int no_inodo, MountedPartition partition_montada, FILE *disk_file) {
    string DOT = "";
    Structs::Superblock superblock;
    fseek(disk_file, partition_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, disk_file);

    Structs::Inodes inodo_ccr;
    fseek(disk_file, superblock.s_inode_start + no_inodo* sizeof (Structs::Inodes), SEEK_SET);
    fread(&inodo_ccr, sizeof(Structs::Inodes), 1, disk_file);

    int contador = 0;
    for (int i : inodo_ccr.i_block){
        if(i != -1){
            //bloques directos
            if(contador<12){
                int desplazamineto_bloques = superblock.s_block_start + i * sizeof(Structs::Folderblock);
                fseek(disk_file, desplazamineto_bloques, SEEK_SET);
                Structs::Folderblock carpeta;
                fread(&carpeta, sizeof(Structs::Fileblock), 1, disk_file);

                for(Structs::Content x: carpeta.b_content){
                    if(x.b_inodo != -1 ){
                        if(x.b_name[0] != '.'){

                            print(x.b_name);
                            Structs::Inodes Inodo_sig;
                            fseek(disk_file, superblock.s_inode_start +x.b_inodo * sizeof (Structs::Inodes), SEEK_SET);
                            fread(&Inodo_sig, sizeof(Structs::Inodes), 1, disk_file);
                            int tipo = 1;
                            if(Inodo_sig.i_type == 0){
                                tipo =0;
                            }
                            string fecha = convertToString(Inodo_sig.i_mtime,17);
                            string name = convertToString(x.b_name,12);
                            string permisos = convertToString(Inodo_sig.i_perm,3);
                            DOT += ls_rep(permisos, to_string(Inodo_sig.i_uid), to_string(Inodo_sig.i_gid),to_string(Inodo_sig.i_size),fecha, to_string(tipo),name,"#57fcc3");
                        }
                    }
                }
            }else{
                //blooques indirectos
            }
        }
        contador++;
    }

    return DOT;
}

string ls_rep(string permisos,string Propietario,string Grupo,string Tamano,string Fecha,string Tipo,string Name,string color){
    string DOT = "<TR>\n"
                 "  <TD bgcolor=\"" + color + "\">" +permisos + "</TD>\n"
                                                                "  <TD bgcolor=\"" + color + "\">" + Propietario + "</TD>\n"
                                                                                                                   "  <TD bgcolor=\"" + color + "\">" + Grupo + "</TD>\n"
                                                                                                                                                                "  <TD bgcolor=\"" + color + "\">" + Tamano + "</TD>\n"
                                                                                                                                                                                                              "  <TD bgcolor=\"" + color + "\">" + Fecha + "</TD>\n"
                                                                                                                                                                                                                                                           "  <TD bgcolor=\"" + color + "\">" + Tipo + "</TD>\n"
                                                                                                                                                                                                                                                                                                       "  <TD bgcolor=\"" + color + "\">"+Name+"</TD>\n"
                                                                                                                                                                                                                                                                                                                                               "  </TR>\n\n";
    return DOT;
}