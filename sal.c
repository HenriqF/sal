#include "util.h"

char DEST[MAX_PATH];
char ARG_REG[MAX_PATH];
#define VER "10.03.2026.1"

int ignore_exes = 1;
int copy_messages = 0;

#define M_NEW 0x01 //
#define M_LOAD 0x02 //carregar porraloca
#define M_EXE 0x04 //incluir .exes ao salvar
#define M_SPC 0x08 //criar build especial
#define M_VIEW 0x10 //visualizar builds do regis
#define M_CPYMSG 0x20 // mostrar mensagems de copiar

//dirs
int createCheckDir(char* dest){
    if (!(CreateDirectory(dest, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)){
        return -1;
    } 
    return 0;
}

void criarDirRegistro(char* path){
    if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) {
        printf("Registro ja existe.");
        return;
    }

    if(!CreateDirectory(path, NULL)){
        printf("erro");
        return;
    }
    char contentPath[MAX_PATH];
    char buildPath[MAX_PATH];
    snprintf(contentPath, MAX_PATH, "%s\\conteudo", path);
    snprintf(buildPath, MAX_PATH, "%s\\build", path);

    if(!CreateDirectory(contentPath, NULL)){
        printf("erro");
        return;
    }
    if(!CreateDirectory(buildPath, NULL)){
        printf("erro");
        return;
    }

    snprintf(buildPath, MAX_PATH,"%s\\build\\salver", path);

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


//builds
int getFileHash(FILE* f, char hash[41]){
    size_t size;
    char* content;
    readFile(f, &size, &content);

    unsigned char out[SHA_DIGEST_LENGTH]; 
    SHA1((unsigned char*)content, size, out);

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(hash+i*2, "%02x", out[i]);
    }
    free(content);
    return 0;
}

void hashCLBuild(char* orig, char* dest, char* conteudo, int copy){
    WIN32_FIND_DATA fd;
    char fonte_path[MAX_PATH];
    char dest_path[MAX_PATH];
    CreateDirectory(dest, NULL);

    snprintf(fonte_path, MAX_PATH, "%s\\*", orig);
    HANDLE hFind = FindFirstFile(fonte_path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..")){
            snprintf(fonte_path, MAX_PATH, "%s\\%s", orig, fd.cFileName);
            snprintf(dest_path, MAX_PATH, "%s\\%s", dest, fd.cFileName);

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                hashCLBuild(fonte_path, dest_path, conteudo, copy);
            }
            else if (copy){
                if (!(ignore_exes && endsWith(fd.cFileName, ".exe"))){
                    FILE* f = fopen(fonte_path, "rb");
                    char hash[41];
                    getFileHash(f, hash);
                    fclose(f);

                    char file_path[MAX_PATH];
                    snprintf(file_path, MAX_PATH, "%s\\%s",conteudo, hash);

                    DWORD at = GetFileAttributesA(file_path);
                    if(!(at != INVALID_FILE_ATTRIBUTES && !(at & FILE_ATTRIBUTE_DIRECTORY))){
                        CopyFile(fonte_path, file_path, FALSE);
                        if (copy_messages) printf("Copiado: %s -> %s\n", fonte_path, file_path);
                        else printf(".");
                    }

                    f = fopen(dest_path, "wb");
                    writeFile(f, hash);
                    fclose(f);
                }
                else{
                    if (copy_messages) printf(RED "Ignorado arquivo: " RESET "%s\n", fonte_path);
                }
            }
            else{
                size_t size = 50;
                char content_name[size];
                
                FILE* f = fopen(fonte_path, "rb");
                noMallocReadFile(f, size, content_name);
                fclose(f);

                char file_content_path[MAX_PATH];
                snprintf(file_content_path, MAX_PATH, "%s\\%s", conteudo, content_name);

                CopyFile(file_content_path, dest_path, FALSE);
            }

        }
    } while(FindNextFile(hFind, &fd));

    FindClose(hFind);
}

