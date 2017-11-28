#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    http://www.dfists.ua.es/~gil/FAT12Description.pdf
    FAT12 byte layout on page2.
 */

 void errorAndExit(char* err) {
     printf("%s\n", err);
     exit(1);
 }

/*
    set an entry to val in the fat table
 */
void FATSet(int entry, int val, char* img) {
    img += SEC_LEN;

    int n = (int)(3 * entry / 2);

    if (entry % 2 == 0) { // even
        img[SEC_LEN + n + 1] = (val >> 8) & 0x0F; //low four
        img[SEC_LEN + n] = val & 0xFF; // all 8
    } else { // odd
        img[SEC_LEN + n] = (val << 4) & 0xF0;
        img[SEC_LEN + n + 1] = (val >> 4) & 0xFF;
    }
}

/*
    Lookup an entry in the fat table.
 */
int FATLookup(int entry, char* img) {
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

/*
    Get the next free pos in the fat table
*/
int FATGetFree(char* img) {
    img += SEC_LEN;

    int n = 2;
    while (FATLookup(n, img) != 0x000) n++;
    return n;
}

/*
    return size of disk from boot sector
 */
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
        if (FATLookup(i, img) == 0x000) f++;

    return SEC_LEN * f;
}

//COULD: probably merge these 3 functions as most of the code is the same... use flag?

/*
    find and return the size of a file name if it is found in a map img.
    0 is returned if file not found
 */
int fileSize(char* name, char* img) {
    while (img[0] != 0x00) {
        if ((img[11] & 0b00000010) == 0b00000000 && (img[11] & 0b00001000) == 0) {
            int i;

            char* cname = malloc(sizeof(char));
            char* cext = malloc(sizeof(char));
            for (i = 0; i < 8; i++) //read current file name
                if (img[i] == ' ') break;
                else cname[i] = img[i];

            for (i = 0; i < 3; i++) cext[i] = img[i+8]; //read current file extension

            strcat(cname, ".");
            strcat(cname, cext);
            if (strcmp(name, cname) == 0) {
                int size = img[28] & 0xFF;
                size += (img[29] & 0xFF) << 8;
                size += (img[30] & 0xFF << 16);
                size += (img[31] & 0xFF) << 24;
                return size;
            }

        }
        img += 32;
    }
    return 0;
}

/*
    search through entries to find a file name. if it exists, return 1, else 0.
 */
int fileExists(char* name, char* img) {
    while (img[0] != 0x00) {
        if ((img[11] & 0b00000010) == 0b00000000 && (img[11] & 0b00001000) == 0) {
            int i;

            char* cname = malloc(sizeof(char));
            char* cext = malloc(sizeof(char));
            for (i = 0; i < 8; i++)
                if (img[i] == ' ') break;
                else cname[i] = img[i];

            for (i = 0; i < 3; i++) cext[i] = img[i+8];

            strcat(cname, ".");
            strcat(cname, cext);
            if (strcmp(name, cname) == 0) return 1;

        }
        img += 32;
    }

    return 0;
}

/*
    Return the first sector of the file, -1 if not found
 */
int fileStartSector(char* name, char* img) {
    while (img[0] != 0x00) {
        if ((img[11] & 0b00000010) == 0b00000000 && (img[11] & 0b00001000) == 0) {
            int i;

            char* cname = malloc(sizeof(char));
            char* cext = malloc(sizeof(char));
            for (i = 0; i < 8; i++) //read current file name
                if (img[i] == ' ') break;
                else cname[i] = img[i];

            for (i = 0; i < 3; i++) cext[i] = img[i+8]; //read current file extension

            strcat(cname, ".");
            strcat(cname, cext);
            if (strcmp(name, cname) == 0) return img[26] + (img[27] << 8);

        }
        img += 32;
    }
    return -1;
}
