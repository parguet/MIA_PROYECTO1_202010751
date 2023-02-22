//
// Created by parguet on 22/02/23.
//

#include "FileSystem.h"
#include "../Extras//Extras.h"
#include "../Structs//Structs.h"
#include "../Global//Global.h"
#include <string.h>
#include <cmath>
using namespace std;
Filesystem::Filesystem(){
}

void create_ext2(int n,MountedPartition partition_montada,  Structs::Superblock spr,string fecha);
void create_ext3(int n,MountedPartition partition_montada,  Structs::Superblock spr,string fecha);


void Filesystem::mkfs(string id,string type,string fs){
    //print(id);
    //print(type);
    //print(fs);
    //mostrar_montadas();

    //Bloques de carpetas [4] contendt //64 bytes
    // content -> name[12] , apuntador  //16 bytes

    //BLoque de apuntadores [16] apuntadores  //64 bytes

    //Bloque de archivo [64] bytes  //64 bytes

    int i_particion_montada = Buscar_Pmontada_id(id);
    if(i_particion_montada == -1 ){
        printErr("Err no se encontro la particion para mkfs");
        return;
    }
    MountedPartition partition_montada = mounted_partitions.at(i_particion_montada);
    int n = fs == "2fs" ? floor((partition_montada.size - sizeof(Structs::Superblock)) /(4 + sizeof(Structs::Inodes) + 3 * sizeof(Structs::Fileblock))) : floor((partition_montada.size - sizeof(Structs::Superblock)) /(4 + sizeof(Structs::Inodes) + 3 * sizeof(Structs::Fileblock) + sizeof(Structs::Journaling)));

    Structs::Superblock spr;
    spr.s_inodes_count = spr.s_free_inodes_count = n;
    spr.s_blocks_count = spr.s_free_blocks_count = 3 * n;
    string fecha = obtenerFecha();
    strcpy( spr.s_mtime , fecha.c_str());
    strcpy( spr.s_umtime , fecha.c_str());
    spr.s_mnt_count = 1;

    //division
    if(fs == "2fs" ){
        create_ext2(n,partition_montada,spr,fecha);
    }else if(fs == "3fs"){
        create_ext3(n,partition_montada,spr,fecha);
    }else{
        printErr("Sistema de archivo irreconocible");
    }
}


