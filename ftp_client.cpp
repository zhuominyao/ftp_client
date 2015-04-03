#include "ftp_client.h"
#include "common.h"

using namespace std;

int main(int argc,char * argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct hostent * hp = NULL;

	if(argc != 2)
	{
		cout<<"usage:ftp_client hostname"<<endl;
		exit(-1);
	}
	
	cout<<argv[1]<<endl;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	hp = gethostbyname(argv[1]);

	if(hp == NULL)
	{
		cout<<argv[1]<<" unknown"<<endl;
		exit(-1);
	}
	else
		cout<<"get the ip of "<<argv[1]<<endl;

	if(sockfd == -1)
	{
		cout<<"socket error"<<endl;
		exit(-1);
	}
	else
		cout<<"get socket"<<endl;

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	bcopy((void*)hp->h_addr,(void*)&servaddr.sin_addr,hp->h_length);

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) != 0)
	{
		cout<<"connect error"<<endl;
		exit(-1);
	}

	handle_message(sockfd);
}

void handle_message(int sockfd)
{
	cout<<"handle_message"<<endl;
	char buffer[MAXLINE];
	int n = -1;
	if((n = read(sockfd,buffer,MAXLINE)) != 0)
	{
		cout<<buffer<<endl;
		if(strcmp(buffer,"220") == 0)
		{
			cout<<"the connection is successfully established"<<endl;
			ftp_request_loop(sockfd);
		}
		else
		{
			cout<<"fail to establish the connection"<<endl;
			close(sockfd);
			exit(-1);
		}
	}
	else
	{
		close(sockfd);
		cout<<"the connection is closeed"<<endl;
	}
}

void ftp_request_loop(int sockfd)
{
	char request[MAXLINE];
	char cwd[512] = "/home/yaozhuomin/";
	while(true)
	{
		cin>>request;
		if(!is_valid_command(request))
		{
			cout<<"please input the valid command"<<endl;
			cout<<"commands support now include :ls"<<endl;
			continue;
		}
		write(sockfd,request,strlen(request));
		if(strcmp(request,"ls") == 0)
		{
			do_ls(sockfd);
		}
	}
}

bool is_valid_command(char * cmd)
{
	if(strcmp(cmd,"ls") == 0)
		return true;
	else
		return false;
}

void do_ls(int sockfd)
{
	char buffer[MAXLINE];
	int n = -1;
	while((n = read(sockfd,buffer,MAXLINE)) != 0)
	{
		cout<<buffer<<endl;	
	}
}
