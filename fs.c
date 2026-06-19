/*
 * RSFS - Really Simple File System
 *
 * Copyright © 2010,2012,2019 Gustavo Maciel Dias Vieira
 * Copyright © 2010 Rodrigo Rocco Barbieri
 *
 * This file is part of RSFS.
 *
 * RSFS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

#define CLUSTERSIZE 4096
#define FATCLUSTERS 65536
#define DIRENTRIES 128

unsigned short fat[FATCLUSTERS];

typedef struct {
  char used;
  char name[25];
  unsigned short first_block;
  int size;
} dir_entry;

dir_entry dir[DIRENTRIES];

int write_fat();//Declaração das funções auxiliares
int write_dir();
int formatado();

int fs_init(){
  for (int i = 0; i < 32; i++) {
    if(bl_read(i, ((char*)fat) + (i * CLUSTERSIZE)) == 0) {
      return 0;
    }
  }
  if(bl_read(32,(char *)dir) == 0){
    return 0;
  }
  if(!formatado()){
    printf("O disco nao esta formatado\n");
  }
  return 1;
}

int fs_format() {
  for(int i = 0; i<32; i++) {
    fat[i] = 3; // Agrupamentos da própria FAT
  }
  fat[32] = 4; // Agrupamento do diretório
  for(int i = 33; i < bl_size();i++) {
    fat[i] = 1; 
  }
  for (int i = 0; i < DIRENTRIES; i++) {
    dir[i].used = 0; // Nenhuma entrada do diretório está em uso
  }
  if(write_fat() == 0 || write_dir() == 0) {
    return 0;
  }
  return 1;
}

int fs_free(){
  if(!formatado()){
    printf("[ERRO] Disco não formatado\n");
    return 0;
  }
  int free_clusters = 0;
  for (int i = 33; i < bl_size(); i++) { // Podemos começar direto no 33 já que os primeiros 32 estão ocupados pela FAT e o 33 pelo diretório e sempre vão estar ocupados (eu acho)
    if (fat[i] == 1) {
      free_clusters++;
    }
  }
  return free_clusters * CLUSTERSIZE;
}

int fs_list(char *buffer, int size) {
  if(!formatado()){
    printf("[ERRO] Disco não formatado\n");
    return 0;
  }
  if (size <= 0) {
    return 0;
  }
    
  buffer[0] = '\0';
  char buffer2[64];
  for (int i = 0; i < DIRENTRIES; i++) {
    if(dir[i].used == 1) {
      sprintf(buffer2, "%s\t\t%d\n", dir[i].name, dir[i].size);
      if (strlen(buffer) + strlen(buffer2) + 1 <= size) {
          strcat(buffer, buffer2);
      } else {
          break;
      }
    }
  }
  
  return 1;
}

int fs_create(char* file_name){
  if(!formatado()){
    printf("[ERRO] Disco não formatado\n");
    return 0;
  }
  if(strlen(file_name) > 24) {
    printf("[ERRO] Nome do arquivo deve ter no maximo 25 caracteres\n");
    return 0;
  }
  int primeiro_livre = -1;
  int controle = 0;
  for(int i = 0; i < DIRENTRIES; i++){
    if (controle == 0 && dir[i].used == 0) {
      primeiro_livre = i; //Só pra n ter que fazer 2 loops, um pra verificar e um pra criar
      controle = 1;
    } else if (dir[i].used == 1 && strncmp(dir[i].name, file_name, 25) == 0) { // Se o arquivo existir e a strcmp der 0, significa que o nome do arquivo já existe no diretório
      printf("[ERRO] Ja existe arquivo com esse nome\n");
      return 0;
    }
  }
  if(primeiro_livre == -1){
    printf("[ERRO] Diretorio cheio\n");
    return 0;
  }
  dir[primeiro_livre].used = 1;
  strncpy(dir[primeiro_livre].name, file_name, 25);
  dir[primeiro_livre].first_block = 0; 
  dir[primeiro_livre].size = 0;
  write_dir();
  return 1;
}

int fs_remove(char *file_name) {
  if(!formatado()){
    printf("[ERRO] Disco não formatado\n");
    return 0;
  }
  for (int i = 0; i < DIRENTRIES; i++) {
    if (dir[i].used == 1 && strncmp(dir[i].name, file_name, 25) == 0) {
      dir[i].used = 0; // Marca a entrada do diretório como livre
      unsigned short block = dir[i].first_block;
      if (dir[i].size > 0){
        while(1){
          if(fat[block] == 2){
            fat[block] = 1; // Marca o bloco como livre
            break;
          } else {
            unsigned short next_block = fat[block];
            fat[block] = 1; // Marca o bloco como livre
            block = next_block;
          }
        }
      }
      write_fat();
      write_dir();
      return 1;
    }
  }
  printf("[ERRO] nao existe arquivo com esse nome\n");
  return 0;
}

int fs_open(char *file_name, int mode) {
  printf("Função não implementada: fs_open\n");
  return -1;
}

int fs_close(int file)  {
  printf("Função não implementada: fs_close\n");
  return 0;
}

int fs_write(char *buffer, int size, int file) {
  printf("Função não implementada: fs_write\n");
  return -1;
}

int fs_read(char *buffer, int size, int file) {
  printf("Função não implementada: fs_read\n");
  return -1;
}

//funcoes auxiliares para nao ficar copiando o mesmo codigo
int write_fat(){
  for (int i = 0; i < 32; i++) {
    if(bl_write(i, ((char*)fat) + (i * CLUSTERSIZE)) == 0) {
      return 0;
    }
  }
  return 1;
}
int write_dir(){
  if(bl_write(32, (char*)dir) == 0) {
    return 0;
  }
  return 1;
}
int formatado(){
  for(int i = 0; i < 32; i++){
    if(fat[i] != 3){
      return 0; //N formatado chefe
    }
  }
  if(fat[32] != 4){
    return 0;
  }
  return 1; //Retorna 1 para formatado
}