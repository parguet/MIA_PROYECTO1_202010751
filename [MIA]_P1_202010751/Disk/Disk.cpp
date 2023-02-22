//
// Created by parguet on 16/02/23.
//

#include <string.h>

#include <iostream>
#include <string>
#include <regex>
#include "Disk.h"
#include "deque"
#include "../Structs/Structs.h"
#include "../Global/Global.h"
#include "../Extras/Extras.h"

using namespace std;
string obtenerFecha();
string format_day_prop(int field);
int Buscar_Pextendida(Structs::MBR mbr_data);
int Buscar_Pnombre(string name,Structs::MBR mbr_data);
int Obtener_sizeDisco(Structs::MBR mbr_data);
int ObtenerDesplazamiento_Pnombre(string name,Structs::MBR mbr_data);
string convertToString(char* a, int size);
string agregar_dot_mbr(string parametro,string valor,string color);
void exportDOT(string DOT,string dot_name,string path);



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

void Disk::rmdisk(string path){
    FILE *disk_file = fopen(path.c_str(), "rb");
    if (disk_file != NULL) {
        if (Warning("DISK_DELETE ¿Estas seguro de querer borrar este disco? (S/n):")) {
            int removed = remove(path.c_str());
            if (removed != 0){
                printErr("No fue posible eliminar el disco.");
                return;
            }
        }
        fclose(disk_file);
        printExitoso("Se ejecuto correctamente rmdisk");
    } else{
        printErr("El disco no existe.");
    }
}

