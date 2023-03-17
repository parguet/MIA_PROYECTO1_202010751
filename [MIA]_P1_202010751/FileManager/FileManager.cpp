//
// Created by parguet on 19/03/23.
//

#include "FileManager.h"
#include "../Extras//Extras.h"
#include "../Global//Global.h"
#include "../Structs//Structs.h"
#include <string>
#include <vector>
#include <regex>
using namespace std;

FileManager::FileManager() {
}

void FileManager::mkfile(string path, bool r, int size,string count) {
    if(usr_sesion.uid == -1){
        printErr("Se necesita una sesion iniciada");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    vector<string> div_path = split(path,'/');
    div_path.erase(div_path.begin());
    string nametxt = div_path.at(div_path.size()-1);
    div_path.pop_back();
    //para buscar el ultimo folder indicado
    string path_recortado;
    for (int i = 0; i < div_path.size(); ++i) {
        path_recortado += '/' +  div_path.at(i);
    }

    if(nametxt.size() > 12){
        printErr("Err el nombre del archivo debe ser menor a 12 caracteres");
        fclose(rfile);
        return;
    }

    if(count.empty()){
        string data = "0123456789";
        int indice = 0;
        string cadena_escribir = "";
        for (int i = 0; i < size; ++i) {
            cadena_escribir += data[indice];
            if(indice == 9){
                indice = 0;
            }else{
                indice++;
            }
        }

        if(r){
            int no_inodo = Structs::Buscar_CreandoInodo(path_recortado,particion_montada,superblock,rfile);
            if(no_inodo == -1){
                printErr("Hubo un error en la creacion del archivo, error de path r");
                fclose(rfile);
                return;
            }
            fseek(rfile, particion_montada.start, SEEK_SET);
            fread(&superblock, sizeof(Structs::Superblock), 1, rfile);
        }

        int no_inodo = Structs::BuscarInodo(path_recortado,particion_montada,superblock,rfile);
        if(no_inodo == -1){
            printErr("Hubo un error en la creacion del archivo");
            fclose(rfile);
            return;
        }
        int retorno = Structs::Crear_txt_inodo(particion_montada,no_inodo,superblock,cadena_escribir,nametxt,rfile);
        if(retorno==-1){
            printErr("NO se pudo crear el archivo");
            fclose(rfile);
            return;
        }

        if(r){
            Structs::Update_journaling("mkfile -r",path,cadena_escribir,particion_montada,superblock,rfile);
        }else{
            Structs::Update_journaling("mkfile",path,cadena_escribir,particion_montada,superblock,rfile);
        }

        printExitoso("Se ejecuto correctamente mkfile");
    }
    else{
        //si viene parametro count
        int no_inodo_count = Structs::BuscarInodo(count,particion_montada,superblock,rfile);
        if(no_inodo_count == -1){
            printErr("No se encontro carpeta para count");
            fclose(rfile);
            return;
        }
        string cadena_leida = Structs::LecturaInodo(no_inodo_count,superblock,rfile);

        if(r){
            int no_inodo = Structs::Buscar_CreandoInodo(path_recortado,particion_montada,superblock,rfile);
            if(no_inodo == -1){
                printErr("Hubo un error en la creacion del archivo, error de path r");
                fclose(rfile);
                return;
            }
            fseek(rfile, particion_montada.start, SEEK_SET);
            fread(&superblock, sizeof(Structs::Superblock), 1, rfile);
        }

        int no_inodo = Structs::BuscarInodo(path_recortado,particion_montada,superblock,rfile);
        if(no_inodo == -1){
            printErr("Hubo un error en la creacion del archivo");
            fclose(rfile);
            return;
        }
        int retorno = Structs::Crear_txt_inodo(particion_montada,no_inodo,superblock,cadena_leida,nametxt,rfile);
        if(retorno==-1){
            printErr("NO se pudo crear el archivo");
            fclose(rfile);
            return;
        }
        if(r){
            Structs::Update_journaling("mkfile -r",path,count,particion_montada,superblock,rfile);
        }else{
            Structs::Update_journaling("mkfile",path,count,particion_montada,superblock,rfile);
        }
        printExitoso("Se ejecuto correctamente mkfile");
    }
    fclose(rfile);
}

void FileManager::cat(vector<string> files) {
    if(usr_sesion.uid == -1){
        printErr("Se necesita una sesion iniciada");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    for (int i = 0; i < files.size(); ++i) {
        string rutatxt = files.at(i);
        int no_inodo = Structs::BuscarInodo(rutatxt,particion_montada,superblock,rfile);

        if(no_inodo == -1){
            printErr("No se encontro inodo");
            fclose(rfile);
            return;
        }

        if(!Structs::permisos(no_inodo,particion_montada,rfile,'r')){
            printErr("El usuario actual no tiene permisos para ejecutar este comando");
            return;
        }

        string cadena_leida = Structs::LecturaInodo(no_inodo,superblock,rfile);
        printExitoso(cadena_leida);
    }
    printExitoso("Se ejecuto correctamente cat");
}

void FileManager::remove(string path) {
    if(usr_sesion.uid == -1 ){
        printErr("Se necesita una sesion iniciada ");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    int no_inodo_inicio = Structs::BuscarInodo(path,particion_montada,superblock,rfile);
    if(no_inodo_inicio == -1){
        printErr("No se encontro carpeta para remover");
        fclose(rfile);
        return;
    }

    vector<string> div_path = split(path,'/');
    div_path.erase(div_path.begin());
    string name_origen = div_path.at(div_path.size()-1);
    div_path.pop_back();
    //para buscar el ultimo folder indicado
    string path_recortado_origen;
    for (int i = 0; i < div_path.size(); ++i) {
        path_recortado_origen += '/' +  div_path.at(i);
    }

    int no_inodo = Structs::BuscarInodo(path,particion_montada,superblock,rfile);
    if(no_inodo == -1){
        printErr("No se encontro la carpeta/archivo a eliminar ");
        fclose(rfile);
        return;
    }

    Structs::Eliminar_updateinodos(rfile,particion_montada,no_inodo,superblock);

    int padre_origen = Structs::BuscarInodo(path_recortado_origen,particion_montada,superblock,rfile);
    int retorno_eliminar = Structs::Eliminar_elemnto(name_origen,rfile,particion_montada,padre_origen);
    if(retorno_eliminar == -1){
        printErr("No se pudo eliminar el elemento");
        fclose(rfile);
        return;
    }

    Structs::Update_journaling("delete",path,"-",particion_montada,superblock,rfile);
    fclose(rfile);
    printExitoso("Se ejecuto correctamente remove");
}

void FileManager::edit(string path, string cont) {
    if(usr_sesion.uid == -1 ){
        printErr("Se necesita una sesion iniciada ");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);


    int no_inodo_cont = Structs::BuscarInodo(cont,particion_montada,superblock,rfile);
    if(no_inodo_cont == -1){
        printErr("No se encontro archivo de cont");
        fclose(rfile);
        return;
    }
    string cadena_leida = Structs::LecturaInodo(no_inodo_cont,superblock,rfile);

    int no_inodo = Structs::BuscarInodo(path,particion_montada,superblock,rfile);
    if(no_inodo == -1){
        printErr("No se encontro el arvivo de path");
        fclose(rfile);
        return;
    }

    int int_retorno = Structs::EscrituraInodo(particion_montada,no_inodo,superblock,cadena_leida,rfile);
    if(int_retorno == -1){
        printErr("Hubo un error en la actualizaciond el archivo");
        fclose(rfile);
        return;
    }

    Structs::Update_journaling("edit",path,cont,particion_montada,superblock,rfile);
    fclose(rfile);
    printExitoso("SE ejecuto correctamente edit");
}

void FileManager::rename(string path, string name) {
    if(usr_sesion.uid == -1){
        printErr("Se necesita una sesion iniciada");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    vector<string> div_path = split(path,'/');
    div_path.erase(div_path.begin());
    string name_buscado = div_path.at(div_path.size()-1);
    string rutatxt = "";
    div_path.pop_back();
    for (int i = 0; i < div_path.size() ; ++i) {
        rutatxt += '/' + div_path.at(i);
    }
    //regex pause("[.][t|T][x|X][T|t]" );
    // if (regex_search(entrada,recorrer) == 1)
    int no_inodo = Structs::BuscarInodo(rutatxt,particion_montada,superblock,rfile);

    if(no_inodo == -1){
        printErr("No se encontro inodo");
        fclose(rfile);
        return;
    }

    int retorno = Structs::Update_namefolder_inodo(particion_montada,no_inodo,superblock,name_buscado,name,rfile);

    if(retorno == -1){
        printErr("No se pudo actualizar el nombre del archivo");
        fclose(rfile);
        return;
    }

    Structs::Update_journaling("rename",path,name,particion_montada,superblock,rfile);
    fclose(rfile);
    printExitoso("Se ejecuto correctamente rename");
}

void FileManager::mkdir(string path, bool p) {
    if(usr_sesion.uid == -1){
        printErr("Se necesita una sesion iniciada");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);
    if(!p){
        vector<string> div_path = split(path,'/');
        div_path.erase(div_path.begin());
        string namefolder = div_path.at(div_path.size()-1);
        div_path.pop_back();
        //se supone que si p = false solo se debe cear la ultima carpeta
        string path_recortado;
        for (int i = 0; i < div_path.size(); ++i) {
            path_recortado += "/" + div_path.at(i);
        }

        int no_inodo = Structs::BuscarInodo(path_recortado,particion_montada,superblock,rfile);
        if(no_inodo == -1){
            printErr("Hubo un error en la creacion de la carpeta, path no encontrado");
            fclose(rfile);
            return;
        }

        int retorno = Structs::Crear_folder_inodo(particion_montada,no_inodo,superblock,namefolder,rfile);
        if(no_inodo == -1){
            printErr("Hubo un error en la creacion de la carpeta");
            fclose(rfile);
            return;
        }
        Structs::Update_journaling("mkdir",path,"-",particion_montada,superblock,rfile);
    }else{
        int no_inodo = Structs::Buscar_CreandoInodo(path,particion_montada,superblock,rfile);
        if(no_inodo == -1){
            printErr("Hubo un error en la creacion de la carpeta, error de path");
            fclose(rfile);
            return;
        }
        Structs::Update_journaling("mkdir -p",path,"-",particion_montada,superblock,rfile);
    }
    fclose(rfile);
    printExitoso("Se ejecuto correctamente mkdir");
}

void FileManager::copy(string path,string destino) {
    if(usr_sesion.uid == -1 ){
        printErr("Se necesita una sesion iniciada ");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    int no_inodo_inicio = Structs::BuscarInodo(path,particion_montada,superblock,rfile);
    if(no_inodo_inicio == -1){
        printErr("No se encontro carpeta para mover");
        fclose(rfile);
        return;
    }

    int no_inodo_destino = Structs::BuscarInodo(destino,particion_montada,superblock,rfile);
    if(no_inodo_destino == -1){
        printErr("No se encontro carpeta de destino");
        fclose(rfile);
        return;
    }

    vector<string> div_path = split(path,'/');
    string name_origen = div_path.at(div_path.size()-1);
    //para buscar el ultimo folder indicado


    int retorno_copiar = Structs::Copiar_elemento(rfile,particion_montada,no_inodo_inicio,superblock,no_inodo_destino,name_origen);
    if(retorno_copiar == -1){
        printErr("Algo sucedio en la copia de archivos");
        return;
    }

    Structs::Update_journaling("copy",path,destino,particion_montada,superblock,rfile);
    fclose(rfile);
    printExitoso("Se ejecuto correctamente copy");
}

void FileManager::move(string path, string destino) {
    if(usr_sesion.uid == -1 ){
        printErr("Se necesita una sesion iniciada ");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    int no_inodo_inicio = Structs::BuscarInodo(path,particion_montada,superblock,rfile);
    if(no_inodo_inicio == -1){
        printErr("No se encontro carpeta para mover");
        fclose(rfile);
        return;
    }

    int no_inodo_destino = Structs::BuscarInodo(destino,particion_montada,superblock,rfile);
    if(no_inodo_destino == -1){
        printErr("No se encontro carpeta de destino");
        fclose(rfile);
        return;
    }

    vector<string> div_path = split(path,'/');
    div_path.erase(div_path.begin());
    string name_origen = div_path.at(div_path.size()-1);
    div_path.pop_back();
    //para buscar el ultimo folder indicado
    string path_recortado_origen;
    for (int i = 0; i < div_path.size(); ++i) {
        path_recortado_origen += '/' +  div_path.at(i);
    }

    int padre_origen = Structs::BuscarInodo(path_recortado_origen,particion_montada,superblock,rfile);
    int retorno_eliminar = Structs::Eliminar_elemnto(name_origen,rfile,particion_montada,padre_origen);
    if(retorno_eliminar == -1){
        printErr("No se pudo eliminar el elemento");
        fclose(rfile);
        return;
    }

    int retorno_agregar = Structs::Agregar_elemnto(name_origen,no_inodo_inicio,rfile,particion_montada,no_inodo_destino);
    if(retorno_agregar == -1){
        printErr("No se pudo agregar el elemento");
        fclose(rfile);
        return;
    }


    Structs::Update_journaling("move",path,destino,particion_montada,superblock,rfile);
    fclose(rfile);
    printExitoso("Se ejecuto correctamente move");
}

void FileManager::find(string path, int tipo) {
    if(usr_sesion.uid == -1 ){
        printErr("Se necesita una sesion iniciada ");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    int no_inodo_inicio = Structs::BuscarInodo(path,particion_montada,superblock,rfile);
    if(no_inodo_inicio == -1){
        printErr("No se encontro carpeta para buscar");
        fclose(rfile);
        return;
    }
    Structs::Inodes inodo_ccr;
    fseek(rfile, superblock.s_inode_start + no_inodo_inicio * sizeof (Structs::Inodes), SEEK_SET);
    fread(&inodo_ccr, sizeof(Structs::Inodes), 1, rfile);

    vector<string> div_path = split(path,'/');
    string name_origen = div_path.at(div_path.size()-1);

    print(name_origen + "/");
    Structs::recorrer_find(inodo_ccr,superblock,rfile,0,tipo);

    fclose(rfile);
    printExitoso("Se ejecuto correctamente find");
}

void FileManager::chown(string path, string usr, bool r) {
    if(usr_sesion.uid == -1 ){
        printErr("Se necesita una sesion iniciada ");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    int usr_id =-1;
    string grp_name;
    int grp_id;

    string rutatxt = "/user.txt";
    int no_inodo = Structs::BuscarInodo(rutatxt,particion_montada,superblock,rfile);

    if(no_inodo == -1){
        printErr("No se encontro archivo");
        fclose(rfile);
        return;
    }

    string cadena_leida = Structs::LecturaInodo(no_inodo,superblock,rfile);
    printExitoso(cadena_leida);
    if(cadena_leida.empty()){
        printErr("No se pudo leer archivo");
        fclose(rfile);
        return;
    }
    vector<string> div_lineas = split(cadena_leida,'\n');
    for (string line:div_lineas) {
        if (line.empty())
            break;
        if (line[2] == 'U' || line[2] == 'u') {
            vector<string> in = split(line, ',');
            if (in[3] == usr) {
                usr_id = stoi(in[0]);
                grp_name = in[2];
                break;
            }
        }
    }
    if(usr_id == -1){
        printErr("No existe el usuario que sera el nuevo propieetario");
        fclose(rfile);
        return;
    }

    for (string line:div_lineas) {
        if (line.empty())
            break;
        if (line[2] == 'g' || line[2] == 'G') {
            vector<string> in = split(line, ',');
            if (in[2] == grp_name) {
                grp_id = stoi(in[0]);
                break;
            }
        }
    }

    no_inodo = Structs::BuscarInodo(path,particion_montada,superblock,rfile);

    if(no_inodo == -1){
        printErr("No se encontro archivo");
        fclose(rfile);
        return;
    }

    if(!r){
        int retorno = Structs::Update_own_inodo(no_inodo,superblock,usr_id,grp_id,rfile);

        if(retorno == -1){
            printErr("Err al cambiar propietario");
            fclose(rfile);
            return;
        }
        Structs::Update_journaling("chown",path,usr,particion_montada,superblock,rfile);
    }else{
        int retorno = Structs::cambiar_propietario(superblock,rfile,no_inodo,usr_id,grp_id);

        if(retorno == -1){
            printErr("Err al cambiar propietario");
            fclose(rfile);
            return;
        }
        Structs::Update_journaling("chown -r",path,usr,particion_montada,superblock,rfile);
    }


    fclose(rfile);
    printExitoso("Se ejecuto correctamente chown");
}

void FileManager::chmod(string path, int ugo, bool r) {
    if(usr_sesion.uid == -1 or usr_sesion.usr != "root"){
        printErr("Se necesita una sesion iniciada y se necesita ser el usuario root");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(usr_sesion.pid);
    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    int no_inodo = Structs::BuscarInodo(path,particion_montada,superblock,rfile);

    if(no_inodo == -1){
        printErr("No se encontro el inodo");
        fclose(rfile);
        return;
    }

    if(!r){
        int retorno = Structs::Update_perm_inodo(no_inodo,superblock,ugo,rfile);

        if(retorno == -1){
            fclose(rfile);
            printErr("Err al cambiar permisos");
            return;
        }
        Structs::Update_journaling("chmod",path, to_string(ugo),particion_montada,superblock,rfile);
    }else{
        int retorno = Structs::cambiar_permisos(superblock,rfile,no_inodo,ugo);
        if(retorno == -1){
            fclose(rfile);
            printErr("Err al cambiar permisos");
            return;
        }
        Structs::Update_journaling("chmod -r",path, to_string(ugo),particion_montada,superblock,rfile);
    }

    fclose(rfile);
    printExitoso("Se ejecuto correctamente chmod");
}