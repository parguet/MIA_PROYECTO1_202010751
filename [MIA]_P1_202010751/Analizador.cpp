//
// Created by parguet on 16/02/23.
//

#include "Analizador.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <queue>
#include "Disk/Disk.h"
#include "Extras/Extras.h"
#include "FileSystem/FileSystem.h"
#include "Users/Users.h"
#include "FileManager/FileManager.h"

using namespace std;
void addToken(string cadena,string tipo);
void analizarTipo(string comando);
void readTokens(string comando);
void fn_mkdisk();
void fn_execute();
void lectura(string path);
void fn_fdisk();
void fn_rmdisk();
void fn_mount();
void  fn_unmount();
void fn_mkfs();
void fn_login();
void fn_logout();
void fn_mkgrp();
void fn_rmgrp();
void fn_mkusr();
void fn_rmusr();
void fn_mkfile();
void fn_cat();
void fn_remove();
void fn_edit();
void fn_rename();
void fn_mkdir();
void fn_copy();



Analizador::Analizador(){
}

struct token {
    string value;
    string tipo;
};
struct comando {
    string comando;
    string valor;
};
comando obtenerComando();

queue<token> colaTokens;
void Analizador::analizarTipo(string comando){
    regex mkdisk("[m|M][k|K][d|D][i|I][s|S][k|K]");
    regex rmdisk("[r|R][m|M][d|D][i|I][s|S][k|K]" );
    regex fdisk("[f|F][d|D][i|I][s|S][k|K]" );
    regex mount("[m|M][o|O][U|u][n|N][t|T]" );
    regex unmount("[u|U][n|N][m|M][o|O][U|u][n|N][t|T]" );
    regex mkfs("[m|M][k|K][f|F][s|S]" );
    regex login("[l|L][o|O][g|G][I|i][N|n]" );
    regex logout("[l|L][o|O][g|G][o|O][u|U][t|T]" );
    regex mkgrp("[m|M][k|K][g|G][r|R][p|P]" );
    regex rmgrp("[r|R][m|M][g|G][r|R][p|P]" );
    regex mkusr("[m|M][k|K][u|U][s|S][r|R]" );
    regex rmusr("[r|R][m|M][u|U][s|S][r|R]" );
    regex mkfile("[m|M][k|K][F|f][I|i][L|l][E|e]" );
    regex cat("[c|C][a|A][t|T]" );
    regex remove("[r|R][e|E][m|M][o|O][v|V][e|E]" );
    regex edit("[e|E][d|D][i|I][t|T]" );
    regex rename("[r|R][e|E][n|N][a|A][m|M][e|E]" );
    regex mkdir("[m|M][k|K][d|D][I|i][r|R]" );
    regex copy("[c|C][o|O][p|P][y|Y]" );



    regex execute("[e|E][x|X][e|E][c|C][u|U][t|T][e|E]" );

    regex rep("[r|R][e|E][p|P]" );


    if(regex_search(comando,mkdisk)==1){
        comando = regex_replace(comando, mkdisk, "");
        cout<<" ---- Se dectecto mkdisk ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        /*while(!colaTokens.empty()){
            token r;
            r = colaTokens.front();
            cout << r.tipo << " "<< r.value <<endl;
            colaTokens.pop();
        }*/
        fn_mkdisk();
        cout<<" ---- Termino mkdisk ---- "<<endl;
    }
    else if(regex_search(comando,rmdisk) == 1){
        comando = regex_replace(comando,rmdisk, "");
        cout<<" ---- Se dectecto rmdisk ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_rmdisk();
        cout<<" ---- Termino rmdisk ---- "<<endl;

    }
    else if(regex_search(comando,unmount) == 1){
        comando = regex_replace(comando,unmount, "");
        cout<<" ---- Se dectecto unmount ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_unmount();
        cout<<" ---- unTermino unmount ---- "<<endl;

    }
    else if(regex_search(comando,mount) == 1){
        comando = regex_replace(comando,mount, "");
        cout<<" ---- Se dectecto mount --analizarTipo-- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_mount();
        cout<<" ---- Termino mount ---- "<<endl;

    }
    else if(regex_search(comando,mkfs) == 1){
        comando = regex_replace(comando,mkfs, "");
        cout<<" ---- Se dectecto mkfs ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_mkfs();
        cout<<" ---- Termino mkfs ---- "<<endl;

    }
    else if(regex_search(comando,login) == 1){
        comando = regex_replace(comando,login, "");
        cout<<" ---- Se dectecto login ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_login();
        cout<<" ---- Termino login ---- "<<endl;

    }
    else if(regex_search(comando,logout) == 1){
        comando = regex_replace(comando,logout, "");
        cout<<" ---- Se dectecto logout ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_logout();
        cout<<" ---- Termino logout ---- "<<endl;

    }
    else if(regex_search(comando,mkgrp) == 1){
        comando = regex_replace(comando,mkgrp, "");
        cout<<" ---- Se dectecto mkgrp ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_mkgrp();
        cout<<" ---- Termino mkgrp ---- "<<endl;

    }
    else if(regex_search(comando,rmgrp) == 1){
        comando = regex_replace(comando,rmgrp, "");
        cout<<" ---- Se dectecto rmgrp ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_rmgrp();
        cout<<" ---- Termino rmgrp ---- "<<endl;

    }
    else if(regex_search(comando,mkusr) == 1){
        comando = regex_replace(comando,mkusr, "");
        cout<<" ---- Se dectecto mkusr ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_mkusr();
        cout<<" ---- Termino mkusr ---- "<<endl;

    }
    else if(regex_search(comando,rmusr) == 1){
        comando = regex_replace(comando,rmusr, "");
        cout<<" ---- Se dectecto rmusr ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_rmusr();
        cout<<" ---- Termino rmusr ---- "<<endl;

    }
    else if(regex_search(comando,mkfile) == 1){
        comando = regex_replace(comando,mkfile, "");
        cout<<" ---- Se dectecto mkfile ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_mkfile();
        cout<<" ---- Termino mkfile ---- "<<endl;

    }
    else if(regex_search(comando,cat) == 1){
        comando = regex_replace(comando,cat, "");
        cout<<" ---- Se dectecto cat ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_cat();
        cout<<" ---- Termino cat ---- "<<endl;

    }
    else if(regex_search(comando,remove) == 1){
        comando = regex_replace(comando,remove, "");
        cout<<" ---- Se dectecto remove ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_remove();
        cout<<" ---- Termino remove ---- "<<endl;
    }
    else if(regex_search(comando,edit) == 1){
        comando = regex_replace(comando,edit, "");
        cout<<" ---- Se dectecto edit ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_edit();
        cout<<" ---- Termino edit ---- "<<endl;
    }
    else if(regex_search(comando,rename) == 1){
        comando = regex_replace(comando,rename, "");
        cout<<" ---- Se dectecto rename ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_rename();
        cout<<" ---- Termino rename ---- "<<endl;
    }
    else if(regex_search(comando,mkdir) == 1){
        comando = regex_replace(comando,mkdir, "");
        cout<<" ---- Se dectecto mkdir ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_mkdir();
        cout<<" ---- Termino mkdir ---- "<<endl;
    }
    else if(regex_search(comando,copy) == 1){
        comando = regex_replace(comando,copy, "");
        cout<<" ---- Se dectecto copy ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        fn_copy();
        cout<<" ---- Termino copy ---- "<<endl;
    }










    else if(regex_search(comando,execute)==1){
        comando = regex_replace(comando,execute,"");
        cout<<" ---- Se dectecto execute ---- "<<endl;
        cout << comando << endl;
        readTokens(comando);
        fn_execute();
        cout<<" ---- Termino execute ---- "<<endl;

    }
    else if(regex_search(comando,fdisk)==1){
        comando = regex_replace(comando,fdisk,"");
        cout<<" ---- Se dectecto fdisk ---- "<<endl;
        cout << comando << endl;
        readTokens(comando);
        fn_fdisk();
        cout<<" ---- Termino fdisk ---- "<<endl;

    }
    else if(regex_search(comando,rep)==1){
        comando = regex_replace(comando,rep,"");
        cout<<" ---- Se dectecto rep ---- "<<endl;
        cout << comando << endl;
        //cout<<" ---- rep mbr ---- "<<endl;
        //Disk *disk_cmd = new Disk();
        //disk_cmd->rep_mbr("id","/home/parguet/Escritorio/Disco1.dsk");
        //cout<<" ---- end rep mbr ---- "<<endl;
        cout<<" ---- rep disk ---- "<<endl;
        Disk *disk_cmd = new Disk();
        disk_cmd->rep_disk("id","/home/parguet/Escritorio/Disco1.dsk");
        cout<<" ---- end rep disk ---- "<<endl;
        cout<<" ---- Termino rep ---- "<<endl;

    }

}

