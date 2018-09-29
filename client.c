#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "sockettome.h"

#define MIN(x, y) ((x < y) ? x : y)
#define PACKET_SIZE 1024
#define FILENAME_SIZE 1024

int main(int argc, char** argv)
{
    uint8_t* data;
    FILE*    input;
    char*    filename;
    int      port, socket, fd, size;
    int      count, filesize, temp;

    /* Error check */
    if(argc != 3)
    {
        fprintf(stderr, "Usage: betterCP_C hostname port\n");
        exit(1);
    }

    port = atoi(argv[2]);
    if(port < 8000)
    {
        fprintf(stderr, "Usage: betterCP_C hostname port\n");
        fprintf(stderr, "       port must be >= 8000\n");
        exit(1);
    }

    /* Connect to the server */
    fd = request_connection(argv[1], port);
    filename = malloc(FILENAME_SIZE);

    /* Read file, get filesize */
    printf("File to transfer: ");
    fgets(filename, FILENAME_SIZE, stdin);
    filename[sizeof(filename)] = '\0';
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

    /* Send name and size, malloc data, sleep to let server catch up */
    printf("Filename: %s\n", filename);
    printf("Filsize:  %d\n", filesize);
    write(fd, filename, FILENAME_SIZE);
    write(fd, &filesize, sizeof(int));
    data = malloc(PACKET_SIZE);
    sleep(1);

    /* write file PACKET_SIZE bytes at a time */
    for(count = filesize; count > 0; count -= PACKET_SIZE)
    {
        port = temp = MIN(count, PACKET_SIZE);
        fread(data, port, 1, input);
        while(temp != 0)
        {
            write(fd, data + (port - temp), temp);
            read(fd, &size, sizeof(int));
            temp -= size;
            if(size != PACKET_SIZE)
                printf("Sent %d bytes\n", size);
        }
    }
    count = (count < 0) ? 0 : count;
    printf("Sent %d/%d bytes\n", filesize - count, filesize);

    free(data); 
    free(filename);
    return(0);
}
