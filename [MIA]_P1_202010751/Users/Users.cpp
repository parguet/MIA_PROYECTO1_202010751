//
// Created by parguet on 16/03/23.
//

#include "Users.h"
#include "../Extras/Extras.h"
#include "../Global/Global.h"
#include "../Structs//Structs.h"
#include <vector>
#include <cstring>

Users::Users(){
}

void Users::login(string usr, string pass, string id) {
    if(usr_sesion.uid != -1){
        printErr("Ya hay un usuario en sesion");
        return;
    }

    int i_particion_montada = Buscar_Pmontada_id(id);
    if(i_particion_montada == -1){
        printErr("Err no se encontr particion a iniciar sesion");
        return;
    }

    MountedPartition particion_montada = mounted_partitions.at(i_particion_montada);
    Structs::Superblock superblock;
    FILE *rfile = fopen(particion_montada.path.c_str(), "rb+");
    fseek(rfile, particion_montada.start, SEEK_SET);
    fread(&superblock, sizeof(Structs::Superblock), 1, rfile);

    if(!(superblock.s_inode_size != 0 or superblock.s_block_size != 0)){
        printErr("La particion no tiene formato EXT2 o EXT3");
        fclose(rfile);
        return;
    }

    string rutatxt = "/user.txt";
    int no_inodo = Structs::BuscarInodo(rutatxt,particion_montada,superblock,rfile);

    if(no_inodo == -1){
        printErr("No se encontro el inodo");
        fclose(rfile);
        return;
    }

    string cadena_leida = Structs::LecturaInodo(no_inodo,superblock,rfile);

    if(cadena_leida.empty()){
        printErr("No se pudo leer user.txt archivo");
        fclose(rfile);
        return;
    }

    vector<string> div_lineas = split(cadena_leida,'\n');

    for (string line:div_lineas) {
        if (line.empty())
            break;
        if (line[2] == 'U' || line[2] == 'u') {
            vector<string> in = split(line, ',');
            if (in[3] == usr && in[4] == pass) {
                usr_sesion.uid = stoi(in[0]);
                usr_sesion.usr = usr;
                usr_sesion.pass = pass;
                usr_sesion.pid = id;
                usr_sesion.grp = in[2];
                break;
            }
        }
    }

    if(usr_sesion.uid == -1){
        printErr("Err no se pudo iniciar sesion");
        fclose(rfile);
        return;
    }

    for (string line:div_lineas) {
        if (line.empty())
            break;
        if (line[2] == 'g' || line[2] == 'G') {
            vector<string> in = split(line, ',');
            if(in[2] == usr_sesion.grp){
                usr_sesion.gid = stoi(in[0]);
                break;
            }
        }
    }

    fclose(rfile);
    printExitoso("Se ejecuto correctamente login");
}

void Users::logout() {
    if (usr_sesion.uid != -1){
        usr_sesion.uid = -1;
        printExitoso("Se ejecuto correctamente logout");
    }else{
        printErr("Necesita haber una sesion iniciada");
    }
}

void Users::mkgrp(string name) {
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


    string rutatxt = "/user.txt";
    int no_inodo = Structs::BuscarInodo(rutatxt,particion_montada,superblock,rfile);

    if(no_inodo == -1){
        printErr("No se encontro inodo");
        fclose(rfile);
        return;
    }

    string cadena_leida = Structs::LecturaInodo(no_inodo,superblock,rfile);

    if(cadena_leida.empty()){
        printErr("No se pudo leer archivo user.txt");
        fclose(rfile);
        return;
    }

    printExitoso(cadena_leida);
    vector<string> div_lineas = split(cadena_leida,'\n');
    int contador = 1;
    for (string line:div_lineas) {
        if (line.empty())
            break;

        if (line[2] == 'g' || line[2] == 'G') {
            vector<string> in = split(line, ',');
            if(in[2] == name){
                contador = -1;
                break;
            }
            contador++;
        }
    }
    if(contador == -1){
        printErr("No se pudo crear grupo, nombre repetido");
        fclose(rfile);
        return;
    }
    string grupo_new = to_string(contador) + ",G," + name + "\n";
    cadena_leida += grupo_new;

    int int_retorno = Structs::EscrituraInodo(particion_montada,no_inodo,superblock,cadena_leida,rfile);
    if(int_retorno == -1){
        printErr("Hubo un error en la actualizaciond el archivo");
        fclose(rfile);
        return;
    }

    fclose(rfile);
    printExitoso("SE ejecuto correctamente mkgrp");
}
