/* OUTPUT:
1. The first column will contain:
	(a) F for regular files, or
	(b) D for directories;
    followed by a single space
2. then 10 characters to show the file size in bytes, followed by a single space
3. then 20 characters for the file name, followed by a single space
4. then the file creation date and creation time.
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
    if (argc < 2) errorAndExit("Error: Invalid arguments. See README.");

    int file = open(argv[1], O_RDWR);
    if (file < 0) errorAndExit("Error: Could not read disk image.");

    struct stat buffer;
    fstat(file, &buffer);
    char* diskimg = mmap(0, buffer.st_size, PROT_READ, MAP_SHARED, file, 0);
    if (diskimg == MAP_FAILED) errorAndExit("Error: failed to map image to memory.");

    /* move this to a seperate func */
    while (diskimg[0] != 0x00) {
        int i;
        char type = (diskimg[11] & 0b00010000) == 0b00010000) ? 'D' : 'F';

        char* name = malloc(sizeof(char));
        char* ext = malloc(sizeof(char));
        for (i = 0; i < 3; i++) ext[i] = diskimg[i+8];
        for (i = 0; i < 8; i++)
            if (diskimg[i] == ' ') break;
            else name[i] = diskimg[i];

        int size = getFileSize(name, ext, diskimg);

        int y = (diskimg[17]);
        int m = 0;
        int d = 0;
        int h = 0;
        int min = 0;

        if (diskimg[11] & 0b00000010 == 0 && diskimg[11] & 0b00001000 == 0)
            printf("%c %d %s.%s %d-%d-%d %02d:%02d\n", type, size, name, ext, y, m, d, h, min);

    }

    close(file);
    munmap(diskimg, buffer.st_size);
    return 0;
}
