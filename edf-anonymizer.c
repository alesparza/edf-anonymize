#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edf-anonymizer.h"
#include "mini-hexdump.h"

void printHelp() {
  printf("Usage: edf-anonymizer [-i]\n");
  printf("\t-i: input file name.  The expected format is .edf\n");
}

char* getInputName() {
  char* input = calloc(BUFFER_SIZE , sizeof(char));
  printf("Please enter the input file name: ");
  fgets(input, BUFFER_SIZE, stdin);
  input[strlen(input) - 1] = '\0';
  return input;
}

char* setOutputFilename(char* inputFileName) {
  char* outputFileName = malloc(sizeof(char) * (strlen(inputFileName) + strlen(DEID_FILE_SUFFIX) + 1));
  strcpy(outputFileName, inputFileName);
  char* extensionIndex = strstr(outputFileName, EDF_EXTENSION);
  if (extensionIndex == NULL) {
    printf("Can't find .edf extension, unable to create output file\n");
    exit(1);
  }
  strcpy(extensionIndex, DEID_EDF_EXTENSION);  // copies new extension to filename

  printf("Set output filename to %s\n", outputFileName);
  return outputFileName;
}

int main(int argc, char **argv) {
  // handle arguments
  if (argc < MINIMUM_ARGUMENTS) {
    printHelp();
    exit(1);
  }

  char* inputFileName = NULL;

  for (int i = 0; i < argc; i++) {
    if (strcmp("-i", argv[i]) == 0) {
      i++;
      if (argv[i] == NULL) {
        printHelp();
        exit(1);
      }
      inputFileName = argv[i];
    }
  }

  if (inputFileName == NULL) {
    inputFileName = getInputName();
  }

  // setup file names
  char* outputFileName = setOutputFilename(inputFileName);
  miniHexDump(inputFileName, HEADER_LENGTH);

  printf("Please enter replacement data for Local Patient Identification (80 character max): ");
  char* newData = calloc(81, sizeof(char));
  fgets(newData, LOCAL_PATIENT_IDENFITICATION_LENGTH, stdin);
  *(newData + strlen(newData) - 1) = '\0'; // removing the \n
  for (int i = 0; i < LOCAL_PATIENT_IDENFITICATION_LENGTH; i++) {
    if (*(newData + i) == '\0') {
      *(newData + i) = ' ';
    }
  }
  printf("Anonymizing file (this can take a bit for large files)\n");
  fflush(stdout);

  FILE* input = fopen(inputFileName, "rb");
  FILE* output = fopen(outputFileName, "wb");

  // copy the version
  int version[VERSION_LENGTH];
  fread(version, VERSION_LENGTH, sizeof(char), input);
  fwrite(version, VERSION_LENGTH, sizeof(char), output);

  // write the local data
  int buffer[BUFFER_SIZE];
  memset(buffer, '\0', sizeof(buffer));
  fread(buffer, LOCAL_PATIENT_IDENFITICATION_LENGTH, sizeof(char), input);
  fwrite(newData, LOCAL_PATIENT_IDENFITICATION_LENGTH, sizeof(char), output);
  free(newData);

  // write the rest of the original file
  memset(buffer, '\0', sizeof(buffer));
  while ((fread(buffer, sizeof(buffer), sizeof(char), input)) != 0) {
    fwrite(buffer, sizeof(buffer), sizeof(char), output);
    memset(buffer, '\0', sizeof(buffer));
  }


  fclose(input);
  fclose(output);
  printf("Done writing the output file %s\n", outputFileName);
  miniHexDump(outputFileName, HEADER_LENGTH);

  free(outputFileName);

  return 0;
}
