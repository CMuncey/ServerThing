#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "sockettome.h"

#define MIN(x, y) ((x < y) ? x : y)
#define PACKET_SIZE 8192
#define FILENAME_SIZE 1024

int main(int argc, char** argv)
{
    uint8_t* data;
    FILE*    input;
    FILE*    fin;
    char*    filename;
    int      port, socket, fd, fd2;
    int      count, filesize, temp;

    /* Error check */
    if(argc != 3)
    {
        fprintf(stderr, "Usage: Client hostname port\n");
        exit(1);
    }

    port = atoi(argv[2]);
    if(port < 8000)
    {
        fprintf(stderr, "Usage: Client hostname port\n");
        fprintf(stderr, "       port must be >= 8000\n");
        exit(1);
    }

    /* Request connection, open fin */
    fd   = request_connection(argv[1], port);
    fin  = fdopen(fd, "r");

    /* Read file, get filesize */
    filename = malloc(1024);
    fgets(filename, FILENAME_SIZE, stdin);
    filename[sizeof(filename)-1] = '\0';
    input = fopen(filename, "rb");
    if(input == NULL)
    {
        fprintf(stderr, "Could not open file ");
        perror(filename);
        exit(1);
    }
    fseek(input, 0, SEEK_END);
    filesize = ftell(input);
    fseek(input, 0, SEEK_SET);

    /* Send name and size, malloc data */
    printf("Filename: %s\n", filename);
    printf("Filsize:  %d\n", filesize);
    write(fd, filename, FILENAME_SIZE);
    write(fd, &filesize, sizeof(int));
    data = malloc(PACKET_SIZE);

    /* Write file PACKET_SIZE bytes at a time */
    for(count = filesize; count > 0; count -= PACKET_SIZE)
    {
        temp = MIN(count, PACKET_SIZE);
        fread(data, temp, 1, input);
        write(fd, data, temp);
    }

    free(data); 
    fclose(fin);
    free(filename);
    return(0);
}
