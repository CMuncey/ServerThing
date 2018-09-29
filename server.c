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
    FILE*    output;
    char*    filename;
    int      port, socket, fd, size;
    int      count, filesize, temp;

    /* Error check */
    if(argc != 2)
    {
        fprintf(stderr, "Usage: betterCP_S port\n");
        exit(1);
    }

    port = atoi(argv[1]);
    if(port < 8000)
    {
        fprintf(stderr, "Usage: betterCP_S port\n");
        fprintf(stderr, "       port must be >= 8000\n");
        exit(1);
    }

    /* Open port and socket, allocate for data and filename */
    socket = serve_socket(port);
    data = malloc(sizeof(uint8_t) * PACKET_SIZE);
    filename = malloc(sizeof(char) * FILENAME_SIZE);

    /* Accept connection, get filename and filesize */
    fd = accept_connection(socket);
    printf("Someone connected\n");
    read(fd, filename, FILENAME_SIZE);
    printf("Filename: %s\n", filename);
    read(fd, &filesize, sizeof(int));
    printf("Filesize: %d\n", filesize);

    /* Open and check output, open input for reading binary */
    output = fopen(filename, "wb");
    if(output == NULL)
    {
        fprintf(stderr, "Could not make file ");
        perror(filename);
        free(data);
        free(filename);
        return(1);
    }

    printf("Beginning file transfer\n");
    /* Read file PACKET_SIZE bytes at a time */
    for(count = filesize; count > 0; count -= PACKET_SIZE)
    {
        temp = MIN(count, PACKET_SIZE);

        /* This was meant to be a failsafe in case it didn't read it all */
        /* But that didn't really work out, so I reduced PACKET_SIZE instead */
        while(temp > 0)
        {
            if((size = read(fd, data, temp)) == -1)
            {
                fprintf(stderr, "Socket closed early. Wrote %d/%d bytes\n", filesize - count, filesize);
                exit(1);
            }
            else
            {
                fwrite(data, size, 1, output);
            }
            write(fd, &size, sizeof(int));
            temp -= size;
        }
    }
    printf("File %s transferred\n", filename);

    fclose(output);
    free(data);
    free(filename);
    return(0);
}
