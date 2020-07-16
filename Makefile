standard = 17
libraries = -pthread -lboost_thread
flags = -g -std=c++$(standard)


all: server client

server: server.cpp 
	g++ server.cpp -o server.o $(libraries) $(flags) 

client: client.cpp 
	g++ client.cpp -o client.o $(libraries) $(flags) 

clean:
	rm -f *.o 
