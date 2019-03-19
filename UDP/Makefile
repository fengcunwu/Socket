all:UDPClient UDPServer
UDPServer:UDPServer.cpp
	g++ -o $@ $^
UDPClient:UDPClient.cpp
	g++ -o $@ $^
.PHONY:clean
clean:
	rm -f UDPServer UDPClient
