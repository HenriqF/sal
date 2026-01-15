#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define DEST "D:\\salvaguarda"


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

int openSalinfo(char salinfo_dir[], size_t* size, char** content){
    FILE* f = fopen(salinfo_dir, "rb");
    if (!f){
        f = fopen(salinfo_dir, "wb");
        if (!f) return -1;

        if(fputc('0', f) == EOF){
            fclose(f);
            return -1;
        }
        fclose(f);

        (*content) = malloc(2);
        if (!(*content)) {
            return -1;
        }

        (*content)[0] = '0';
        (*content)[1] = '\0';
        (*size) = 1;
    }
    else{
        if (fseek(f, 0, SEEK_END) != 0){
            fclose(f);
            return -1;
        }
            
        long pos = ftell(f);
        if (pos < 0) {
            fclose(f);
            return -1;
        }
        (*size) = (size_t)pos;

        if (fseek(f, 0, SEEK_SET) != 0){
            fclose(f);
            return -1;
        };

        (*content) = malloc((*size)+1);
        if (!(*content)){
            fclose(f);
            return -1;
        }
        size_t read = fread((*content), 1, (*size), f);
        if (read != (*size)){
            free((*content));
            fclose(f);
            return -1;
        }

        (*content)[*size] = '\0';
        fclose(f);
    }
    return 0;
}

int updateSalinfo(char salinfo_dir[], int ver){
    FILE* f = fopen(salinfo_dir, "w");
    if (!f) return -1;
        
    if (fprintf(f, "%d", ver) < 0) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

int registro(char* orig, char* dest){
    char salinfo_dir[MAX_PATH];
    sprintf(salinfo_dir, "%s\\salinfo", dest);

    size_t size = 0;
    char* content = NULL;

    if (openSalinfo(salinfo_dir, &size, &content) != 0) return -1;
    int ver = atoi(content)+1;
    if (updateSalinfo(salinfo_dir, ver) != 0) return -1;

    char newDest[MAX_PATH];
    sprintf(newDest, "%s\\%d", dest, ver);

    copy(orig, newDest);
    return 0;
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
        printf("quantia de argumentos indevida.");
        return 1;
    }

    char origin[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, origin);


    createCheckDir(DEST);
    char regDest[MAX_PATH] = "";
    sprintf(regDest, "%s\\%s", DEST, argv[1]);
    createCheckDir(regDest);



    registro(origin, regDest);
    return 0;
}
