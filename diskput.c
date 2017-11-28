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
 #include "common.h"


 void copyFromMem(char* dimg, char* fimg, int size, char* name) {
      int start = fileStartSector(name, dimg + SEC_LEN*19);
      int n = start;
      int rem = size;
      int addr; //ignore first sectors

      do {
           n = (rem == size) ? n : FATLookup(n, dimg);
           addr = SEC_LEN * (31 + n);

           int i;
           for (i = 0; i < SEC_LEN; i++) {
                if (rem == 0) break;
                fimg[size - rem] = dimg[i + addr];
                rem --;
           }
      } while (FATLookup(n, dimg) != 0xFFF);

 }

 /********
  * Main *
  ********/
 int main (int argc, char* argv[]) {
      if (argc < 3) errorAndExit("Error: Invalid arguments. See README.\n Usage: diskget <image file> <filetoget>\n");

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
                close(destfile);
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

           copyFromMem(diskimg, fileimg, size, argv[2]);

           munmap(fileimg, size);
           close(destfile);

      } else printf("File not found.\n");

      close(file);
      munmap(diskimg, buffer.st_size);
      return 0;
  }