void fn_mkdisk(){
    string size = "";
    char fit = 'f';
    char unit = 'm';
    string path = "";

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        //print(comando_entrada.comando);
        //print(comando_entrada.valor);
        if (comando_entrada.comando == "size"){
            size = comando_entrada.valor;
        }else if (comando_entrada.comando == "unit"){
            unit = comando_entrada.valor.at(0);
        }else if(comando_entrada.comando == "fit"){
            fit = comando_entrada.valor.at(0);
        }else if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else{
            cout << "\033[1;31m" << "Error: " <<  "Parametro erroneo" << "\033[0m"<< endl;
            return;
        }
        comando_entrada = obtenerComando();
    }
    if(!(fit == 'b' or fit == 'w' or fit == 'f')){
        cout << "\033[1;31m" << "Error: " <<  "Parametro de ajuste erroneo" << "\033[0m"<< endl;
        return;
    }

    if(!( unit == 'k' or unit == 'm')){
        cout << "\033[1;31m" << "Error: " <<  "Parametro de unidad erroneo" << "\033[0m"<< endl;
        return;
    }

    if (size.empty()){
        cout << "\033[1;31m" << "Error: " <<  "Falto el parametro obligatorio s" << "\033[0m"<< endl;
        return;
    }
    int tamanio = 0;
    try {
        tamanio = stoi(size);
        if(tamanio <= 0){
            cout << "\033[1;31m" << "Error: " <<  "EL valor de s debe ser mayor a 0" << "\033[0m"<< endl;
            return;
        }
    }catch(...){
        cout << "\033[1;31m" << "Error: " <<  "EL valor de s debe ser un digito" << "\033[0m"<< endl;
        return;
    }

    if (path.empty()){
        cout << "\033[1;31m" << "Error: " <<  "Falto el parametro obligatorio path" << "\033[0m"<< endl;
        return;
    }

    regex dsk("[.][d|D][s|S][k|K]" );
    if(regex_search(path,dsk) == 0){
        cout << "\033[1;31m" << "Error: " <<  "Se esperaba extension DSK" << "\033[0m"<< endl;
        return;
    }

    Disk *disck_cmd = new Disk();
    disck_cmd->mkdisk(tamanio,fit,unit,path);
}