int newBuild(char* orig, char* dest){
    char* last_slash_orig = strrchr(orig, '\\');
    char orig_folder_name[MAX_PATH];
    snprintf(orig_folder_name, MAX_PATH, last_slash_orig+1);

    if (strcmp(orig_folder_name, ARG_REG) != 0){
        char resposta;
        printf(RED "/!\\" RESET " Mandar '" RED "%s" RESET "' para '" RED "%s" RESET"'? (s/n): ", orig_folder_name, ARG_REG);
        scanf("%c", &resposta);
        if (resposta != 's' && resposta != 'S') return -1;
    }



    char buildPath[MAX_PATH];
    char conteudoPath[MAX_PATH];
    char salverPath[MAX_PATH];
    char newVerPath[MAX_PATH];
    snprintf(buildPath, MAX_PATH, "%s\\build", dest);
    snprintf(conteudoPath, MAX_PATH, "%s\\conteudo", dest);
    snprintf(salverPath, MAX_PATH, "%s\\salver", buildPath);

    char* content;
    size_t sz = 0;

    FILE* salf = fopen(salverPath, "rb");
    readFile(salf, &sz, &content);
    fclose(salf);
    int current_ver = atoi(content);

    snprintf(newVerPath, MAX_PATH, "%s\\%d", buildPath, current_ver+1);
    if(createCheckDir(newVerPath) != 0){
        printf("Registro não existe.");
        return -1;
    }

    printf("Nova build: %s\n",newVerPath);

    char new_ver[30];
    snprintf(new_ver, (size_t)30, "%d", current_ver+1);

    salf = fopen(salverPath, "wb");
    writeFile(salf, new_ver);
    fclose(salf);

    hashCLBuild(orig, newVerPath, conteudoPath, 1);
    
    return 0;
}

int newSpecialBuild(char* orig, char* dest, char* nome){
    char buildPath[MAX_PATH];
    char conteudoPath[MAX_PATH];
    char newVerPath[MAX_PATH];

    snprintf(buildPath, MAX_PATH, "%s\\build", dest);
    snprintf(conteudoPath, MAX_PATH, "%s\\conteudo", dest);
    snprintf(newVerPath, MAX_PATH, "%s\\%s", buildPath, nome);

    if (GetFileAttributesA(newVerPath) != INVALID_FILE_ATTRIBUTES) {
        printf("Build ja existe.");
        return -1;
    }

    if(createCheckDir(newVerPath) != 0){
        printf("Registro não existe.");
        return -1;
    }
    
    printf("Nova build: %s\n",newVerPath);
    hashCLBuild(orig, newVerPath, conteudoPath, 1);
    
    return 0;
}

int loadBuild(char* orig, char* dest, char* ver){
    char salver_path[MAX_PATH];
    snprintf(salver_path, MAX_PATH, "%s\\build\\salver", orig);

    char build_path[MAX_PATH];
    if (strlen(ver) != 0){
        snprintf(build_path, MAX_PATH, "%s\\build\\%s", orig, ver);
    }
    else{
        char* content;
        size_t size = 0;
        FILE* f = fopen(salver_path, "rb");
        readFile(f, &size, &content);
        fclose(f);
        snprintf(build_path, MAX_PATH, "%s\\build\\%s", orig, content);
        free(content);
    }

    char conteudo_path[MAX_PATH];
    snprintf(conteudo_path, MAX_PATH, "%s\\conteudo", orig);

    if (GetFileAttributesA(build_path) != INVALID_FILE_ATTRIBUTES) {
        hashCLBuild(build_path, dest, conteudo_path, 0);
        return 0;
    }
    printf("Build nao existe.");
    return -1;
}


//visualizacao

void listRegistros(){
    WIN32_FIND_DATA fd;

    char dirReg[MAX_PATH];
    snprintf(dirReg, MAX_PATH, "%s\\*", DEST);

    HANDLE hFind = FindFirstFile(dirReg, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..")){
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                printf("Registro "BLUE "%s" RESET " -> ", fd.cFileName);

                char dirSalver[MAX_PATH];
                snprintf(dirSalver, MAX_PATH, "%s\\%s\\build\\salver", DEST, fd.cFileName);

                char* content;
                size_t size = 0;
                FILE* f = fopen(dirSalver, "rb");
                readFile(f, &size, &content);
                fclose(f);

                printf("versão %s\n", content);

            } 
        }
    } while(FindNextFile(hFind, &fd));

    FindClose(hFind);
}

