//
// Constants.h
//
#ifndef __CONSTANTS__H
#define __CONSTANTS__H

#define FS_IDENTIFIER			"CSC360FS"
#define DEFAULT_BLOCK_SIZE 		512

#define DIRECTORY_ENTRY_SIZE		64
#define DIRECTORY_ENTRY_PER_BLOCK	(DEFAULT_BLOCK_SIZE/DIRECTORY_ENTRY_SIZE)

#define DIRECTORY_ENTRY_FREE		0x00
#define DIRECTORY_ENTRY_USED		0x01
#define DIRECTORY_ENTRY_FILE		0x02
#define DIRECTORY_ENTRY_DIRECTORY	0x04

#define DIRECTORY_STATUS_OFFSET		0
#define DIRECTORY_START_BLOCK_OFFSET	1
#define DIRECTORY_BLOCK_COUNT_OFFSET	5
#define DIRECTORY_FILE_SIZE_OFFSET	9
#define DIRECTORY_CREATE_OFFSET		13
#define DIRECTORY_MODIFY_OFFSET		20
#define DIRECTORY_FILENAME_OFFSET	27

#define DIRECTORY_STATUS_SIZE 1
#define DIRECTORY_START_BLOCK_SIZE 4
#define DIRECTORY_FILE_SIZE_SIZE 4
#define DIRECTORY_MODIFY_SIZE 7

#define DIRECTORY_MAX_NAME_LENGTH	30

typedef struct _date {
    short year;
    char month;
    char date;
    char hour;
    char min;
    char sec;
} date;

typedef struct _directory_entry {
    char status;
    int start_block;
    int num_block;
    int file_size;
    date create_time;
    date modify_time;
    char file_name[DIRECTORY_MAX_NAME_LENGTH+1];
    char unused[6];
} directory_entry;

#define FAT_ENTRY_SIZE		4
#define FAT_ENTRY_PER_BLOCK	(DEFAULT_BLOCK_SIZE/FAT_ENTRY_SIZE)
#define	FAT_FREE		0x00000000
#define FAT_RESERVED		0x00000001
#define FAT_EOF			0xFFFFFFFF


// positions of the various entries in the superblock
#define IDENT_OFFSET		0
#define BLOCKSIZE_OFFSET	8
#define BLOCKCOUNT_OFFSET	10
#define FATSTART_OFFSET		14
#define FATBLOCKS_OFFSET	18
#define ROOTDIRSTART_OFFSET	22
#define ROOTDIRBLOCKS_OFFSET	26

#define BUFFER_SIZE 120
#define SUPERBLOCK_INFO_SIZE 4

#endif
