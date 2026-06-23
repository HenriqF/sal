#include "diff.h"

void diff(char** a, size_t la, char** b, size_t lb, int mode){
    int** sigma = malloc((la+1)*sizeof(int*));
    for (size_t i = 0 ; i < la+1; i++){
        sigma[i] = calloc(lb+1, sizeof(int));
    }

    for (size_t i = 1 ; i < la+1; i++){
        for (size_t j = 1; j < lb+1; j++){
            if (strcmp(a[i-1], b[j-1]) == 0 ){
                sigma[i][j] = sigma[i - 1][j - 1] + 1;
            }
            else{
                int a = sigma[i - 1][j];
                int b = sigma[i][j-1];
                if (a >= b) sigma[i][j] = a;
                else sigma[i][j] = b;
            }
        }
    }

    size_t bl = la+lb;
    char** subsq = malloc(bl * sizeof(char*));
    int* map_type = malloc(bl * sizeof(int));

    size_t start = bl-1;
    size_t subsq_i = start;

    int i = la;
    int j = lb;
    while (i > 0 || j > 0){
        if (i > 0 && j > 0 && (strcmp(a[i-1], b[j-1]) == 0)){
            map_type[subsq_i] = 0;
            subsq[subsq_i--] = a[i-1];

            j--;
            i--;
        }
        else if (i > 0 && (j == 0 || sigma[i-1][j] > sigma[i][j-1])){
            map_type[subsq_i] = -1;
            subsq[subsq_i--] = a[i-1];

            i--;
        }
        else if (j > 0 && (i == 0 || sigma[i][j-1] >= sigma[i-1][j])){
            map_type[subsq_i] = 1;
            subsq[subsq_i--] = b[j-1];

            j--;
        }

    }

    subsq_i++;
    for (; subsq_i <= start; subsq_i++){
        if (map_type[subsq_i] == 1) {
            if ((mode & MODE_ADD) == MODE_ADD) printf(COR_OKL " + %s\n" RESET, subsq[subsq_i]);
        }
        else if ( map_type[subsq_i] == - 1){
            if ((mode & MODE_RMV) == MODE_RMV) printf(COR_NOKL " - %s\n" RESET, subsq[subsq_i]);
        }
        else {
            printf("   %s\n", subsq[subsq_i]);
        }

    }

    free(map_type);
    free(subsq);
    for (size_t i = 0 ; i < la+1; i++)free(sigma[i]);
    free(sigma);
}

char** get_lines(char* texto, size_t l, size_t* lines_c){
    size_t lines = 1;
    for (size_t i = 0; i < l; i++){
        if (texto[i] == '\n') lines++;
    }
    (*lines_c) = lines;

    size_t pos = 0;
    char** ret = malloc(lines*sizeof(size_t));

    size_t start = 0;
    int car = 0;

    size_t i = 0;
    for (; i < l; i++){
        if (texto[i] == '\r') car++;
        if (texto[i] == '\n'){
            size_t line_length = i-start-car;
            char* line = malloc((line_length+1)*sizeof(char));
            snprintf(line, line_length+1, "%s", texto+start);
            ret[pos++] = line;

            start = i+1;
            car = 0;
        }
    }
    size_t line_length = i-start-car;
    char* line = malloc((line_length+1)*sizeof(char));
    snprintf(line, line_length+1, "%s", texto+start);
    ret[pos++] = line;

    return ret;
}

void clear_lines(char** lh, size_t l){
    for (size_t i = 0; i < l; i++) free(lh[i]);
    free(lh);
}

void show_diff(char* fa, char* fb, int mode){
    FILE* f_a = fopen(fa, "rb");
    char* a_texto;
    size_t a_len = 0;

    FILE* f_b = fopen(fb, "rb");
    char* b_texto;
    size_t b_len = 0;

    readFile(f_a, &a_len, &a_texto);
    readFile(f_b, &b_len, &b_texto);

    fclose(f_a);
    fclose(f_b);

    size_t qtd_linhas_a;
    char** la = get_lines(a_texto, a_len, &qtd_linhas_a);
    //for (size_t i = 0 ; i < qtd_linhas_a; i++) printf("%s\n", la[i]);
    
    size_t qtd_linhas_b;
    char** lb = get_lines(b_texto, b_len, & qtd_linhas_b);
    //for (size_t i = 0 ; i < qtd_linhas_b; i++) printf("%s\n", lb[i]);
    //return;

    diff(la, qtd_linhas_a, lb, qtd_linhas_b, mode);

    clear_lines(la, qtd_linhas_a);
    clear_lines(lb, qtd_linhas_b);
}