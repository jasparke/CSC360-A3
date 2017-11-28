#define SECTOR_LENGTH 512

int getFatEntry(int entry, char* map);
int getDiskSize(char* map);
int getFreeSpace(int diskCap, char* map);
int sizeOfFile(char* fname, char* map);