//Prog
void init(){
    SetConsoleOutputCP(CP_UTF8);

    char program_path[MAX_PATH]; 
    GetModuleFileNameA(NULL, program_path, MAX_PATH);
    *strrchr(program_path, '\\') = '\0';

    char config_path[MAX_PATH];
    snprintf(config_path, MAX_PATH, "%s\\svconfig.txt", program_path);


    char** svconfig_lines;
    size_t line_count;

    FILE* f = fopen(config_path, "rb");
    if (!f) msgExit("Sem arquivo svconfig...");

    getFileLines(f, &svconfig_lines, &line_count);
    fclose(f);
    if (line_count > 0){
        strcpy(DEST, svconfig_lines[0]);
    }

    if (createCheckDir(DEST) != 0) msgExit("Falha em criar / achar diretorio destino (primeira linha svconfig)");
}

int main(int argc, char** argv){
    char proj_path[MAX_PATH];
    char reg_path[MAX_PATH];

    init();
    GetCurrentDirectoryA(MAX_PATH, proj_path);
    snprintf(reg_path, MAX_PATH, "%s\\%s", DEST, argv[argc-1]);
    snprintf(ARG_REG, MAX_PATH, "%s", argv[argc-1]);

    if (argc == 1){
        printf(BLUE "salvaguarda " RESET "versão " BLUE "%s\n\n" RESET, VER);
        listRegistros();
        printf("\n");
        return 0;
    }    
    else if (argc == 2){
        newBuild(proj_path, reg_path);
        return 0;
    }

    int flags = 0;
    int special_flag_index = 0;
    int load_flag_index = 0;
    
    for (int i = 1; i < argc-1; i++){
        //muda algo
        if ((startsWith(argv[i], "-from") == 1)){
            char new_path[MAX_PATH];
            snprintf(new_path, MAX_PATH, "%s", argv[i]+5);
            DWORD fa = GetFileAttributes(new_path);
            if (fa == INVALID_FILE_ATTRIBUTES || ((fa & FILE_ATTRIBUTE_DIRECTORY) == 0)){
                printf("caminho indevido / inexistente.");
                return 0;
            }
            snprintf(proj_path, MAX_PATH, "%s", new_path);
        }
        
        //termina programa
        else if ((startsWith(argv[i], "-new") == 1)){
            criarDirRegistro(reg_path);
            return 0;
        }
        else if ((startsWith(argv[i], "-view") == 1)){
            char build_path[MAX_PATH];
    
            if (argv[i][5] != '\0'){
                snprintf(build_path, MAX_PATH, "%s\\build\\%s", reg_path, argv[i]+5);
                printf("\n");
                fileTravel(build_path, 1, 1);
                return 0;
            }

            snprintf(build_path, MAX_PATH, "%s\\build", reg_path, argv[i]+5);
            fileTravel(build_path, 0, 0);
            return 0;
        }
        
        
        //levanta flag
        else if ((startsWith(argv[i], "-load") == 1)){
            load_flag_index = i;
            flags |= M_LOAD;
        } 
        else if ((startsWith(argv[i], "-S") == 1)){
            special_flag_index = i;
            flags |= M_SPC;
        }
        else if ((startsWith(argv[i], "-exe") == 1)) flags |= M_EXE;
        else if ((startsWith(argv[i], "-msg") == 1)) flags |= M_CPYMSG;
    }

    //flags exclusivas
    if ((flags & M_LOAD) == M_LOAD){
        char ver_string[50];
        snprintf(ver_string, (size_t)50, "%s", argv[load_flag_index]+5);

        loadBuild(reg_path, proj_path, ver_string);
        return 0;
    }


    //nao exclusivas
    if ((flags & M_EXE) == M_EXE){
        ignore_exes = 0;
    }
    if ((flags & M_CPYMSG) == M_CPYMSG){
        copy_messages = 1;
    }
    if ((flags & M_SPC) == M_SPC){
        char nome_build[50];
        snprintf(nome_build, (size_t)50, "%s", argv[special_flag_index]+2);
        newSpecialBuild(proj_path, reg_path, nome_build);
        return 0;
    }
    newBuild(proj_path, reg_path);

    return 0;
}