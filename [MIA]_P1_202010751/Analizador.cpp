//
// Created by parguet on 16/02/23.
//

#include "Analizador.h"

#include <iostream>
#include <regex>
#include <queue>
#include "Disk/Disk.h"
using namespace std;
void addToken(string cadena,string tipo);
void analizarTipo(string comando);
void readTokens(string comando);
void fn_mkdisk();


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
    if(regex_search(comando,mkdisk)==1){
        comando = regex_replace(comando, mkdisk, "");
        cout<<" ---- Se dectecto mkdisk ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);
        cout <<colaTokens.size()<<endl;
        /*while(!colaTokens.empty()){
            token r;
            r = colaTokens.front();
            cout << r.tipo << " "<< r.value <<endl;
            colaTokens.pop();
        }*/
        fn_mkdisk();
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