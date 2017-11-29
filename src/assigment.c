#define BUFFER_SIZE 200000
#define FILENAMELENGTH 100
#define BUFFER_BYTE 16

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <math.h>
#include "helper.h"

void printHelp() {
    //Helper for right usage
    printf("Usage: Insert -i (input file) or -o (output file) or -s (statistic file)");
}

int main(int argc, char *argv[]) {
    FILE * fOutputfile; //INITIALISIEREN
    //char *inputfile [FILENAMELENGTH * sizeof(char)];
    char *inputfile = malloc(FILENAMELENGTH* sizeof(char));
    char *outputfile = malloc(FILENAMELENGTH* sizeof(char)); //Magic number
    char *statisticsfile = malloc(FILENAMELENGTH* sizeof(char)); //Magic number
    int opt =0; //Magic number

    while ((opt =getopt(argc, argv, "hi:o:s:"))!= -1){
        switch (opt){
            case 'h':
                printHelp();
                return EXIT_SUCCESS;
            case 'i':
                inputfile = strdup(optarg);
                //snprintf(inputfile,FILENAMELENGTH * sizeof(char),"%s",optarg);
                //inputfile = optarg;
                break;
            case 'o':
                outputfile = strdup(optarg);
                //snprintf(outputfile,FILENAMELENGTH * sizeof(char),"%s",optarg);
                break;
            case 's':
                statisticsfile = strdup(optarg);
                //snprintf(statisticsfile,FILENAMELENGTH * sizeof(char),"%s",optarg);
                break;
            default:
                printHelp();
                return EXIT_FAILURE;

        }
    }

    int file = open(inputfile, O_RDONLY); //Open the file for read only
    fOutputfile = fopen(outputfile, "w"); //Open output file Write into it
    if(fOutputfile == 0){
        printHelp();
        printf("ERROR COULD NOT OPEN THE FILE");
        return EXIT_FAILURE;
    }
    if (file < 0) { //Could not open the file
        printHelp();
        printf("ERROR COULD NOT OPEN THE FILE");
        return EXIT_FAILURE;
    } else {
        if (flock(file, LOCK_SH) == 0) {//lock file to use it

            char buffer[BUFFER_SIZE];
            int read_bytes, bytes_to_decode;
            if ((read_bytes = read(file, buffer, BUFFER_SIZE)) > 0) {

                bytes_to_decode = read_bytes;

                while (bytes_to_decode > 0) {//start to decode the input
                    char currentBuffer[BUFFER_BYTE +1] ={0};//+1 for String-Terminator
                    int offset = read_bytes - bytes_to_decode; //Magic number
                    bytes_to_decode -= BUFFER_BYTE;

                    strncpy(currentBuffer, buffer + offset, BUFFER_BYTE);

                    int i;
                    uint16_t character; //initialiseren
                    for (i = 0; i < 16; i++) {
                        int bit = currentBuffer[i] - '0'; // hack for char to int

                        character = character << 1; // binary shift
                        character = character | bit; // bitwise OR
                    }

                    u_int16_t actual_character = decodefile(character);

                    fprintf(fOutputfile,"%c", actual_character);//write into output file

                }

                fclose(fOutputfile);//close output file
                fOutputfile = fopen(outputfile, "r");
                if(fOutputfile == 0){
                    printHelp();
                    printf("ERROR COULD NOT OPEN THE FILE");
                    return EXIT_FAILURE;
                }
                int wordcount = countWordsinfile(fOutputfile); //erwartet FILE *Pointer
                statisticFileWriter(outputfile, statisticsfile, wordcount);
                fclose(fOutputfile);

            } else if (read_bytes == -1) {
                printHelp();
                printf("ERROR CANT READ ALL BYTES");
            }

            flock(file, LOCK_UN);
        } else {
            printHelp();
            printf("ERROR CANT OPEN FILE");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


