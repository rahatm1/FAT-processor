#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include "Constants.h"

void getDate(date *modDate, char *date_string)
{
    sprintf(date_string, "%4d/%02d/%02d %02d:%02d:%02d",
        ntohs(modDate->year),
        (modDate->month),
        (modDate->date),
        (modDate->hour),
        (modDate->min),
        (modDate->sec));
}

int main(int argc, char const *argv[]) {
    if (argc < 2)
    {
        perror("Invalid Input");
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        perror("file doesn't exist");
        exit(1);
    }

    fseek(fp, ROOTDIRSTART_OFFSET, SEEK_SET);
    int root_start;
    fread(&root_start, SUPERBLOCK_INFO_SIZE, 1, fp);
    root_start = ntohl(root_start);

    int root_blocks;
    fread(&root_blocks, SUPERBLOCK_INFO_SIZE, 1, fp);
    root_blocks = ntohl(root_blocks);

    char status = 0;
    bool file = false;
    int file_size = 0;
    char file_name[DIRECTORY_MAX_NAME_LENGTH + 1];
    date modDate;
    char date_string[BUFFER_SIZE];

    for (int i = 0; i < DIRECTORY_ENTRY_PER_BLOCK * root_blocks; i++) {
        int entry = root_start * DEFAULT_BLOCK_SIZE + i*DIRECTORY_ENTRY_SIZE;

        fseek(fp, entry + DIRECTORY_STATUS_OFFSET, SEEK_SET);
        fread(&status, DIRECTORY_STATUS_SIZE, 1, fp);
        if (status == DIRECTORY_ENTRY_FREE) continue;
        else if (status == (DIRECTORY_ENTRY_USED | DIRECTORY_ENTRY_FILE)) file = true;

        fseek(fp, entry + DIRECTORY_FILE_SIZE_OFFSET, SEEK_SET);
        fread(&file_size, DIRECTORY_FILE_SIZE_SIZE, 1, fp);
        file_size = ntohl(file_size);

        fseek(fp, entry + DIRECTORY_FILENAME_OFFSET, SEEK_SET);
        fread(file_name, DIRECTORY_MAX_NAME_LENGTH, 1, fp);

        fseek(fp, entry + DIRECTORY_MODIFY_OFFSET, SEEK_SET);
        fread(&modDate, DIRECTORY_MODIFY_SIZE, 1, fp);
        getDate(&modDate, date_string);

        printf("%c %10d %30s %s\n", (file ? 'F':'D'), file_size, file_name, date_string);

    }

    fclose(fp);
    return 0;
}
