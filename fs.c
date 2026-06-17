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


int fs_init() {
  printf("Função não implementada: fs_init\n");
  return 1;
}

int fs_format() {
  int buffer[1];
  for(int i = 0; i<32; i++) {
    fat[i] = 3; // Agrupamentos da própria FAT
  }
  fat[32] = 4; // Agrupamento do diretório
  for(int i = 33; i < FATCLUSTERS;i++) {
    fat[i] = 1; 
  }
  /*for (int i = total_setores; i < FATCLUSTERS; i++) {
    fat[i] = 1;  // Marca o resto da FAT além do tamanho do disco (se existir) como livre
  }*/

  for (int i = 0; i < DIRENTRIES; i++) {
    dir[i].used = 0; // Nenhuma entrada do diretório está em uso
  }

  //FALTA GRAVAR A FAT E GRAVAR O DIRETÓRIO
  

  return 1;
}

int fs_free() {
  int free_clusters = 0;
  for (int i = 33; i < FATCLUSTERS; i++) { // Podemos começar direto no 33 já que os primeiros 32 estão ocupados pela FAT e o 33 pelo diretório e sempre vão estar ocupados (eu acho)
    if (fat[i] == 1) {
      free_clusters++;
    }
  }
  return free_clusters * CLUSTERSIZE;
}

int fs_list(char *buffer, int size) {
  printf("Função não implementada: fs_list\n");
  return 0;
}

int fs_create(char* file_name) {
  if(strlen(file_name) > 25) {
    printf("Nome do arquivo deve ter no maximo 25 caracteres\n");
    return 0;
  }
  int primeiro_livre = 0;
  int controle = 0;
  for(int i = 0; i < DIRENTRIES; i++){
    if (controle == 0 && dir[i].used == 0) {
      primeiro_livre = i; //Só pra n ter que fazer 2 loops, um pra verificar e um pra criar
      controle = 1;
    } else if (dir[i].used == 1 && strncmp(dir[i].name, file_name, 25) == 0) { // Se o arquivo existir e a strcmp der 0, significa que o nome do arquivo já existe no diretório
      printf("Ja existe arquivo com esse nome\n");
      return 0;
    }
  }
  dir[primeiro_livre].used = 1;
  strncpy(dir[primeiro_livre].name, file_name, 25);
  dir[primeiro_livre].first_block = 0; 
  dir[primeiro_livre].size = 0;
  return 1;
}

int fs_remove(char *file_name) {
  printf("Função não implementada: fs_remove\n");
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