void fn_rmdisk(){
    string path = "";
    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }

    regex dsk("[.][d|D][s|S][k|K]" );
    if(regex_search(path,dsk) == 0){
        printErr("Se esperaba extension DSK");
        return;
    }
    Disk *disck_cmd = new Disk();
    disck_cmd->rmdisk(path);
}

void fn_mount(){
    string name;
    string path;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else if(comando_entrada.comando == "name"){
            name = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }
    regex dsk("[.][d|D][s|S][k|K]" );
    if(regex_search(path,dsk) == 0){
        printErr("Se esperaba extension DSK");
        return;
    }

    if (name.empty()){
        printErr("Falto el parametro obligatorio name");
        return;
    }

    Disk *disck_cmd = new Disk();
    disck_cmd->mount(path,name);
}

void fn_unmount(){
    string id;
    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "id"){
            id = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (id.empty()){
        printErr("Falto el parametro obligatorio id");
        return;
    }

    Disk *disck_cmd = new Disk();
    disck_cmd->unmount(id);
}

void fn_mkfs(){
    string id;
    string type = "full";
    string fs = "2fs";

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "type"){
            type = comando_entrada.valor;
        }else if(comando_entrada.comando == "id"){
            id = comando_entrada.valor;
        }else if(comando_entrada.comando == "fs"){
            fs = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (id.empty()){
        printErr("Falto el parametro obligatorio id");
        return;
    }

    if(type != "full"){
        printErr("El unico parametro que puede tener type es full");
        return;
    }

    if(!(fs == "2fs" or fs == "3fs")){
        printErr("Parametro erroneo de fs");
        return;
    }

    Filesystem *file_cmd = new Filesystem();
    file_cmd->mkfs(id,type,fs);
}

