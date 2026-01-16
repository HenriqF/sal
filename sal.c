#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "readwrite.h"
#include <openssl/sha.h>

#define DEST "D:\\testes"

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

void dirCopy(char* orig, char* dest){
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
                dirCopy(fontePath, destPath);
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



void criarDirRegistro(char* path){
    if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) {
        printf("Reg ja existe.");
        return;
    }

    if(!CreateDirectory(path, NULL)){
        printf("erro");
        return;
    }
    char contentPath[MAX_PATH];
    char buildPath[MAX_PATH];
    sprintf(contentPath, "%s\\conteudo", path);
    sprintf(buildPath, "%s\\build", path);

    if(!CreateDirectory(contentPath, NULL)){
        printf("erro");
        return;
    }
    if(!CreateDirectory(buildPath, NULL)){
        printf("erro");
        return;
    }

    sprintf(buildPath, "%s\\build\\salver", path);

    FILE* f = fopen(buildPath, "w");
    if (!f){
        printf("erro");
        return;
    }
    fprintf(f, "0");
    fclose(f);


    printf("Criado novo registro: %s", path);
    return;
}



void newBuild(char* orig, char* dest){

}

int main(int argc, char** argv){
    if (argc == 1){
        printf("Sem args.");
        return 0;
    }

    createCheckDir(DEST);

    if (argc == 2){
        char origin[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, origin);
        printf("%s", origin);
    }
    else if (argc == 3){

        if (strcmp(argv[1], "new") == 0){
            char regDest[MAX_PATH] = "";
            sprintf(regDest, "%s\\%s", DEST, argv[2]);
            criarDirRegistro(regDest);
            
            return 0;
        }

    }

    

    
        

    

    //FILE* f = fopen("porraloca.txt", "rb");
    //char hash[41];
    //getFileHash(f, hash);
    //printf("%s sigma", hash);
    //fclose(f);

    return 0;
}