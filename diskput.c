#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <time.h>
#include "Constants.h"

void formatTime(date *dir_date)
{
    time_t t;
    struct tm * timeinfo;
    time (&t);
    timeinfo = localtime (&t);

    dir_date->year = timeinfo->tm_year+1900;
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

    //get file size
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

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

    dir.status = 0x3;
    dir.start_block = 0;
    dir.num_block = file_size/DEFAULT_BLOCK_SIZE + 1;
    dir.file_size = file_size;

    formatTime(&dir.create_time);
    //TODO: should be samed
    formatTime(&dir.modify_time);

    //TODO: strip file name from path
    strncpy(dir.file_name, "test.txt", DIRECTORY_MAX_NAME_LENGTH);
    strcpy(dir.unused, "\x00\xFF\xFF\xFF\xFF\xFF");

    //while
    //find available block in FAT
    // save a block of data
    // ensure that disk has space for this file
    int cur_block;
    int last_block = -1;
    int to_write = dir.num_block;
    // for (int i = 0; i < fat_blocks * FAT_ENTRY_PER_BLOCK; i++) {
    //     //read fat entry
    //     fseek(disk, fat_start * DEFAULT_BLOCK_SIZE + i * FAT_ENTRY_SIZE, SEEK_SET);
    //     fread(&cur_block, FAT_ENTRY_SIZE, 1, disk);
    //     cur_block = ntohl(cur_block);
    //
    //     if (cur_block == FAT_FREE)
    //     {
    //         if (dir.start_block == -1)
    //         {
    //             dir.start_block = cur_block;
    //         }
    //         //TODO:write data at cur_block
    //         to_write -= 1;
    //         //write cur_block number at last_block FAT entry
    //         if (last_block != -1)
    //         {
    //             fseek(disk, fat_start * DEFAULT_BLOCK_SIZE + last_block * FAT_ENTRY_SIZE, SEEK_SET);
    //             //FIXME: change ENDIAN?
    //             fwrite(&cur_block, FAT_ENTRY_SIZE, 1, disk);
    //         }
    //         last_block = i;
    //     }
    //
    //     //FIXME: <= 0 ?
    //     if (to_write <= 0) {
    //         fseek(disk, fat_start * DEFAULT_BLOCK_SIZE + cur_block * FAT_ENTRY_SIZE, SEEK_SET);
    //         int file_end = FAT_EOF;
    //         fwrite(&file_end, FAT_ENTRY_SIZE, 1, disk);
    //         break;
    //     }
    //
    // }


    //find an empty spot and write directory entry to root dir
    char status = 0;
    for (int i = 0; i < DIRECTORY_ENTRY_PER_BLOCK * root_blocks; i++)
    {
        int entry = root_start * DEFAULT_BLOCK_SIZE + i*DIRECTORY_ENTRY_SIZE;

        fseek(disk, entry + DIRECTORY_STATUS_OFFSET, SEEK_SET);
        fread(&status, DIRECTORY_STATUS_SIZE, 1, disk);
        if (status == DIRECTORY_ENTRY_FREE)
        {
            fseek(disk, entry + DIRECTORY_STATUS_OFFSET, SEEK_SET);
            fwrite(&dir, DIRECTORY_ENTRY_SIZE, 1, disk);
            break;
        }
    }

    return 0;
}
