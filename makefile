CC = gcc

INCLUDES = -I/home/Chris/CS/360/src

CFLAGS = -g $(INCLUDES)

LIBDIR = /home/Chris/CS/360/src

LIBS = $(LIBDIR)/sockettome.o

EXECUTABLES = Server Client

all: $(EXECUTABLES)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c

Server: server.o
	$(CC) $(CFLAGS) -o Server server.o $(LIBS)

Client: client.o
	$(CC) $(CFLAGS) -o Client client.o $(LIBS)

clean:
	rm $(EXECUTABLES) server.o client.o
