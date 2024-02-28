#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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