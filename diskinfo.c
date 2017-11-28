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


/* Helper Functions */

void printDiskInfo(char* os, char* label, int diskSize, int freeSize, int rootFiles, int fatCopies, int sectors) {
    printf("OS Name: %s\n
            Label of the disk: %s\n
            Total size of the disk: %d\n
            Free size of the disk: %d\n
            \n
            ==============\n
            The number of files in the root directory (not including subdirectories): %d\n
            \n
            =============\n
            Number of FAT copies: %d\n
            Sectors per FAT: %d\n",
           os, label, diskSize, freeSize, rootFiles, fatCopies, sectors);
}

/********
 * Main *
 ********/
if main (int argc, char* argv[]) {
    if (argc < 2) {
        printf("Error: Invalid arguments.\n
                USAGE: diskinfo <DISK IMAGE>");
        exit(1);
    }

    printDiskInfo("hi", "label", 1,2,3,4,5);

    return 0;
}
