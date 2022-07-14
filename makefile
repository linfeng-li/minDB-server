

all : server client mfork
.PHONY : all`
server:server.cpp
	g++ -g server.cpp -o  server
client:client.cpp
	g++ -g client.cpp -o client
mfork:mfork.cpp
	g++ -g mfork.cpp -o mfork --std=c++11 -pthread

.PHONY:clean *clean
clean:
	$(RM)  server client mfork