void create_ext3(int n,MountedPartition partition_montada,  Structs::Superblock spr,string fecha){
    spr.s_filesystem_type = 3;
    Structs::Journaling journaling;
    journaling.size = -1;
    string operacion_J = "mkdir";
    string path = "/";
    string content = "-";

    strcpy(journaling.contenido[0].operation, operacion_J.c_str());
    strcpy(journaling.contenido[0].path, path.c_str());
    strcpy(journaling.contenido[0].content, content.c_str());
    strcpy(journaling.contenido[0].date, fecha.c_str());

    operacion_J = "mkfile";
    path = "/users.txt";
    content = "1,G,root\n1,U,root,root,123\n";

    strcpy(journaling.contenido[1].operation, operacion_J.c_str());
    strcpy(journaling.contenido[1].path, path.c_str());
    strcpy(journaling.contenido[1].content, content.c_str());
    strcpy(journaling.contenido[1].date, fecha.c_str());

    spr.s_bm_inode_start = partition_montada.start + sizeof(Structs::Superblock) + sizeof (Structs::Journaling);
    spr.s_bm_block_start = spr.s_bm_inode_start + n;
    spr.s_inode_start = spr.s_bm_block_start + (3 * n);
    spr.s_block_start = spr.s_inode_start + (n * sizeof(Structs::Inodes)); //spr.s_block_start = spr.s_bm_inode_start + (n * sizeof(Structs::Inodes));

    //se crea inodo 0, se crea carpeta 0, se crea inodo 1 para user txt, ese inodo crea un bloque de archivos
    spr.s_free_inodes_count--;
    spr.s_free_blocks_count--;
    spr.s_free_inodes_count--;
    spr.s_free_blocks_count--;
    FILE *bfile = fopen(partition_montada.path.c_str(), "rb+");
    fseek(bfile, partition_montada.start, SEEK_SET);
    fwrite(&spr, sizeof(Structs::Superblock), 1, bfile);
    fwrite(&journaling, sizeof(Structs::Journaling), 1, bfile);

    char zero = '0';
    fseek(bfile, spr.s_bm_inode_start, SEEK_SET);
    for (int i = 0; i < n; i++) {
        fwrite(&zero, sizeof(zero), 1, bfile);
    }

    fseek(bfile, spr.s_bm_block_start, SEEK_SET);
    for (int i = 0; i < (3 * n); i++) {
        fwrite(&zero, sizeof(zero), 1, bfile);
    }

    Structs::Inodes inode;
    fseek(bfile, spr.s_inode_start, SEEK_SET);
    for (int i = 0; i < n; i++) {
        fwrite(&inode, sizeof(Structs::Inodes), 1, bfile);
    }

    Structs::Folderblock folder;
    fseek(bfile, spr.s_block_start, SEEK_SET);
    for (int i = 0; i < (3 * n); i++) {
        fwrite(&folder, sizeof(Structs::Folderblock), 1, bfile);
    }
    fclose(bfile);

    Structs::Superblock recuperado;
    FILE *archivo = fopen(partition_montada.path.c_str(), "rb");
    fseek(archivo, partition_montada.start, SEEK_SET);
    fread(&recuperado, sizeof(Structs::Superblock), 1, archivo);
    fclose(archivo);
    inode.i_uid = 1;
    inode.i_gid = 1;
    inode.i_size = 0;
    inode.i_size = 0;
    strcpy( inode.i_atime  , fecha.c_str());
    strcpy( inode.i_ctime  , fecha.c_str());
    strcpy( inode.i_mtime  , fecha.c_str());
    inode.i_type = 0;
    inode.i_perm[0] = '6';
    inode.i_perm[1] = '6';
    inode.i_perm[2] = '4';

    inode.i_block[0] = 0;

    Structs::Folderblock fb;
    strcpy(fb.b_content[0].b_name, ".");
    fb.b_content[0].b_inodo = 0;
    strcpy(fb.b_content[1].b_name, "..");
    fb.b_content[1].b_inodo = 0;
    strcpy(fb.b_content[2].b_name, "user.txt");
    fb.b_content[2].b_inodo = 1;

    string data = "1,G,root\n1,U,root,root,123\n";
    Structs::Inodes inodetmp;
    inodetmp.i_uid = 1;
    inodetmp.i_gid = 1;
    inodetmp.i_size = sizeof(data.c_str()) + sizeof(Structs::Fileblock);
    strcpy( inodetmp.i_atime , fecha.c_str());
    strcpy( inodetmp.i_ctime  , fecha.c_str());
    strcpy( inodetmp.i_mtime  , fecha.c_str());
    inodetmp.i_type = 1;
    inodetmp.i_perm[0] = '6';
    inodetmp.i_perm[1] = '6';
    inodetmp.i_perm[2] = '4';
    inodetmp.i_block[0] = 1;

    inode.i_size = inodetmp.i_size + sizeof(Structs::Folderblock) + sizeof(Structs::Inodes);

    Structs::Fileblock fileb;
    strcpy(fileb.b_content, data.c_str());

    FILE *bfiles = fopen(partition_montada.path.c_str(), "rb+");
    fseek(bfiles, spr.s_bm_inode_start, SEEK_SET);
    char caracter = '1';
    fwrite(&caracter, sizeof(caracter), 1, bfiles);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);

    fseek(bfiles, spr.s_bm_block_start, SEEK_SET);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);

    fseek(bfiles, spr.s_inode_start, SEEK_SET);
    fwrite(&inode, sizeof(Structs::Inodes), 1, bfiles);
    fwrite(&inodetmp, sizeof(Structs::Inodes), 1, bfiles);

    fseek(bfiles, spr.s_block_start, SEEK_SET);
    fwrite(&fb, sizeof(Structs::Folderblock), 1, bfiles);
    fwrite(&fileb, sizeof(Structs::Fileblock), 1, bfiles);
    fclose(bfiles);

    printExitoso("Se ejecuto correctamente mkfs ext3");
}

