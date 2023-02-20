#include <iostream>
#include <regex>
#include "Analizador.h"
using namespace std;

int main() {
    //home/parguet/Escritorio
    cout<<" ---- Bienvenido al Sistema de Archivos de  - 202010751 ---- "<<endl;
    string comando;
    cout << "Ingresa un comando: "<< endl;
    getline(cin,comando);
    Analizador analizador;
    analizador.analizarTipo(comando);

}
