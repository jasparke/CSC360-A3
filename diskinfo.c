/* OUTPUT:
In part I, you will write a program that displays information about the file system. In order to complete part I,
you will need to understand the file system structure of MS-DOS, including FAT Partition Boot Sector, FAT File
Allocation Table, FAT Root Folder, FAT Folder Structure, and so on.
Your program for part I will be invoked as follows:
./diskinfo disk.IMA
Your output should include the following information:

OS Name:
Label of the disk:
Total size of the disk:
Free size of the disk:
==============
The number of files in the root directory (not including subdirectories):
=============
Number of FAT copies:
Sectors per FAT:
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"


/* Helper Functions */

void errorAndExit(char* errstring) {
    printf("%s\n", errstring);
    exit(1);
}

void findLabel(char* buffer, char* img) {
    int i;
    for (i = 0; i < 8; i++) buffer[i] = img[i+43];

    if (buffer[0] == ' ') {
        img += SECTOR_LENGTH * 19;
        while (img[0] != 0x00) {
            if (img[11] == 8) for (i = 0; i < 8; i++) buffer[i] = img[i];
            img+= 32;
        }
    }
}

int getNumRootFiles(char* img) {
    img += SECTOR_LENGTH * 19;
    int c = 0;
    while (img[0] != 0x00) {
        if ((img[11] & 0b00000010) == 0 && (img[11] & 0b00001000) == 0 && (img[11] & 0b00010000) == 0) c++;
        img+= 32;
    }
    return c;
}

void printDiskInfo(char* os, char* label, int diskSize, int freeSpace, int rootFiles, int fatCopies, int sectors) {
    printf("OS Name: %s\n", os);
    printf("Label of the disk: %s\n", label);
    printf("Total size of the disk: %d\n", diskSize);
    printf("Free size of the disk: %d\n", freeSpace);
    printf("\n==============\n");
    printf("The number of files in the root directory (not including subdirectories): %d\n", rootFiles);
    printf("\n=============\n");
    printf("Number of FAT copies: %d\n", fatCopies);
    printf("Sectors per FAT: %d\n", sectors);
}

/********
 * Main *
 ********/
int main (int argc, char* argv[]) {
    if (argc < 2) errorAndExit("Error: Invalid arguments. See README.");

    int file = open(argv[1], O_RDWR);
    if (file < 0) errorAndExit("Error: Could not read disk image.");

    struct stat buffer;
    fstat(file, &buffer);
    char* diskimg = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, file, 0);
    if (diskimg == MAP_FAILED) errorAndExit("Error: failed to map image to memory.");

    // Get OS name (pos 3, 8bytes)
    int i;
    char* os = malloc(sizeof(char));
    for (i = 0; i < 8; i++) os[i] = diskimg[i+3];

    char* label = malloc(sizeof(char));
    findLabel(label, diskimg);

    int diskSize = getDiskSize(diskimg);
    int freeSize = getFreeSpace(diskSize, diskimg);
    int rootFiles = getNumRootFiles(diskimg);
    int fatCopies = diskimg[16];
    int sectors = diskimg[22] + (diskimg[23] << 8);
    printDiskInfo(os, label, diskSize, freeSize, rootFiles, fatCopies, sectors);

    return 0;
}
