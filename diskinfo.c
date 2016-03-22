#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <netinet/in.h>
#include "Constants.h"

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

    int data;
    printf("Super block information:\n");

    short block_size;
    fseek(fp, BLOCKSIZE_OFFSET, SEEK_SET);
    fread(&block_size, SUPERBLOCK_INFO_SIZE * 0.5, 1, fp);
    printf("Block size: %d\n", block_size = ntohs(block_size));

    int block_count;
    fread(&block_count, SUPERBLOCK_INFO_SIZE, 1, fp);
    printf("Block count: %d\n", block_count = ntohl(block_count));

    int fat_start;
    fread(&fat_start, SUPERBLOCK_INFO_SIZE, 1, fp);
    printf("FAT starts: %d\n", fat_start = ntohl(fat_start));

    int fat_blocks;
    fread(&fat_blocks, SUPERBLOCK_INFO_SIZE, 1, fp);
    printf("FAT blocks: %d\n", fat_blocks = ntohl(fat_blocks));

    fread(&data, SUPERBLOCK_INFO_SIZE, 1, fp);
    printf("Root directory start: %d\n", ntohl(data));

    fread(&data, SUPERBLOCK_INFO_SIZE, 1, fp);
    printf("Root directory blocks: %d\n", ntohl(data));

    printf("\n");

    printf("FAT information:\n");

    int entry;
    int free_blocks = 0;
    int reserved_blocks = 0;
    int alloc_blocks = 0;

    fseek(fp, fat_start * block_size, SEEK_SET);
    for (int i = 0; i < block_count; i++) {
        fread(&entry, FAT_ENTRY_SIZE, 1, fp);
        entry = ntohl(entry);
        if (entry == FAT_FREE) free_blocks++;
        else if (entry == FAT_RESERVED) reserved_blocks++;
        else alloc_blocks++;
    }

    printf("Free Blocks: %d\n", free_blocks);
    printf("Reserved Blocks: %d\n", reserved_blocks);
    printf("Allocated Blocks: %d\n", alloc_blocks);

    fclose (fp);
    return 0;
}
