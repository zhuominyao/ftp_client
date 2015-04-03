tcp_client:ftp_client.o
	g++ -o ftp_client ftp_client.o

tcp_client.o:ftp_client.cpp
	g++ -c ftp_client.cpp
