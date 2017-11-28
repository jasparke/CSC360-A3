/*
In part III, you will write a program that copies a file from the file system to the current directory in Linux. If the
specified file is not found in the root directory of the file system, you should output the message File not found.
and exit.
Your program for part III will be invoked as follows:
./diskget disk.IMA ANS1.PDF
If your code runs correctly, ANS1.PDF should be copied to your current Linux directory, and you should be able
to read the content of ANS1.PDF.
*/

/*
     http://www.dfists.ua.es/~gil/FAT12Description.pdf
     FAT12 byte layout on page2.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"

/********
 * Main *
 ********/
int main (int argc, char* argv[]) {
     if (argc < 3) errorAndExit("Error: Invalid arguments. See README.\n
	 							 Usage: diskget <image file> <filetoget>\n");

     int file = open(argv[1], O_RDWR);
     if (file < 0) errorAndExit("Error: Could not read disk image.");

     struct stat buffer;
     fstat(file, &buffer);
     char* diskimg = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, file, 0);
     if (diskimg == MAP_FAILED) errorAndExit("Error: failed to map image to memory.");

     /* move this to a seperate func */
     int size = fileSize(argv[2], diskimg + SEC_LEN * 19);
     while (size > 0) { // file actually exists
          int i;
          char type = ((diskimg[11] & 0b00010000) == 0b00010000) ? 'D' : 'F';

          char* name = malloc(sizeof(char));
          char* ext = malloc(sizeof(char));
          for (i = 0; i < 8; i++)
               if (diskimg[i] == ' ') break;
               else name[i] = diskimg[i];

          for (i = 0; i < 3; i++) ext[i] = diskimg[i+8];

          strcat(name, ".");
          strcat(name, ext);

          int size = fileSize(name, diskimg);

          int y = ((diskimg[17] & 0b11111110) >> 1) + 1980;
          int m = ((diskimg[16] & 0b11100000) >> 5);
          m += ((diskimg[17] & 0b00000001) << 3);
          int d = diskimg[16] & 0b00011111;
          int h = (diskimg[15] & 0b11111000) >> 3;
          int n = (diskimg[14] & 0b11100000) >> 5;
          n += (diskimg[15] & 0b00000111) << 3;

          if ((diskimg[11] & 0b00000010) == 0 && (diskimg[11] & 0b00001000) == 0)
               printf("%c %10d %20s %02d-%02d-%02d %02d:%02d\n", type, size, name, y, m, d, h, n);

          diskimg += 32;
     }

     close(file);
     munmap(diskimg, buffer.st_size);
     return 0;
 }
