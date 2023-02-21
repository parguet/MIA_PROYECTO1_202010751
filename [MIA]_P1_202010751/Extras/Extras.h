//
// Created by parguet on 21/02/23.
//

#ifndef _MIA__P1_202010751_EXTRAS_H
#define _MIA__P1_202010751_EXTRAS_H

#include "string"
using namespace std;

void print(string mensaje);
void printErr(string error);
void printExitoso(string pregunta);
bool Warning(string pregunta);
string format_day_prop(int field);
string obtenerFecha();
string convertToString(char* a, int size);
void exportDOT(string DOT,string dot_name,string path);


#endif //_MIA__P1_202010751_EXTRAS_H
