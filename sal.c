#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "readwrite.h"
#include <openssl/sha.h>

#define DEST "D:\\salvaguarda"


int getFileHash(FILE* f, char hash[41]){
    size_t size;
    char* content;
    readFile(f, &size, &content);

    unsigned char out[SHA_DIGEST_LENGTH]; 
    SHA1((unsigned char*)content, strlen(content), out);

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(hash+i*2, "%02x", out[i]);
    }

    return 0;
}

void copy(char* orig, char* dest){
    WIN32_FIND_DATA fd;
    char fontePath[MAX_PATH];
    char destPath[MAX_PATH];
    CreateDirectory(dest, NULL);

    sprintf(fontePath, "%s\\*", orig);
    HANDLE hFind = FindFirstFile(fontePath, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..")){
            sprintf(fontePath, "%s\\%s", orig, fd.cFileName);
            sprintf(destPath, "%s\\%s", dest, fd.cFileName);

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                copy(fontePath, destPath);
            } 
            else{
                CopyFile(fontePath, destPath, FALSE);
            }
        }
    } while(FindNextFile(hFind, &fd));

    FindClose(hFind);
}

int createCheckDir(char* dest){
    if (!(CreateDirectory(dest, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)){
        printf("falho");
        return -1;
    } 
    return 0;
}

int main(int argc, char** argv){
    if (argc < 2 || argc > 2){
        printf("Coloca o nome da pasta do registro.");
        return 1;
    }

    char origin[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, origin);

    char regDest[MAX_PATH] = "";
    sprintf(regDest, "%s\\%s", DEST, argv[1]);

    createCheckDir(DEST);
    createCheckDir(regDest);




    FILE* f = fopen("porraloca.txt", "rb");
    char hash[41];
    getFileHash(f, hash);
    printf("%s sigma", hash);
    fclose(f);

    return 0;
}