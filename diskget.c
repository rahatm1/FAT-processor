#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include "Constants.h"

void processFile(FILE *fp, int dir_entry, char *file_name)
{
    int fat_start;
    fseek(fp, FATSTART_OFFSET, SEEK_SET);
    fread(&fat_start, SUPERBLOCK_INFO_SIZE, 1, fp);
    fat_start = ntohl(fat_start);

    FILE *out = fopen(file_name, "w");
    char *str = (char *) malloc(DEFAULT_BLOCK_SIZE);

    unsigned int block;
    fseek(fp, dir_entry + DIRECTORY_START_BLOCK_OFFSET, SEEK_SET);
    fread(&block, DIRECTORY_START_BLOCK_SIZE, 1, fp);
    block = ntohl(block);

    int file_size;
    fseek(fp, dir_entry + DIRECTORY_FILE_SIZE_OFFSET, SEEK_SET);
    fread(&file_size, DIRECTORY_FILE_SIZE_SIZE, 1, fp);
    file_size = ntohl(file_size);

    while (block != FAT_EOF) {
        //read data and write to out
        fseek(fp, block * DEFAULT_BLOCK_SIZE, SEEK_SET);
        fread(str, DEFAULT_BLOCK_SIZE, 1, fp);

        if (file_size < DEFAULT_BLOCK_SIZE)
        {
            fwrite(str, file_size, 1, out);
        }
        else
        {
            fwrite(str, DEFAULT_BLOCK_SIZE, 1, out);
            file_size -= DEFAULT_BLOCK_SIZE;
        }

        //read FAT entry
        fseek(fp, fat_start * DEFAULT_BLOCK_SIZE + block * FAT_ENTRY_SIZE, SEEK_SET);
        fread(&block, FAT_ENTRY_SIZE, 1, fp);
        block = ntohl(block);
    }
    free(str);
}

int main(int argc, char const *argv[]) {
    if (argc < 3)
    {
        printf("Error: Invalid Input");
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Error: File doesn't exist");
        exit(1);
    }

    char file_name[DIRECTORY_MAX_NAME_LENGTH+1];
    strcpy(file_name, argv[2]);

    fseek(fp, ROOTDIRSTART_OFFSET, SEEK_SET);
    int root_start;
    fread(&root_start, SUPERBLOCK_INFO_SIZE, 1, fp);
    root_start = ntohl(root_start);

    int root_blocks;
    fread(&root_blocks, SUPERBLOCK_INFO_SIZE, 1, fp);
    root_blocks = ntohl(root_blocks);

    char cur_file[DIRECTORY_MAX_NAME_LENGTH + 1];

    short status;
    for (int i = 0; i < DIRECTORY_ENTRY_PER_BLOCK * root_blocks; i++) {
        int entry = root_start * DEFAULT_BLOCK_SIZE + i*DIRECTORY_ENTRY_SIZE;

        fseek(fp, entry + DIRECTORY_STATUS_OFFSET, SEEK_SET);
        fread(&status, DIRECTORY_STATUS_SIZE, 1, fp);
        if (status == (DIRECTORY_ENTRY_USED | DIRECTORY_ENTRY_FILE))
        {
            fseek(fp, entry + DIRECTORY_FILENAME_OFFSET, SEEK_SET);
            fread(cur_file, DIRECTORY_MAX_NAME_LENGTH, 1, fp);

            if (strcmp(cur_file, file_name) == 0) {
                processFile(fp, entry, file_name);
                return 0;
            }
        }
    }

    printf("File not found.\n");

    return 0;
}
