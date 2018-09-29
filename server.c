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
    FILE*    output;
    char*    filename;
    int      port, socket, fd;
    int      count, filesize, temp;

    /* Error check */
    if(argc != 2)
    {
        fprintf(stderr, "Usage: Server port\n");
        exit(1);
    }

    port = atoi(argv[1]);
    if(port < 8000)
    {
        fprintf(stderr, "Usage: Server port\n");
        fprintf(stderr, "       port must be >= 8000\n");
        exit(1);
    }

    /* Open port and socket, allocate for data and filename */
    socket = serve_socket(port);
    data = malloc(sizeof(uint8_t) * PACKET_SIZE);
    filename = malloc(sizeof(char) * FILENAME_SIZE);

    /*
    input = fdopen(fd, "r");
    output = fdopen(fd, "w");
    fgets(filename, 1024, input);
    fputs(filename, stdout);
    sprintf(filename, "Cool cool, thanks\n");
    write(fd, filename, strlen(filename));
    fclose(input);
    fclose(output);
    free(data);
    free(filename);
    return(0);
    */

    /* Continuously look for connections and read data */
    while(1)
    {
        /* Accept connection, get filename and filesize */
        fd = accept_connection(socket);
        printf("Someone connected\n");
        read(fd, filename, FILENAME_SIZE);
        printf("Filename: %s\n", filename);
        read(fd, &filesize, sizeof(int));
        printf("Filesize: %d\n", filesize);

        /* Open and check output, open input for reading binary */
        output = fopen(filename, "ab");
        if(output == NULL)
        {
            fprintf(stderr, "Could not make file ");
            perror(filename);
            free(data);
            free(filename);
            return(1);
        }

        /* Read file PACKET_SIZE bytes at a time */
        for(count = filesize; count > 0; count -= PACKET_SIZE)
        {
            temp = MIN(count, PACKET_SIZE);
            if(read(fd, data, temp) != temp)
                break;
            else
                fwrite(data, temp, 1, output);
        }

        /* If the read failed, report it */
        if(count > 0)
            fprintf(stderr, "Socket closed early. Wrote %d/%d bytes\n", filesize - count, filesize);    

        /* Clean up and do it again */
        fclose(output);
    }

    /* This shouldn't happen */
    free(data);
    free(filename);
    return(0);
}
