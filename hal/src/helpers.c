
#include <stdlib.h>
#include <stdio.h>


int read_int_value_from_file(char* file_path){
    FILE *file = fopen(file_path, "r");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    int content;
    int charRead = fscanf(file, "%d", &content);
    if (charRead < 0){
        printf("Error reading from file!\n");
        exit(1);
    }
    fclose(file);
    return content;
}

void set_int_value_in_file(char* file_path, int content){
    FILE *file = fopen(file_path, "w");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    int charWritten = fprintf(file, "%d", content);
    if (charWritten < 0){
        printf("Error writing to file!\n");
        exit(1);
    }
    fclose(file);
}

void set_char_value_in_file(char* file_path, char* content){
    FILE *file = fopen(file_path, "w");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    int charWritten = fprintf(file, "%s", content);
    if (charWritten < 0){
        printf("Error writing to file!\n");
        exit(1);
    }
    fclose(file);
}


void run_command(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}