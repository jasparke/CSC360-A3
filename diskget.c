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

     int size = fileSize(argv[2], diskimg + SEC_LEN * 19);
     if (size > 0) { // file actually exists
          int destfile = open(argv[2], O_RDWR | O_CREAT, 0644);
          if (destfile < 0) {
               munmap(diskimg, buffer.st_size);
               close(realfile);
               errorAndExit("Error: Could not open or create destination file");
          }

          int status = lseek(destfile, size - 1, SEEK_SET); // go to the end of our new destfile
          if (status == -1) {
               munmap(diskimg, buffer.st_size);
               close(file);
               close(destfile);
               errorAndExit("Error: Could not move through destination file");
          }

          status = write(destfile, "", 1); // write a null character to mark end
          if (status != 1) {
               munmap(diskimg, buffer.st_size);
               close(file);
               close(destfile);
               errorAndExit("Error: Could not write to destination file");
          }

          char* fileimg = mmap(0, size, PROT_WRITE, MAP_SHARED, destfile, 0);
          if (fileimg == MAP_FAILED) errorAndExit("Error: failed to map destiantion file to memory.");

          munmap(fileimg, size);
          close(destfile);

     } else printf("File %s not found in %s.\n", argv[2], argv[1]);

     close(file);
     munmap(diskimg, buffer.st_size);
     return 0;
 }
