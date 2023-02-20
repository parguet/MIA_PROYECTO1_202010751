#include <iostream>
#include <regex>
#include "Analizador.h"
using namespace std;

int main() {
    //home/parguet/Escritorio
    cout<<" ---- Bienvenido al Sistema de Archivos de  - 202010751 ---- "<<endl;
    regex comentario("[#][^\n]*");
    regex flecha("->");
    regex exit("[e|E][x|X][i|I][t|T]" );
    while (true){
        string comando;
        cout << "Ingresa un comando: "<< endl;
        getline(cin,comando);
        comando= regex_replace(comando,comentario,"");
        comando= regex_replace(comando,flecha,"=");
        if(comando.empty())
            continue;
        if (regex_search(comando,exit) == 1){
            break;
        }
        Analizador analizador;
        analizador.analizarTipo(comando);
    }
    cout<<" ---- Termino programa | Gracias por usar ---- "<<endl;
}
