#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal_helper.h"
#include "stdbool.h"

//Write to GPIO file
void writeToFile(const char* fileToWriteGpio, const char* content) {
    FILE *pFile = fopen(fileToWriteGpio, "w");
    if (pFile == NULL){
        printf("Error to open the file %s\n", fileToWriteGpio); 
        exit(EXIT_FAILURE);
    }

    //write into the file
    fprintf(pFile, "%s", content);
    fclose(pFile);
}

//Read to GPIO file
void readFromFile(const char* fileToReadGpio, char* buff, unsigned int maxLength){
    FILE *pFile = fopen(fileToReadGpio, "r");
    if (pFile == NULL) {
        printf("Error to open the file %s\n", fileToReadGpio); 
        exit(EXIT_FAILURE);
    }

    // Use fgets to read a line into the provided buffer
    if (fgets(buff, maxLength, pFile) == NULL) {
        fclose(pFile);
        exit(EXIT_FAILURE);
    }

    fclose(pFile);
}

// Read values
int valueReader(char *fileName) {
    FILE *pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", fileName);
        exit(-1);
    }

    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);

    // Close
    fclose(pFile);

    // Copy the buffer and return
    int number = atoi(buff);
    return number;
}

// Provided by Brian Fraser (Runs a linux command)
void runCommand(char* command) {
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

// Provided by Brian Fraser
void write_int_to_file(const char* filename, int value){
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error OPENING %s.", filename);
        exit(1);
    }
    
    int writtenValue = fprintf(file, "%d" , value);
    if (writtenValue <= 0){
        printf("ERROR WRITING DATA");
        exit(1);
    }
    fclose(file);
}

void fileWriter(char* filePath, char* data) {
    FILE *file = fopen(filePath, "w");

	if (file == NULL){
    	printf("ERROR OPENING %s\n", filePath);
    	exit(1);
	}

    int charWritten = fprintf(file, "%s", data);
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA\n");
        exit(1);
    }

	fclose(file);
}