void Disk::fdisk(int s , char u,string path,char t,char f,string del,string name,int add  ){
    FILE *disk_file = fopen(path.c_str(), "rb+");
    if (disk_file != NULL){
        Structs::MBR mbr_data;
        fseek(disk_file, 0, SEEK_SET);
        fread(&mbr_data, sizeof(Structs::MBR), 1, disk_file);

        if (add == 0 and del.empty()){
            //crear particion p,e,l
            //obtener info
            int cantidad_particiones = 0;
            int cantidad_extendidas = 0;
            int desplazamiento = 1+sizeof(mbr_data);
            bool encontro_vacio = true;
            int i_particion = 0;
            int size_acumulado = 0;

            for (Structs::Partition p_ccr : mbr_data.partitions){
                if (p_ccr.name[0] != '~'){
                    cantidad_particiones++;
                    desplazamiento += p_ccr.size+1;
                    size_acumulado += p_ccr.size;
                    if (p_ccr.type == 'e') {
                        cantidad_extendidas = 1;
                    }
                    if(encontro_vacio){
                        i_particion ++;
                    }
                    if( name == p_ccr.name){
                        cout << "\033[1;31m" << "Error: " <<  "No se puede crear una particion con el mismo nombre"<< "\033[0m"<< endl;
                        return;
                    }
                }else{
                    encontro_vacio = false;
                }
            }

            if (cantidad_particiones == 4 and t != 'l') {
                cout << "\033[1;31m" << "Error: " <<  "No se puede crear mas 4 particiones"<< "\033[0m"<< endl;
                return;
            }
            else if(cantidad_extendidas == 1 and t == 'e'){
                cout << "\033[1;31m" << "Error: " <<  "No se puede crear mas de una paricion extendida"<< "\033[0m"<< endl;
                return;
            }
            else{
                if (t!='l'){
                    //particion primaria o extendida
                    Structs::Partition partition_data;
                    partition_data.type = t;
                    partition_data.fit = f;
                    partition_data.start = desplazamiento;
                    partition_data.size = s * ( u == 'k'? 1024: u == 'm' ? 1024 * 1024 : 1);
                    strcpy( partition_data.name, name.c_str());

                    if (size_acumulado+partition_data.size > mbr_data.mbr_tamano){
                        cout << "\033[1;31m" << "Error: " <<  "Espacio insuficiente en el disco para crear la particion"<< "\033[0m"<< endl;
                        return;
                    }
                    //ACtualizar mbr
                    mbr_data.partitions[i_particion] = partition_data;
                    fseek(disk_file, 0, SEEK_SET);
                    fwrite(&mbr_data, sizeof(Structs::MBR), 1, disk_file);

                    cout << "\033[1;32m" << "Se ejecuto correctamente fdisk" << "\033[0m"<< endl;
                }
                else{
                    if (cantidad_extendidas == 1){
                        //particion logicaunit
                        int i_particione = Buscar_Pextendida(mbr_data);
                        Structs::Partition particion_e = mbr_data.partitions[i_particione];

                        //Buscar si hay EBR
                        int desplazamiento = particion_e.start;
                        Structs::EBR ebr_data;
                        bool fallo = false;
                        do {
                            fseek(disk_file, desplazamiento , SEEK_SET);
                            fread(&ebr_data, sizeof(Structs::EBR), 1, disk_file);
                            if(ebr_data.size != 0){
                                if(ebr_data.name == name){
                                    fallo = true;
                                    break;
                                }
                                desplazamiento += sizeof(Structs::EBR) + 1 + ebr_data.size + 1;
                            }
                        }while (ebr_data.size != 0);

                        if(fallo){
                            cout << "\033[1;31m" << "Error: " <<  "No se permiten nombres repetidos"<< "\033[0m"<< endl;
                            return;
                        }

                        if(desplazamiento + sizeof(Structs::EBR) + s - particion_e.start <= particion_e.size  ){
                            //creando particion logica
                            Structs::EBR ebr_new;
                            ebr_new.fit = f;
                            ebr_new.start = desplazamiento + sizeof(Structs::MBR) + 1;
                            ebr_new.size = s * ( u == 'k'? 1024: u == 'm' ? 1024 * 1024 : 1);
                            ebr_new.next = desplazamiento + sizeof(Structs::MBR) + 1 + ebr_new.size +1;
                            strcpy( ebr_new.name, name.c_str());

                            fseek(disk_file, desplazamiento, SEEK_SET);
                            fwrite(&ebr_new, sizeof(Structs::EBR), 1, disk_file);

                            cout << "\033[1;32m" << "Se ejecuto correctamente fdisk l" << "\033[0m"<< endl;
                        }else{
                            cout << "\033[1;31m" << "Error: " <<  "Particion extendida llego a su limite de capacidad"<< "\033[0m"<< endl;
                        }
                    }else{
                        cout << "\033[1;31m" << "Error: " <<  "Se encesita particion extendida para crear una logica"<< "\033[0m"<< endl;
                        return;
                    }
                }
            }
        }
        else if(add != 0){
            //agragar o quitar particion
            int i_ParticionBuscado = Buscar_Pnombre(name,mbr_data);
            if (i_ParticionBuscado != -1){
                int size_ocupado = Obtener_sizeDisco(mbr_data);
                Structs::Partition partition_data;
                partition_data = mbr_data.partitions[i_ParticionBuscado];
                size_ocupado -= partition_data.size;
                partition_data.size += add;

                if(partition_data.size <= 0){
                    cout << "\033[1;31m" << "Error: " <<  "EL tamano resultante de la particion tien eque ser mayor a 0"<< "\033[0m"<< endl;
                    return;
                }

                if(size_ocupado + partition_data.size <= mbr_data.mbr_tamano){
                    int desplazamiento = ObtenerDesplazamiento_Pnombre(name,mbr_data);

                    //Actualizar mbr
                    mbr_data.partitions[i_ParticionBuscado] = partition_data;
                    fseek(disk_file, 0, SEEK_SET);
                    fwrite(&mbr_data, sizeof(Structs::MBR), 1, disk_file);

                    //escribir particion
                    fseek(disk_file, desplazamiento, SEEK_SET);
                    fwrite(&partition_data, sizeof(partition_data), 1, disk_file);

                    cout << "\033[1;32m" << "Se ejecuto correctamente fdisk add" << "\033[0m"<< endl;
                }else{
                    cout << "\033[1;31m" << "Error: " <<  "El nuevo tamano de la paricion es mayor a la capacidad del disco"<< "\033[0m"<< endl;
                }
            }else{
                cout << "\033[1;31m" << "Error: " <<  "Se debe buscar en logicas"<< "\033[0m"<< endl;
            }

        }
        else{
            //delete particion
            if (del != "full"){
                cout << "\033[1;31m" << "Error: " <<  "Se requeria el aprametro full para delete"<< "\033[0m"<< endl;
                return;
            }
            //agragar o quitar particion
            int i_ParticionBuscado = Buscar_Pnombre(name,mbr_data);
            if (i_ParticionBuscado != -1){

                Structs::Partition partition_data_delete;
                partition_data_delete = mbr_data.partitions[i_ParticionBuscado];

                Structs::Partition partition_data;
                mbr_data.partitions[i_ParticionBuscado] = partition_data;

                char buffer[1024];
                for (int i = 0; i < 1024; i++)
                    buffer[i] = '\0';

                fseek(disk_file, partition_data_delete.start, SEEK_SET);
                for (int i = 0; i < partition_data_delete.size / 1024;i++){
                    fwrite(&buffer, 1024, 1, disk_file);
                }

                cout << "\033[1;32m" << "Se ejecuto correctamente fdisk delete" << "\033[0m"<< endl;
            }else{
                //particion logica
                int i_particione = Buscar_Pextendida(mbr_data);
                Structs::Partition particion_e = mbr_data.partitions[i_particione];

                //Buscar si hay EBR
                int desplazamiento = particion_e.start;
                Structs::EBR ebr_data;
                bool fallo = false;
                do {
                    fseek(disk_file, desplazamiento , SEEK_SET);
                    fread(&ebr_data, sizeof(Structs::EBR), 1, disk_file);
                    if(ebr_data.size != 0){
                        if(ebr_data.name == name){
                            fallo = true;
                            break;
                        }
                        desplazamiento += sizeof(Structs::EBR) + 1 + ebr_data.size + 1;
                    }
                }while (ebr_data.size != 0);

                if(!fallo){
                    cout << "\033[1;31m" << "Error: " <<  "No se encontro particion buscada"<< "\033[0m"<< endl;
                    return;
                }

                int desplazamiento_anterior = desplazamiento;
                desplazamiento += sizeof(Structs::EBR) + 1 + ebr_data.size + 1;

                fseek(disk_file, desplazamiento , SEEK_SET);
                fread(&ebr_data, sizeof(Structs::EBR), 1, disk_file);


                do {
                    //pehar el ebr sig en mi anterior
                    fseek(disk_file, desplazamiento_anterior, SEEK_SET);
                    fwrite(&ebr_data, sizeof(Structs::EBR), 1, disk_file);

                    desplazamiento_anterior = desplazamiento;
                    desplazamiento += sizeof(Structs::EBR) + 1 + ebr_data.size + 1;

                    //leer el sig
                    fseek(disk_file, desplazamiento , SEEK_SET);
                    fread(&ebr_data, sizeof(Structs::EBR), 1, disk_file);

                }while (ebr_data.size != 0);


                char letter = '\0';
                fseek(disk_file, desplazamiento_anterior, SEEK_SET);
                for (int i = 0; i < sizeof (Structs::EBR);i++){
                    fwrite(&letter, sizeof (letter), 1, disk_file);
                }

                cout << "\033[1;32m" << "Se ejecuto correctamente fdisk l delete" << "\033[0m"<< endl;
            }
        }
        fclose(disk_file);
    }else{
        cout << "\033[1;31m" << "Error: " <<  "No se encontro el archivo"<< "\033[0m"<< endl;
    }
}



