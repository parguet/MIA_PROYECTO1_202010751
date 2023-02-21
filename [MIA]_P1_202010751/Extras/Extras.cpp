//
// Created by parguet on 21/02/23.
//

#include "Extras.h"
#include <iostream>
#include <regex>
using namespace std;

void print(string mensaje){
    cout <<  mensaje << endl;
}

void printErr(string error){
    cout << "\033[1;31m" << "Error: " <<  error << "\033[0m"<< endl;
}

void printExitoso(string mensaje){
    string confirm = "no";
    cout << "\033[1;32m" << mensaje << "\033[0m"<< endl;
}

bool Warning(string pregunta){
    string confirm = "no";
    cout << "\033[1;33m" << pregunta << "\033[0m"<< endl;
    cin >> confirm;
    return confirm == "s" || confirm == "S";
}

