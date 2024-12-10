#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

#define FILE_NAME_LEN 30
#define LINE_LEN 256

typedef struct {
    FILE* file1;
    FILE* file2;
} Files;

int fFor2 = 1;
int fFor3 = 1;
int fFor4 = 1;

void xor(char* str);
void atbash(char* str);
void* encode1(void* arg);
void* encode2(void* arg);
void* decode1(void* arg);
void* decode2(void* arg);

int main(int argc, char** argv) 
{
    if (argc < 2) {
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    }

    Files en1, en2, de1, de2;
    pthread_t thr1, thr2, thr3, thr4;

    en1.file1 = fopen(argv[1], "r");
    en1.file2 = fopen("encode1.txt", "w");
    en2.file1 = fopen("encode1.txt", "r");
    en2.file2 = fopen("encode2.txt", "w");
    de1.file1 = fopen("encode2.txt", "r");
    de1.file2 = fopen("decode1.txt", "w");
    de2.file1 = fopen("decode1.txt", "r");
    de2.file2 = fopen("newfile.txt", "w");

    if (pthread_create(&thr1, NULL, encode1, &en1)) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    } 
    if (pthread_create(&thr2, NULL, encode2, &en2)) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    } 
    if (pthread_create(&thr3, NULL, decode1, &de1)) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    } 
    if (pthread_create(&thr4, NULL, decode2, &de2)) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    } 
    
    if (pthread_join(thr1, NULL)) {
        perror("Join");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thr2, NULL)) {
        perror("Join");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thr3, NULL)) {
        perror("Join");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thr4, NULL)) {
        perror("Join");
        exit(EXIT_FAILURE);
    }

    fclose(en1.file1);
    fclose(en1.file2);
    fclose(en2.file1);
    fclose(en2.file2);
    fclose(de1.file1);
    fclose(de1.file2);
    fclose(de2.file1);
    fclose(de2.file2);
}

void xor(char* str) {
    char key = 'Z';
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] != '\n') {
            str[i] ^= key;
        }
    } 
}

void atbash(char* str) {
        for (int i = 0; str[i] != '\0'; i++) {
        if (isupper(str[i])) {
            str[i] = 'Z' - (str[i] - 'A');
        } else if (islower(str[i])) {
            str[i] = 'z' - (str[i] - 'a');
        }
    }
}

void* encode1(void* arg) {
    Files* files = (Files*) arg;
    char* buff = malloc(sizeof(char) * LINE_LEN);
    if (buff == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    while (fgets(buff, LINE_LEN, files->file1)) {
        xor(buff);
        fputs(buff, files->file2);
        fflush(files->file2);
    }
    fFor2 = 0;
    return NULL;
}

void* encode2(void* arg) {
    Files* files = (Files*) arg;
    char* buff = malloc(sizeof(char) * LINE_LEN);
    if (buff == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }
    while (fFor2 || fgets(buff, LINE_LEN, files->file1)) {
        atbash(buff);
        fputs(buff, files->file2);
        fflush(files->file2);
    } 
    fFor3 = 0;
    return NULL;
}

void* decode1(void* arg) {
    Files* files = (Files*) arg;
    char* buff = malloc(sizeof(char) * LINE_LEN);
    if (buff == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    while (fFor3 || fgets(buff, LINE_LEN, files->file1)) {
        atbash(buff);
        fputs(buff, files->file2);
        fflush(files->file2);
    } 
    fFor4 = 0;
    return NULL;
}

void* decode2(void* arg) {
    Files* files = (Files*) arg;
    char* buff = malloc(sizeof(char) * LINE_LEN);
    if (buff == NULL) {
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    while (fFor4 || fgets(buff, LINE_LEN, files->file1)) {
        xor(buff);
        fputs(buff, files->file2);
        fflush(files->file2);
    }
    return NULL;
}