void MontarParticion(string path, string name, char type,int start,int size );
void MontarParticion(string path, string name, char type,int start,int size ){
    MountedPartition Particion_montada;
    Particion_montada.name = name;
    Particion_montada.path = path;
    Particion_montada.type = type;
    Particion_montada.status = '1';

    //para hacerlo con el nombre del disco
    regex Disk_name("[a-zA-z]([a-zA-z]|[_]|[0-9])*[.][d][s][k]" );
    regex extension("[.][d][s][k]([a-zA-z]|[_]|[0-9])*" );
    smatch m;
    regex_search(path,m,Disk_name);
    string match = "";
    for (auto x : m){
        match +=x;
    }
    int count_aux=1;
    for(int i=0; i< mounted_partitions.size() ; i++){
        if(mounted_partitions.at(i).path == path ){
            count_aux++;
        }
    }
    match = regex_replace(match,extension,"");
    Particion_montada.id = to_string(51) + to_string(count_aux) + match;
    printExitoso("Id mount : "+ Particion_montada.id);
    /*int count_aux=0;
    for(int i=0; i< mounted_partitions.size() ; i++){
        if(mounted_partitions.at(i).path == path ){
            count_aux++;
        }
    }

    vector<char> alfabeto = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r','s', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    char letra = alfabeto.at(count_aux);

    count_aux=1;
    for(int i=0; i< mounted_partitions.size() ; i++){
        string nameaux = mounted_partitions.at(i).id;
        int length = nameaux.size();
        if( nameaux.at(length-1) == letra ){
            count_aux++;
        }
    }

    Particion_montada.id = to_string(19) + to_string(count_aux) + letra; */
    Particion_montada.start = start;
    Particion_montada.size = size;
    mounted_partitions.push_back(Particion_montada);
}

