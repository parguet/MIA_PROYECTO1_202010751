//
// Created by parguet on 16/02/23.
//

#ifndef _MIA__P1_202010751_STRUCTS_H
#define _MIA__P1_202010751_STRUCTS_H


class Structs {
    public:
        Structs();

    typedef struct _Partition {
        char status = '0'; // 0 no usado. 1 ya esta en uso
        char type = 'p';  // P, E, L
        char fit = 'w'; // configuracion wf, bf, ff
        int start = -1; //
        int size = -1; //
        char name[16] = "~"; //nombre maximo de 16 char
    } Partition;

    typedef struct _MBR
    {
        int mbr_tamano;
        char FechaCreacion[17];
        int mbr_disk_signature;
        char disk_fit;
        Partition partitions[4];
    } MBR;
};


#endif //_MIA__P1_202010751_STRUCTS_H
