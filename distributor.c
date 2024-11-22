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

void get_operations(FILE* task);
void print_result(FILE* task, FILE* result);

int main() {
   FILE* status, *task, *result;
   open_files(&status, &task, &result);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { //Exec to worker
        execl("./worker", "./worker", NULL);
    } else { //Parent
        get_operations(task);
        fseek(status, 0, SEEK_SET);
        fputs("READY", status);
        fflush(status);
        int st;
        wait(&st);
        if (WIFEXITED(st)) {
            char done[5];
            fseek(status, 0, SEEK_SET);
            fgets(done, 5, status);
            if (strcmp("DONE", done) == 0) {
                print_result(task, result);
            } else {
                printf("Worker haven't done.\n");
                close_files(status, task, result);
                exit(EXIT_FAILURE);
            }
        } else if (WIFSIGNALED(st)) {
            printf("Worker fail the work.\nExit status: %d\n", WTERMSIG(st));
            close_files(status, task, result);
            exit(EXIT_FAILURE);
        }
        close_files(status, task, result);
        exit(EXIT_SUCCESS);
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

void get_operations(FILE* task) {
    int num1, num2;
    char c;
    printf("Insert the operation\n(q for quit): ");
    while (scanf(" %d", &num1) == 1) {
        scanf(" %c %d", &c, &num2);
        fprintf(task, "%d %c %d\n", num1, c, num2);
        fflush(task);
        printf("Insert the operation\n(q for quit): ");
    }
}

void print_result(FILE* task, FILE* result) {
    printf("The results:\n");
    int num1, num2, res;
    char c;
    fseek(task, 0, SEEK_SET);
    fseek(result, 0, SEEK_SET);
    while (fscanf(task, " %d %c %d", &num1, &c, &num2) != EOF) {
        fscanf(result, " %d", &res);
        printf("%d %c %d = %d\n", num1, c, num2, res);
    }
}