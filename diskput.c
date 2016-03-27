#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <libgen.h>
#include <time.h>
#include "Constants.h"

void formatTime(date *dir_date)
{
    time_t t;
    struct tm * timeinfo;
    time (&t);
    timeinfo = localtime (&t);

    dir_date->year = htons(timeinfo->tm_year+1900);
    dir_date->month = timeinfo->tm_mon+1;
    dir_date->date = timeinfo->tm_mday;
    dir_date->hour = timeinfo->tm_hour;
    dir_date->min = timeinfo->tm_min;
    dir_date->sec = timeinfo->tm_sec;
}

int main(int argc, char const *argv[]) {
    if (argc < 3)
    {
        printf("Error: Invalid Input");
        exit(1);
    }

    FILE *disk = fopen(argv[1], "r+");
    FILE *file = fopen(argv[2], "r");

    if (file == NULL || disk == NULL)
    {
        printf("File not found.\n");
        exit(1);
    }

    int fat_start;
    fseek(disk, FATSTART_OFFSET, SEEK_SET);
    fread(&fat_start, SUPERBLOCK_INFO_SIZE, 1, disk);
    fat_start = ntohl(fat_start);

    int fat_blocks;
    fread(&fat_blocks, SUPERBLOCK_INFO_SIZE, 1, disk);
    fat_blocks = ntohl(fat_blocks);

    int root_start;
    fread(&root_start, SUPERBLOCK_INFO_SIZE, 1, disk);
    root_start = ntohl(root_start);

    int root_blocks;
    fread(&root_blocks, SUPERBLOCK_INFO_SIZE, 1, disk);
    root_blocks = ntohl(root_blocks);

    directory_entry dir;
    memset(&dir, 0, sizeof(dir));

    //get file size
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    dir.status = 0x3;
    dir.start_block = 0;
    dir.num_block = htonl(file_size/DEFAULT_BLOCK_SIZE + 1);
    dir.file_size = htonl(file_size);

    formatTime(&dir.create_time);
    formatTime(&dir.modify_time);

    char path[BUFFER_SIZE];
    strcpy(path, argv[2]);
    strncpy(dir.file_name, basename(path), sizeof(dir.file_name));

    //while
    //find available block in FAT
    // save a block of data
    // ensure that disk has space for this file
    int cur_block_data;
    int last_block = -1;
    int to_write = ntohl(dir.num_block);
    char *str = (char *) malloc(DEFAULT_BLOCK_SIZE);
    for (int block = 0; block < fat_blocks * FAT_ENTRY_PER_BLOCK; block++)
    {
        //read fat entry
        fseek(disk, fat_start * DEFAULT_BLOCK_SIZE + block * FAT_ENTRY_SIZE, SEEK_SET);
        fread(&cur_block_data, FAT_ENTRY_SIZE, 1, disk);
        cur_block_data = ntohl(cur_block_data);

        if (cur_block_data == FAT_FREE)
        {
            printf("BLOCK written at:%d\n", block);

            if (dir.start_block == 0)
            {
                dir.start_block = htonl(block);
            }
            //TODO:write data at block
            to_write -= 1;
            fseek(file, 0L, SEEK_CUR);
            fread(str, DEFAULT_BLOCK_SIZE, 1, file);

            fseek(disk, block * DEFAULT_BLOCK_SIZE, SEEK_SET);
            fwrite(str, DEFAULT_BLOCK_SIZE, 1, disk);

            //write block number at last_block FAT entry
            if (last_block != -1)
            {
                fseek(disk, fat_start * DEFAULT_BLOCK_SIZE + last_block * FAT_ENTRY_SIZE, SEEK_SET);
                int block_big_endian = htonl(block);
                fwrite(&block_big_endian, FAT_ENTRY_SIZE, 1, disk);
            }
            last_block = block;

            if (to_write <= 0)
            {
                fseek(disk, fat_start * DEFAULT_BLOCK_SIZE + block * FAT_ENTRY_SIZE, SEEK_SET);
                int file_end = FAT_EOF;
                fwrite(&file_end, FAT_ENTRY_SIZE, 1, disk);
                break;
            }
        }

    }


    //find an empty spot and write directory entry to root dir
    char status = 0;
    for (int i = 0; i < DIRECTORY_ENTRY_PER_BLOCK * root_blocks; i++)
    {
        int entry = root_start * DEFAULT_BLOCK_SIZE + i*DIRECTORY_ENTRY_SIZE;

        fseek(disk, entry + DIRECTORY_STATUS_OFFSET, SEEK_SET);
        fread(&status, DIRECTORY_STATUS_SIZE, 1, disk);
        if (status == DIRECTORY_ENTRY_FREE)
        {
            fseek(disk, entry, SEEK_SET);
            fwrite(&dir.status, 1, 1, disk);
            fwrite(&dir.start_block, 4, 1, disk);
            fwrite(&dir.num_block, 4, 1, disk);
            fwrite(&dir.file_size, 4, 1, disk);

            fwrite(&dir.create_time, 7, 1, disk);
            fwrite(&dir.modify_time, 7, 1, disk);

            fwrite(&dir.file_name, 31, 1, disk);
            fwrite("\x00\xFF\xFF\xFF\xFF\xFF", 6, 1, disk);
            break;
        }
    }

    fclose(disk);
    fclose(file);
    return 0;
}
