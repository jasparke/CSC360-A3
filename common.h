#define SEC_LEN 512

void errorAndExit(char* err);
int getFatEntry(int entry, char* img);
int getDiskSize(char* img);
int getFreeSpace(int diskCap, char* img);
int fileExists(char* name, char* img);
int fileSize(char* name, char* img);
