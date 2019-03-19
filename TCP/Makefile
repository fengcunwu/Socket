all: TCPServer TCPClient
TCPServer:TCPServer.cpp
	g++ -o $@ $^
TCPClient:TCPClient.cpp
	g++ -o $@ $^
.PHONY:clean
clean:
	rm -f TCPServer TCPClient