void fn_login(){
    string usr;
    string pass;
    string id;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "user"){
            usr = comando_entrada.valor;
        }else if(comando_entrada.comando == "pass"){
            pass = comando_entrada.valor;
        }else if(comando_entrada.comando == "id"){
            id = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (usr.empty()){
        printErr("Falto el parametro obligatorio usr");
        return;
    }

    if (pass.empty()){
        printErr("Falto el parametro obligatorio pass");
        return;
    }

    if (id.empty()){
        printErr("Falto el parametro obligatorio id");
        return;
    }

    Users *user_cmd = new Users();
    user_cmd->login(usr,pass,id);

}

void fn_logout(){
    Users *user_cmd = new Users();
    user_cmd->logout();
}

void fn_mkgrp(){
    string name;
    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "name"){
            name = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (name.empty()){
        printErr("Falto el parametro obligatorio name");
        return;
    }

    Users *usuarios_cmd = new Users();
    usuarios_cmd->mkgrp(name);
}

void fn_rmgrp(){
    string name;
    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "name"){
            name = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (name.empty()){
        printErr("Falto el parametro obligatorio name");
        return;
    }

    Users *usuarios_cmd = new Users();
    usuarios_cmd->rmgrp(name);
}

void fn_mkusr(){
    string usr;
    string pass;
    string grp;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "user"){
            usr = comando_entrada.valor;
        }else if(comando_entrada.comando == "pass"){
            pass = comando_entrada.valor;
        }else if(comando_entrada.comando == "grp"){
            grp = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (usr.empty()){
        printErr("Falto el parametro obligatorio usr");
        return;
    }

    if (pass.empty()){
        printErr("Falto el parametro obligatorio pass");
        return;
    }

    if (grp.empty()){
        printErr("Falto el parametro obligatorio grp");
        return;
    }

    if(usr.size() > 10 or pass.size() > 10 or grp.size() > 10){
        printErr("Err el maximo para los parametros de mkusr es de 10 caracteres");
        return;
    }

    Users *user_cmd = new Users();
    user_cmd->mkusr(usr,pass,grp);
}

void fn_rmusr(){
    string usr;
    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "user"){
            usr = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (usr.empty()){
        printErr("Falto el parametro obligatorio usr");
        return;
    }

    Users *user_cmd = new Users();
    user_cmd->rmusr(usr);
}

void fn_mkfile(){
    string path;
    string size_str = "0";
    bool r = false;
    string cont;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else if(comando_entrada.comando == "size"){
            size_str = comando_entrada.valor;
        }else if(comando_entrada.comando == "r"){
            r = true;
        }else if(comando_entrada.comando == "cont"){
            cont = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }

    int size;
    try {
        size = stoi(size_str);
        if(size < 0){
            printErr("Err size debe ser mayor a 0");
            return;
        }
    }catch(...){
        printErr("EL valor de ugo debe ser un digito");
        return;
    }

    FileManager *fm_cmd = new FileManager();
    fm_cmd->mkfile(path,r,size,cont);
}

void fn_cat(){
    vector<string> files;
    regex cmd("[f|F][I|i][l|L][e|E]([0-9])*" );
    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(regex_search(comando_entrada.comando,cmd) == 1){
            files.push_back(comando_entrada.valor);
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    FileManager *fm_cmd = new FileManager();
    fm_cmd->cat(files);
}

void fn_remove(){
    string path;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }

    FileManager *fm_cmd = new FileManager();
    fm_cmd->remove(path);

}

void fn_edit(){
    string path;
    string cont;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else if(comando_entrada.comando == "cont"){
            cont = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }

    if (cont.empty()){
        printErr("Falto el parametro obligatorio cont");
        return;
    }

    FileManager *fm_cmd = new FileManager();
    fm_cmd->edit(path,cont);
}

