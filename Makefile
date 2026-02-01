CC = g++
CFLAGS = -Wall -g -I./src


all: client server

client: 
	$(CC) $(CFLAGS) src/client/Client.cpp -o client -lssl -lcrypto 

server: 
	$(CC) $(CFLAGS) 									\
		src/server/Server.cpp 							\
		src/server/container/Container.cpp				\
		src/server/session/Session.cpp					\
		src/server/threads/Threads.cpp					\
		src/server/database/Database.cpp				\
		src/server/session/SessionManager.cpp			\
		-o server -lutil -lsqlite3 -lssl -lcrypto 

clean:
	rm -f client server

# For insert user into database!
# all: 
# 	$(CC) $(CFLAGS) src/main.cpp src/server/database/Database.cpp -o main -lsqlite3 -lssl -lcrypto