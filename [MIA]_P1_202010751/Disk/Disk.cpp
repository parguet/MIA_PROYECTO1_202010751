//
// Created by parguet on 16/02/23.
//

#include <string.h>

#include <iostream>
#include <string>
#include "Disk.h"
#include "../Structs/Structs.h"
using namespace std;
string obtenerFecha();
string format_day_prop(int field);

Disk::Disk(){
}

void Disk::mkdisk(int size, char fit, char unit, string path){
    string direccion_sinarchivo = '"' + path.substr(0, path.find_last_of("/\\")) + '"';
    string comando1 = "mkdir -p " + direccion_sinarchivo + "";
    system(comando1.c_str());

    FILE *disk_file = fopen(path.c_str(), "wb");
    if (disk_file != NULL) {
        string fecha = obtenerFecha();

        Structs::MBR mbr_data;
        strcpy( mbr_data.FechaCreacion, fecha.c_str());
        mbr_data.mbr_tamano = unit == 'k' ? size * 1024 : size * 1024 * 1024;
        mbr_data.mbr_disk_signature = rand() % 9999 + 100;
        mbr_data.disk_fit = fit;

        char buffer[1024];
        for (int i = 0; i < 1024; i++)
            buffer[i] = '\0';

        for (int i = 0;i < (size * (unit == 'k' ? 1 : 1024));i++)
            fwrite(&buffer, 1024, 1, disk_file);

        fseek(disk_file, 0, SEEK_SET);
        fwrite(&mbr_data, sizeof(Structs::MBR), 1, disk_file);
        fclose(disk_file);
        cout << "\033[1;32m" << "Se ejecuto correctamente mkdisk" << "\033[0m"<< endl;
    }else{
        cout << "\033[1;31m" << "Error: " <<  "No se puedo crear el disco" << "\033[0m"<< endl;
    }

}

string obtenerFecha() {
    time_t time_now = time(0);
    tm *now = localtime(&time_now);
    string current_date =
            format_day_prop(now->tm_mday) + "/" + format_day_prop(now->tm_mon + 1) +
            "/" + to_string(now->tm_year + 1900) + " " +
            format_day_prop(now->tm_hour) + ":" + format_day_prop(now->tm_min);
    return current_date;
}
string format_day_prop(int field) {
    return (field < 10 ? "0" : "") + to_string(field);
}