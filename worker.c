#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

void close_files(FILE* status, FILE* task, FILE* result);
void open_files(FILE** status, FILE** task, FILE** result);

int action(int num1, int num2, char c);

int main() {

   FILE* status, *task, *result;
   open_files(&status, &task, &result);

    char ready[6];
    ready[5] = '\0';
    while(strcmp(ready, "READY")) {
        fseek(status, 0, SEEK_SET);
        fgets(ready, 6, status);
    }
    int num1, num2, res;
    char c;
    while (fscanf(task," %d %c %d", &num1, &c, &num2) != EOF) {
        res = action(num1, num2, c);
        fprintf(result, "%d\n", res);
    }
    fseek(status, 0, SEEK_SET);
    truncate("./status.txt", 4);
    fputs("DONE", status);
    fflush(status);
    usleep(1000);
    close_files(status, task, result);
    exit(EXIT_SUCCESS);
}

int action(int num1, int num2, char c) {;
        switch (c)
        {
            case '+':
                    return num1 + num2;
            case '-':
                    return num1 - num2;
            case '/':
                    return num1 / num2;
            case '*':
                    return num1 * num2;
        }
}

void open_files(FILE** status, FILE** task, FILE** result) {
    *status = fopen("status.txt", "w+");
    if (*status == NULL) {
        perror("Open");
        exit(EXIT_FAILURE);
    }
    *result = fopen("result.txt", "w+");
    if (result == NULL) {
        perror("Open");
        fclose(*status);
        exit(EXIT_FAILURE);
    }
    *task = fopen("task.txt", "w+");
    if (*task == NULL) {
        perror("Open");
        fclose(*result);
        fclose(*status);
        exit(EXIT_FAILURE);
    }
}

void close_files(FILE* status, FILE* task, FILE* result) {
    if (fclose(result) == -1) {
        perror("Close");
        exit(EXIT_FAILURE);
    }
    if (fclose(task) == -1) {
        perror("Close");
        exit(EXIT_FAILURE);
    }
    if (fclose(status) == -1) {
        perror("Close");
        exit(EXIT_FAILURE);
    }

}