void Disk::mount(string path, string name) {
    FILE *disk_file = fopen(path.c_str(), "r+b");
    if (disk_file != NULL) {
        Structs::MBR mbr_data;
        fseek(disk_file, 0, SEEK_SET);
        fread(&mbr_data, sizeof(Structs::MBR), 1, disk_file);

        //Intenado montar normales
        int i_particion = Buscar_Pnombre(name,mbr_data);
        if (i_particion != -1){
            Structs::Partition parctiacion_encontrada;
            parctiacion_encontrada = mbr_data.partitions[i_particion];

            if (parctiacion_encontrada.type == 'e'){
                printErr("Err No se puede montar particion extendida");
                return;
            }

            int i_encontroMontada = Buscar_Pmontada_name_path(name,path);
            if (i_encontroMontada != -1){
                printErr("Err la particion ya esta montada");
                return;
            }

            parctiacion_encontrada.status = '1';
            MontarParticion(path,name,parctiacion_encontrada.type,parctiacion_encontrada.start,parctiacion_encontrada.size);
            printExitoso("Se ejecuto correctamente Mount");
        }
        else{
            int i_encontroMontada = Buscar_Pmontada_name_path(name,path);
            if (i_encontroMontada != -1){
                printErr("Err la particion ya esta montada");
                return;
            }

            //Buscar si hay EBR
            int i_Pextendida = Buscar_Pextendida(mbr_data);
            if (i_Pextendida == -1){
                printErr("Err no existe la particion  en el disco (e)");
                return;
            }

            Structs::Partition particion_e = mbr_data.partitions[i_Pextendida];
            int desplazamiento = particion_e.start ;

            Structs::EBR ebr_data;
            fseek(disk_file, desplazamiento , SEEK_SET);
            fread(&ebr_data, sizeof(Structs::EBR), 1, disk_file);

            if(ebr_data.size == 0){
                printErr("Err no existe ninguna particion logica");
                return;
            }

            Structs::EBR ebr_last = ebr_data;
            desplazamiento += sizeof(Structs::EBR) + 1 + ebr_data.size + 1;
            bool encontrado = false;
            if(ebr_data.name == name){
                encontrado = true;
            }
            while (!encontrado){
                fseek(disk_file, desplazamiento , SEEK_SET);
                fread(&ebr_data, sizeof(Structs::EBR), 1, disk_file);
                if(ebr_data.size != 0){
                    ebr_last = ebr_data;
                    desplazamiento += sizeof(Structs::EBR) + 1 + ebr_data.size + 1;
                }
                if(ebr_data.name == name){
                    encontrado = true;
                }
            }

            if(encontrado){
                MontarParticion(path,name,'l',ebr_last.start,ebr_last.size);
                printExitoso("Se ejecuto correctamente Mount");
            }else{
                printErr("No se encontro particion logica");
            }
        }
        fclose(disk_file);
    }else{
        printErr("No se encontro el disco buscado");
    }
}

void Disk::unmount(string id) {
    mostrar_montadas();
    bool eliminado = false;
    for(int i=0; i< mounted_partitions.size() ; i++){
        if( mounted_partitions.at(i).id == id){
            eliminado = true;
            mounted_partitions.at(i).status = '0';
            break;
        }
    }

    if(!eliminado){
        printErr("No se encontro el id para unmount");
    }else{
        printExitoso("Se ejecuto correctamente Unmount");
    }
}




int Buscar_Pextendida(Structs::MBR mbr_data){
    int i_particion = -1;
    int i = 0;
    for (Structs::Partition p_ccr : mbr_data.partitions){
        if( 'e' == p_ccr.type){
            i_particion = i;
            return i_particion;
        }
        i++;
    }
    return i_particion;
}

int Buscar_Pnombre(string name,Structs::MBR mbr_data){
    int i_particion = -1;
    int i = 0;
    for (Structs::Partition p_ccr : mbr_data.partitions){
        if( name == p_ccr.name){
            i_particion = i;
            return i_particion;
        }
        i++;
    }
    return i_particion;
}

int Obtener_sizeDisco(Structs::MBR mbr_data){
    int size = 0;
    for (Structs::Partition p_ccr : mbr_data.partitions){
        if (p_ccr.name[0] != '~'){
            size += p_ccr.size;
        }
    }
    return size;
}

int ObtenerDesplazamiento_Pnombre(string name,Structs::MBR mbr_data){
    int desplazamiento = 1 + sizeof(mbr_data);
    for (Structs::Partition p_ccr : mbr_data.partitions){
        desplazamiento = p_ccr.size +1;
        if( name == p_ccr.name){
            return desplazamiento;
        }
    }
}