void fn_rename(){
    string path;
    string name;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else if(comando_entrada.comando == "name"){
            name = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }

    if (name.empty()){
        printErr("Falto el parametro obligatorio name");
        return;
    }


    FileManager *fm_cmd = new FileManager();
    fm_cmd->rename(path,name);
}

void fn_mkdir(){
    string path;
    bool p = false;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else if(comando_entrada.comando == "r"){
            p = true;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }

    FileManager *fm_cmd = new FileManager();
    fm_cmd->mkdir(path,p);
}

void fn_copy(){
    string path;
    string destino;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else if(comando_entrada.comando == "destino"){
            destino = comando_entrada.valor;
        }else{
            printErr("Parametro erroneo");
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        printErr("Falto el parametro obligatorio path");
        return;
    }

    if (destino.empty()){
        printErr("Falto el parametro obligatorio destino");
        return;
    }

    FileManager *fm_cmd = new FileManager();
    fm_cmd->copy(path,destino);
}









void fn_execute(){
    string path;

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else{
            cout << "\033[1;31m" << "Error: " <<  "Parametro erroneo" << "\033[0m"<< endl;
            return;
        }
        comando_entrada = obtenerComando();
    }

    if (path.empty()){
        cout << "\033[1;31m" << "Error: " <<  "Falto el parametro obligatorio path"<< "\033[0m"<< endl;
        return;
    }

    regex eea("[.][e|E][e|E][a|A]" );
    if(regex_search(path,eea) == 0){
        cout << "\033[1;31m" << "Error: " <<  "Se esperaba extension eea"<< "\033[0m"<< endl;
        return;
    }

    lectura(path);
}

void fn_fdisk(){
    string size ="-1";
    char u ='k';
    string path;
    char t = 'p';
    char f = 'w';
    string del;
    string name;
    string addvalue = "0";

    comando comando_entrada = obtenerComando();
    while(!comando_entrada.comando.empty()){
        if (comando_entrada.comando == "size"){
            size = comando_entrada.valor;
        }else if (comando_entrada.comando == "unit"){
            u = comando_entrada.valor.at(0);
        }else if(comando_entrada.comando == "path"){
            path = comando_entrada.valor;
        }else if (comando_entrada.comando == "type"){
            t = comando_entrada.valor.at(0);
        }else if (comando_entrada.comando == "fit"){
            f = comando_entrada.valor.at(0);
        }else if(comando_entrada.comando == "delete"){
            del = comando_entrada.valor;
        }else if(comando_entrada.comando == "name"){
            name = comando_entrada.valor;
        }else if(comando_entrada.comando == "add"){
            addvalue = comando_entrada.valor;
        }else{
            cout << "\033[1;31m" << "Error: " <<  "Parametro erroneo" << "\033[0m"<< endl;
            return;
        }
        comando_entrada = obtenerComando();
    }

    if(!(f == 'b' or f == 'w' or f == 'f')){
        cout << "\033[1;31m" << "Error: " <<  "Parametro de ajuste erroneo" << "\033[0m"<< endl;
        return;
    }

    if(!(u == 'b' or u == 'k' or u == 'm')){
        cout << "\033[1;31m" << "Error: " <<  "Parametro de unidad erroneo" << "\033[0m"<< endl;
        return;
    }

    if (path.empty()){
        cout << "\033[1;31m" << "Error: " <<  "Falto el parametro obligatorio path" << "\033[0m"<< endl;
        return;
    }
    regex dsk("[.][d|D][s|S][k|K]" );
    if(regex_search(path,dsk) == 0){
        cout << "\033[1;31m" << "Error: " <<  "Se esperaba extension DSK" << "\033[0m"<< endl;
        return;
    }

    if (name.empty()){
        cout << "\033[1;31m" << "Error: " <<  "Falto el parametro obligatorio name" << "\033[0m"<< endl;
        return;
    }

    int s = 0;
    try {
        s = stoi(size);
    }catch(...){
        cout << "\033[1;31m" << "Error: " <<  "EL valor de s debe ser un digito" << "\033[0m"<< endl;
        return;
    }

    int add = 0;
    try {
        add = stoi(addvalue);
    }catch(...){
        cout << "\033[1;31m" << "Error: " <<  "EL valor de s debe ser un digito" << "\033[0m"<< endl;
        return;
    }

    Disk *disck_cmd = new Disk();
    disck_cmd->fdisk(s,u,path,t,f,del,name,add);
}

