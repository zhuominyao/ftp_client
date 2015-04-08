#include "ftp_client.h"
#include "common.h"

using namespace std;

char default_work_directory[MAXLINE] = "/";

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
	char parameter[MAXLINE];
	while(true)
	{
		scanf("%s",request);
		if(!is_valid_command(request))
		{
			cout<<"please input the valid command"<<endl;
			cout<<"commands support now include :ls,cd,pwd,get"<<endl;
			continue;
		}
		if(strcmp(request,"ls") == 0)
		{
			//cout<<"in if"<<endl;
			write(sockfd,request,strlen(request));
			do_ls(sockfd);
		}
		else if(strcmp(request,"cd") == 0)
		{	
			scanf("%s",parameter);
			write(sockfd,request,strlen(request));
			char confirm[5];
			int n;
			n = read(sockfd,confirm,5);
			confirm[n] = '\0';
			cout<<confirm<<endl;	
			write(sockfd,parameter,strlen(parameter));
			do_cd(sockfd);
		}
		else if(strcmp(request,"pwd") == 0)
		{
			write(sockfd,request,strlen(request));
			do_pwd(sockfd);
		}
		else if(strcmp(request,"get") == 0)
		{	
			scanf("%s",parameter);
			write(sockfd,request,strlen(request));
			char confirm[5];
			int n;
			n = read(sockfd,confirm,5);
			confirm[n] = '\0';
			write(sockfd,parameter,strlen(parameter));
			do_get(sockfd,parameter);
		}
	}
}

bool is_valid_command(char * cmd)
{
	if(strcmp(cmd,"ls") == 0)
		return true;
	if(strcmp(cmd,"cd") == 0)
		return true;
	if(strcmp(cmd,"pwd") == 0)
		return true;
	if(strcmp(cmd,"get") == 0)
		return true;
	return false;
}

void do_get(int sockfd,char * parameter)
{
	FILE * fr = fdopen(sockfd,"r");
	int result;
	
	fscanf(fr,"%d",&result);
	if(result == 1)
		cout<<"the file you request is a directory"<<endl;
	else if(result == 2)
	{
		cout<<"the file you request exist"<<endl;
		char * filename;
		int i;
		char absolute_path[MAXLINE];
		for(i = strlen(parameter);i >= 0;i--)
		{
			if(parameter[i] == '/')
				break;
		}
		if(i == -1)
			filename = parameter;
		else
			filename = parameter + i + 1;
		cout<<filename<<endl;
		strcpy(absolute_path,default_work_directory);
		strcat(absolute_path,filename);
		cout<<absolute_path<<endl;
		//FILE * fw = fopen()
	}
	else if(result == 3)
		cout<<"the file you request is not exist"<<endl;
}

void do_pwd(int sockfd)
{
	FILE * fr = fdopen(sockfd,"r");
	char wd[MAXLINE];
	fscanf(fr,"%s",wd);
	cout<<"the work directory now is:"<<wd<<endl;
}

void do_ls(int sockfd)
{
	char buffer[MAXLINE];
	int n = -1;
	FILE * fr = fdopen(sockfd,"r");
	while( fgets(buffer,MAXLINE,fr) != NULL && strcmp(buffer,"endoffile\n") != 0)
		cout<<buffer;	
}

void do_cd(int sockfd)
{
	FILE * fr = fdopen(sockfd,"r");
	int result;
	cout<<"before fscanf"<<endl;
	fscanf(fr,"%d",&result);
	if(result == 1)
		cout<<"change directory successfully"<<endl;
	else if(result == 2)
		cout<<"fail to change directory : the directory you request is not a path"<<endl;
	else if(result == 3)
		cout<<"fail to change directory : the directory you request is not exist"<<endl;
}
