#include "common.h"
#include <stdio.h>
#include <stdlib.h>

/*
    http://www.dfists.ua.es/~gil/FAT12Description.pdf
    FAT12 byte layout on page2.
 */

 void errorAndExit(char* err) {
     printf("%s\n", err);
     exit(1);
 }

int getFatEntry(int entry, char* img) {
    int val;

    int a;
    int b;

    int n = (int)(3 * entry / 2);

    /*
    If n is even, then the physical location of the entry is the low four bits in location 1+(3*n)/2
    and the 8 bits in location (3*n)/2
    If n is odd, then the physical location of the entry is the high four bits in location (3*n)/2 and
    the 8 bits in location 1+(3*n)/2
     */
    if (entry % 2 == 0) { // even
        a = img[SEC_LEN + n + 1] & 0x0F; //low four
        b = img[SEC_LEN + n] & 0xFF; // all 8
        val = (a << 8) + b;
    } else { // odd
        a = img[SEC_LEN + n] & 0xF0;
        b = img[SEC_LEN + n + 1] & 0xFF;
        val = (a >> 4) + (b << 4);
    }

    return val;
}

int getDiskSize(char* img) {
    int b = img[11] + (img[12] << 8);
    int s = img[19] + (img[20] << 8);
    return b*s;
}

/*
    returns number of free sectors*SEC_LEN
 */
int getFreeSpace(int diskSize, char* img) {
    int f = 0;
    int i = 2;

    for (; i < (diskSize/SEC_LEN); i++)
        if (getFatEntry(i, img) == 0x000) f++;

    return SEC_LEN * f;
}

int sizeOfFile(char* fname, char* img) {
    return 1;
}
