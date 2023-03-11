//
// Created by parguet on 16/02/23.
//

#ifndef _MIA__P1_202010751_STRUCTS_H
#define _MIA__P1_202010751_STRUCTS_H
#include <string>
#include <ctime>
#include "../Global//Global.h"

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

    typedef struct _EBR {
        char status = '0';
        char fit;
        int start;
        int size = 0;
        int next = -1;
        char name[16];
    } EBR;

    typedef struct _Inodes {
        int i_uid = -1;
        int i_gid = -1;
        int i_size = -1;
        char i_atime[17];
        char i_ctime[17];
        char i_mtime[17];
        int i_block[15] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        char i_type = -1; //0 = folder , 1 = archivo
        char i_perm[3] = {-1,-1,-1};
    } Inodes;


    typedef struct _Superblock {
        int s_filesystem_type;
        int s_inodes_count;
        int s_blocks_count;
        int s_free_blocks_count;
        int s_free_inodes_count;
        char s_mtime[17];
        char s_umtime[17];
        int s_mnt_count;
        int s_magic = 0xEF53;
        int s_inode_size = sizeof(Inodes);
        int s_block_size= sizeof(Folderblock);
        int s_fist_ino = 0;
        int s_first_blo = 0;
        int s_bm_inode_start;
        int s_bm_block_start;
        int s_inode_start;
        int s_block_start;
    } Superblock;

    typedef struct _Content {
        char b_name[12];
        int b_inodo = -1;
    } Content;

    typedef struct _Folderblock //64
    {
        Content b_content[4];
    } Folderblock;

    typedef struct _Fileblock //64
    {
        char b_content[64];
    } Fileblock;

    typedef struct _Content_J {
        char operation[10] = "";
        char path[100] = "";
        char content[100] = "";
        char date[17];
    } Content_J;

    typedef struct _Journaling {
        int size = 0;
        int ultimo = 1;
        Content_J contenido[50];
    } Journaling;

    static int BuscarInodo(string path,MountedPartition particionmontada,Superblock superblick,FILE *disk_file);
    static string LecturaInodo(int indice_inodo, Superblock superblock,FILE *disk_file);
    static int EscrituraInodo(MountedPartition partition_montada, int indice_inodo, Structs::Superblock superblock, string cadena,FILE *disk_file );
    static int Buscar_CreandoInodo(string path,MountedPartition particionmontada,Superblock superblick,FILE *disk_file);
    static int Crear_folder_inodo(MountedPartition partition_montada, int indice_inodo, Structs::Superblock superblock,string nombrefolder,FILE *disk_file);
    static int Crear_txt_inodo(MountedPartition partition_montada, int indice_inodo, Structs::Superblock superblock,string cadena_txt,string nombretxt,FILE *disk_file);
    static void Update_journaling(string operacion,string path,string content,MountedPartition partition_montada, Structs::Superblock superblock,FILE *disk_file);
    static bool permisos(int no_inodo,MountedPartition partition_montada,FILE *disk_file,char permiso_necesario);
    static void Eliminar_updateinodos(FILE *disk_file, MountedPartition partition_montada, int no_inodo,Structs::Superblock superblock);
    static int Eliminar_elemnto(string nombre,FILE *disk_file,MountedPartition partition_montada,int no_inodo);
    static int Update_namefolder_inodo(MountedPartition partition_montada, int indice_inodo, Structs::Superblock superblock,string name_buscado,string name_new, FILE *disk_file);
    static int Copiar_elemento(FILE *disk_file,MountedPartition partition_montada,int no_inodo,Structs::Superblock superblock,int padre,string nombre);
    static int Agregar_elemnto(string nombre,int inodo_agregar,FILE *disk_file,MountedPartition partition_montada,int no_inodo);
    static void recorrer_find(Structs::Inodes inodo_ccr,Structs::Superblock superblock , FILE *disk_file,int profundidad,int tipo);


};


#endif //_MIA__P1_202010751_STRUCTS_H
