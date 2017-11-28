/*
You will write a program that copies a file from the current Linux directory into the root directory of the file system.
If the specified file is not found, you should output the message File not found. on a single line and exit. If the
file system does not have enough free space to store the file, you should output the message No enough free space
in the disk image. and exit.
Your program will be invoked as follows:
./diskput disk.IMA foo.txt
Note that a correct execution should update FAT and related allocation information in disk.IMA accordingly.
To validate, you can use diskget implemented in Part III to check if you can correctly read foo.txt from the file
system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"


void updateRoot(char* dimg, int n, int size, char* name) {
     // loop until we find a free root directory
     dimg += SEC_LEN * 19;
     while (dimg[0] != 0x00) dimg += 32;

     int i, j = -1;
     for (i = 0; i < 8; i++) {
          if (name[i] == '.') j = i; // write till we hit the extension
          dimg[i] = (j == -1) ? name[i] : ' ';
     }

     for (i = 0; i < 3; i++) dimg[i+8] = name[i+j+1];

     dimg[11] = 0x00; // attributes go to 0;

     //time shifts
     time_t t = time(NULL);
     struct tm *l = localtime(&t);
     for (i = 14; i < 18; i++) dimg[i] = 0;
     dimg[17] |= (l->tm_year) << 1;
     dimg[17] |= (l->tm_mon - ((dimg[16] & 0b11100000) >> 5)) >> 3;
     dimg[16] |= (l->tm_mon - ((dimg[17] & 0b00000001) << 3)) << 5;
     dimg[16] |= l->tm_mday & 0b00011111;
     dimg[15] |= (l->tm_hour << 3) & 0b11111000;
     dimg[15] |= (l->tm_min - ((dimg[14] & 0b11100000) >> 5)) >> 3;
     dimg[14] |= (l->tm_min - ((dimg[15] & 0b00000111) << 3)) << 5;

     //set start point;
     dimg[26] = (n - (dimg[27] << 8)) & 0xFF;
     dimg[27] = (n - dimg[26]) >> 8;

     //save the file size
     dimg[28] = size & 0x000000FF;
     dimg[29] = (size & 0x0000FF00) >> 8;
     dimg[30] = (size & 0x00FF0000) >> 16;
     dimg[31] = (size & 0xFF000000) >> 24;
}

void copyToMem(char* dimg, char* fimg, int size, char* name) {
     if (!fileExists(name, dimg + SEC_LEN * 19)) {
          int rem = size;
          int n = FATGetFree(dimg);

          updateRoot(dimg, n, size, name);

          while (rem > 0) {
               int addr = SEC_LEN * (31 + n);

               int i;
               for (i = 0; i < SEC_LEN; i++) {
                    if (rem == 0) {
                         FATSet(n, 0xFFF, dimg);
                         return;
                    }
                    dimg[i + addr] = fimg[size-rem];
                    rem--;
               }
               FATSet(n, 0x69, dimg);
               i = FATGetFree(dimg);
               FATSet(n, i, dimg);
               n = i;
          }
     }
}

/********
 * MAIN *
 ********/
int main (int argc, char* argv[]) {
     if (argc < 3) errorAndExit("Error: Invalid arguments. See README.\n Usage: diskget <image file> <filetoget>\n");

     int file = open(argv[1], O_RDWR);
     if (file < 0) errorAndExit("Error: Could not read disk image.");

     struct stat buffer;
     fstat(file, &buffer);
     char* diskimg = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, file, 0);
     if (diskimg == MAP_FAILED) errorAndExit("Error: failed to map image to memory.");

     int srcfile = open(argv[2], O_RDWR);
     if (srcfile < 0) {
         munmap(diskimg, buffer.st_size);
         close(srcfile);
         errorAndExit("File not found.");
     }

     struct stat fbuffer;
     fstat(srcfile, &fbuffer);
     char* srcimg = mmap(0, fbuffer.st_size, PROT_READ, MAP_SHARED, srcfile, 0);
     if (srcimg == MAP_FAILED) errorAndExit("Error: failed to map file to memory.");

     if (getFreeSpace(getDiskSize(diskimg), diskimg) >= fbuffer.st_size) copyToMem(srcimg, diskimg, fbuffer.st_size, argv[2]);
     else printf("Not enough free space in the disk image.\n");

     close(file);
     close(srcfile);
     munmap(diskimg, buffer.st_size);
     munmap(srcimg, fbuffer.st_size);
     return 0;
}