void lectura(string path){
    cout<<" ---- Nueva lectura ---- "<<endl;
    ifstream archivo;
    string entrada;
    archivo.open(path,ios::in);
    //archivo.open("../Analizador/Test/Entrada.txt",ios::in);//abrir archivo modo lectura

    if(archivo.fail()){
        cout << "\033[1;31m" << "Error: " <<  "No se pudo abrir el archivo"<< "\033[0m"<< endl;
        return;
    }

    regex comentario("[#][^\n]*");
    regex flecha("->");

    while(!archivo.eof()){
        getline(archivo,entrada);
        entrada= regex_replace(entrada,comentario,"");
        entrada= regex_replace(entrada,flecha,"=");
        if(entrada.empty())
            continue;
        Analizador analizador;
        analizador.analizarTipo(entrada);
    }
    archivo.close();
}


comando obtenerComando(){
    comando comando_salida;

    if (!colaTokens.empty()) {
        if (colaTokens.front().value == ">"){
            colaTokens.pop();
            comando_salida.comando = colaTokens.front().value;
            colaTokens.pop();
            if (colaTokens.empty()) {
                return comando_salida;
            }
            if (colaTokens.front().value == "="){
                colaTokens.pop();
                comando_salida.valor = colaTokens.front().value;
                colaTokens.pop();
            }
        }
    }
    return comando_salida;
}



void readTokens(string comando){
    string buffer = "";
    comando += " ";
    queue<token> instance;
    colaTokens = instance;
    int estate = 0;
    string texto = "";
    for(int i=0; i<comando.length(); i++){
        texto+= comando[i];
        char c = comando[i];
        if (estate == 0){
            if (isalpha(c) or c == '*' or c == '?'){
                estate = 1;
                buffer+= tolower(c);
            }else if (isdigit(c)){
                estate = 2;
                buffer+= c;
            }else if (c == '"'){

                estate = 4;
                //buffer+= c;

            }else if (c == '/' or  c == '.'){
                estate = 8;
                buffer+= c;
            }else if (isspace(c)){
                estate = 0;
                buffer = "";
            }else if (c == '>') {
                estate = 9;
                buffer+= c;
            }else if (c == '=') {
                buffer+= c;
                addToken(buffer, "IGUAL");
                buffer = "";
            }else{
                cout<<"Erro lexico"<<endl;
            }

        }else if (estate == 1){
            if (isalpha(c) || isdigit(c) || c == '_' || c == '.' || c == '*' ){
                buffer+= tolower(c);
            }else{
                addToken(buffer, "ID");
                buffer = "";
                estate = 0;
                i--;
            }
        }
        else if (estate == 2){
            if (isdigit(c)  ){
                buffer+=c;
            }else if(c == '.'){
                estate = 6;
                buffer+=c;
            }else{
                if(isalpha(c)){
                    estate = 1;
                    buffer+=c;
                }else{
                    addToken(buffer, "NUMERO");
                    buffer = "";
                    estate = 0;
                    i--;
                }

            }
        }
        else if (estate == 4){
            if(c != '"'){
                buffer+=c;
            }else{
                //buffer+= c;
                addToken(buffer, "CADENA");
                buffer = "";
                estate = 0;
            }
        }
        else if (estate == 6){
            if (isdigit(c)){
                buffer+=c;
            }else{
                addToken(buffer, "NUMERO");
                buffer = "";
                estate = 0;
                i--;
            }
        }
        else if(estate == 8){
            if (!isspace(c)){
                buffer+=c;
            }else{
                addToken(buffer, "RUTA");
                estate = 0;
                buffer = "";
            }
        }else if(estate == 9){
            if (isdigit(c)){
                estate = 2;
                buffer+=c;
            }else if(isalpha(c)){
                addToken(buffer, "GION");
                estate =0;
                i--;
                buffer = "";
            }
        }
    }
}

void addToken(string cadena,string tipo){
    token tk ;
    tk.value = cadena;
    tk.tipo = tipo;
    colaTokens.push(tk);
}