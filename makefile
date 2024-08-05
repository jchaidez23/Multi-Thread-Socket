CC1=g++ -O3 -Wall -pthread -std=c++11

CFLAGS = -g -c -Wall
#CFLAGS = -ansi -c -Wall -pedantic
all: Client Server

A = Client.o
B = Server.o

Client: $A
	$(CC1) -o Client $A

Client.o: Client.cpp
	$(CC1) -c Client.cpp

Server: $B
	$(CC1) -o Server $B

Server.o: Server.cpp
	$(CC1) -c Server.cpp

clean:
	rm *.o
	rm Server
	rm Client