void Disk::rep_disk(string id, string path) {
    string direccion_sinarchivo = '"' + path.substr(0, path.find_last_of("/\\")) + '"';
    string comando1 = "mkdir -p " + direccion_sinarchivo + "";
    system(comando1.c_str());

    FILE *disk_file = fopen(path.c_str(), "rb+");
    if(disk_file != NULL){
        string DOT = "digraph D {\n"
                     "subgraph cluster_0 {\n"
                     "bgcolor=\"#68d9e2\"\n"
                     "node [style=\"rounded\" style=filled];\n\n"
                     "node_A [shape=record    label=\"";

        Structs::MBR mbr_data;
        fseek(disk_file, 0, SEEK_SET);
        fread(&mbr_data, sizeof(Structs::MBR), 1, disk_file);
        int size_total = mbr_data.mbr_tamano;
        int size_libre_total = size_total;

        DOT += "MBR";

        for(Structs::Partition x: mbr_data.partitions){
            if(x.name[0]== '~')
                continue;

            string cadena ="|";
            if(x.type == 'p'){
                cadena += "Primaria";
            }else{
                cadena += "{Extendida";
            }

            cadena += "\\n" + convertToString(x.name,16);
            size_libre_total -= x.size;
            if(x.type != 'e'){
                float size_ocupado =  float(x.size*100)/size_total;
                cadena += "\\n" + to_string(size_ocupado) + "%";
            }else{
                int size_libre_e = x.size;
                Structs::EBR EBR_ccr;
                fseek(disk_file, x.start, SEEK_SET);
                fread(&EBR_ccr, sizeof(Structs::EBR), 1, disk_file);
                int desplazamiento = x.start;
                if(EBR_ccr.size!=0){
                    cadena += "|{";
                    bool primera = true;
                    do{
                        if(primera){
                            cadena +="EBR";
                            primera = false;
                        }else{
                            cadena +="|EBR";
                        }
                        cadena +="|Logica \\n";
                        float size_ocupado =  float(EBR_ccr.size*100)/size_total;
                        size_libre_e -= EBR_ccr.size;
                        cadena += to_string(size_ocupado) +  "%";

                        desplazamiento += sizeof(Structs::EBR) + 1 + EBR_ccr.size + 1;
                        fseek(disk_file, desplazamiento, SEEK_SET);
                        fread(&EBR_ccr, sizeof(Structs::EBR), 1, disk_file);
                    }while(EBR_ccr.size != 0);

                    if(size_libre_e >0){
                        cadena +="|Libre\\n";
                        float size_ocupado =  float(size_libre_e*100)/size_total;
                        cadena += to_string(size_ocupado) + "%";
                    }
                    cadena += "}}";
                }
            }
            DOT += cadena;
        }
        if(size_libre_total >0){
            DOT +="|Libre\\n";
            float size_ocupado =  float(size_libre_total*100)/size_total;
            DOT += to_string(size_ocupado) + "%";
        }

        DOT += "\"];\n}}";
        //print(DOT);
        exportDOT(DOT,"/home/parguet/Escritorio/mbr.dot",path);
    }else{
        cout << "\033[1;31m" << "Error: " <<  "no se puedo abrir el disco" << "\033[0m"<< endl;
        fclose(disk_file);
        return;
    }

    fclose(disk_file);
    cout << "\033[1;32m" <<   "Se ejecuto correctamente el reporte disk" << "\033[0m"<< endl;
}


string convertToString(char* a, int size){
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        int temp = (int) tolower(a[i]);

        // if( (temp > 47 and temp < 58)  or  (temp > 96 and temp < 123) or  (temp > 31 and temp < 48)  or  (temp > 57 and temp < 65)){
        if( (temp > 46 and temp < 58)  or  (temp > 96 and temp < 123) or  temp == 32 or  temp == 58  or  temp == 46 or temp == 45 or temp == 44){
            s += a[i];
        }
    }
    //s.erase(find(s.begin(), s.end(), '\0'), s.end());
    return s;
}

string agregar_dot_mbr(string parametro,string valor,string color){
    string DOT = "<TR>\n"
                 "<TD bgcolor=\"" + color + "\">" + parametro +"</TD>\n"
                                                               "<TD bgcolor=\"" + color + "\">" + valor + "</TD>\n"
                                                                                                          "</TR>\n\n";
    return DOT;
}

void exportDOT(string DOT,string dot_name,string path){
    fopen(dot_name.c_str(), "w+");
    FILE *dot_file = fopen(dot_name.c_str(), "w+");
    fwrite(DOT.c_str(), DOT.length(), 1, dot_file);
    fclose(dot_file);

    regex extension_pdf("[.][p|P][d|D][f|F]" );
    string dot_svg;
    if(regex_search(path,extension_pdf) == 1){
        dot_svg = "dot -T pdf " + dot_name + " -o " + path;
    }else{
        dot_svg = "dot -T svg " + dot_name + " -o " + path;
    }
    system(dot_svg.c_str());
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