void create_ext2(int n,MountedPartition partition_montada,  Structs::Superblock spr,string fecha){
    spr.s_filesystem_type = 2;

    spr.s_bm_inode_start = partition_montada.start + sizeof(Structs::Superblock);
    spr.s_bm_block_start = spr.s_bm_inode_start + n;
    spr.s_inode_start = spr.s_bm_block_start + (3 * n);
    spr.s_block_start = spr.s_inode_start + (n * sizeof(Structs::Inodes)); //spr.s_block_start = spr.s_bm_inode_start + (n * sizeof(Structs::Inodes));

    //se crea inodo 0, se crea carpeta 0, se crea inodo 1 para user txt, ese inodo crea un bloque de archivos
    spr.s_free_inodes_count--;
    spr.s_free_blocks_count--;
    spr.s_free_inodes_count--;
    spr.s_free_blocks_count--;
    FILE *bfile = fopen(partition_montada.path.c_str(), "rb+");
    fseek(bfile, partition_montada.start, SEEK_SET);
    fwrite(&spr, sizeof(Structs::Superblock), 1, bfile);

    char zero = '0';
    fseek(bfile, spr.s_bm_inode_start, SEEK_SET);
    for (int i = 0; i < n; i++) {
        fwrite(&zero, sizeof(zero), 1, bfile);
    }

    fseek(bfile, spr.s_bm_block_start, SEEK_SET);
    for (int i = 0; i < (3 * n); i++) {
        fwrite(&zero, sizeof(zero), 1, bfile);
    }

    Structs::Inodes inode;
    fseek(bfile, spr.s_inode_start, SEEK_SET);
    for (int i = 0; i < n; i++) {
        fwrite(&inode, sizeof(Structs::Inodes), 1, bfile);
    }

    Structs::Folderblock folder;
    fseek(bfile, spr.s_block_start, SEEK_SET);
    for (int i = 0; i < (3 * n); i++) {
        fwrite(&folder, sizeof(Structs::Folderblock), 1, bfile);
    }
    fclose(bfile);

    Structs::Superblock recuperado;
    FILE *archivo = fopen(partition_montada.path.c_str(), "rb");
    fseek(archivo, partition_montada.start, SEEK_SET);
    fread(&recuperado, sizeof(Structs::Superblock), 1, archivo);
    fclose(archivo);
    inode.i_uid = 1;
    inode.i_gid = 1;
    inode.i_size = 0;
    inode.i_size = 0;
    strcpy( inode.i_atime  , fecha.c_str());
    strcpy( inode.i_ctime  , fecha.c_str());
    strcpy( inode.i_mtime  , fecha.c_str());
    inode.i_type = 0;
    inode.i_perm[0] = '6';
    inode.i_perm[1] = '6';
    inode.i_perm[2] = '4';

    inode.i_block[0] = 0;

    Structs::Folderblock fb;
    strcpy(fb.b_content[0].b_name, ".");
    fb.b_content[0].b_inodo = 0;
    strcpy(fb.b_content[1].b_name, "..");
    fb.b_content[1].b_inodo = 0;
    strcpy(fb.b_content[2].b_name, "user.txt");
    fb.b_content[2].b_inodo = 1;

    string data = "1,G,root\n1,U,root,root,123\n";
    Structs::Inodes inodetmp;
    inodetmp.i_uid = 1;
    inodetmp.i_gid = 1;
    inodetmp.i_size = sizeof(data.c_str()) + sizeof(Structs::Fileblock);
    strcpy( inodetmp.i_atime , fecha.c_str());
    strcpy( inodetmp.i_ctime  , fecha.c_str());
    strcpy( inodetmp.i_mtime  , fecha.c_str());
    inodetmp.i_type = 1;
    inodetmp.i_perm[0] = '6';
    inodetmp.i_perm[1] = '6';
    inodetmp.i_perm[2] = '4';
    inodetmp.i_block[0] = 1;

    inode.i_size = inodetmp.i_size + sizeof(Structs::Folderblock) + sizeof(Structs::Inodes);

    Structs::Fileblock fileb;
    strcpy(fileb.b_content, data.c_str());

    FILE *bfiles = fopen(partition_montada.path.c_str(), "rb+");
    fseek(bfiles, spr.s_bm_inode_start, SEEK_SET);
    char caracter = '1';
    fwrite(&caracter, sizeof(caracter), 1, bfiles);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);

    fseek(bfiles, spr.s_bm_block_start, SEEK_SET);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);
    fwrite(&caracter, sizeof(caracter), 1, bfiles);

    fseek(bfiles, spr.s_inode_start, SEEK_SET);
    fwrite(&inode, sizeof(Structs::Inodes), 1, bfiles);
    fwrite(&inodetmp, sizeof(Structs::Inodes), 1, bfiles);

    fseek(bfiles, spr.s_block_start, SEEK_SET);
    fwrite(&fb, sizeof(Structs::Folderblock), 1, bfiles);
    fwrite(&fileb, sizeof(Structs::Fileblock), 1, bfiles);
    fclose(bfiles);

    printExitoso("Se ejecuto correctamente mkfs ext2");
}

