#include "ftp_client.h"
#include "common.h"

using namespace std;

char default_work_directory[MAXLINE] = "/home/yaozhuomin/";

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

	sockfd = socket(AF_INET,SOCK_STREAM,0);//SOCK_STREAM表示运用tcp协议
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
	bcopy((void*)hp->h_addr,(void*)&servaddr.sin_addr,hp->h_length);//填充socket

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) != 0)
	{
		cout<<"connect error"<<endl;
		exit(-1);
	}

	handle_message(sockfd);
}

void handle_message(int sockfd)
{
	char buffer[MAXLINE];
	int n = -1;
	if((n = read(sockfd,buffer,MAXLINE)) != 0)
	{
		buffer[n] = '\0';
		if(strcmp(buffer,"220") == 0)//如果服务器返回的是220,则表示连接成功建立
		{
			cout<<"the connection is successfully established"<<endl;
			ftp_request_loop(sockfd);
		}
		else//否则表示连接建立不成功
		{
			cout<<"fail to establish the connection"<<endl;
			close(sockfd);
			exit(-1);
		}
	}
	else
	{
		close(sockfd);
		cout<<"the connection is closeed because the client connecting to server is overload"<<endl;
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
			cout<<endl;
			continue;
		}
		if(strcmp(request,"ls") == 0)
		{
			write(sockfd,request,strlen(request));
			do_ls(sockfd);
			cout<<endl;
		}
		else if(strcmp(request,"cd") == 0)
		{	
			scanf("%s",parameter);//读取变更的路径
			write(sockfd,request,strlen(request));
			char confirm[5];
			int n;
			n = read(sockfd,confirm,5);
			write(sockfd,parameter,strlen(parameter));//在收到confirm之后,再发送请求变更的路径
			do_cd(sockfd);
			cout<<endl;
		}
		else if(strcmp(request,"pwd") == 0)
		{
			write(sockfd,request,strlen(request));
			do_pwd(sockfd);
			cout<<endl;
		}
		else if(strcmp(request,"get") == 0)
		{	
			scanf("%s",parameter);//读取请求获取的文件名
			write(sockfd,request,strlen(request));
			char confirm[5];
			int n;
			n = read(sockfd,confirm,5);
			write(sockfd,parameter,strlen(parameter));//在收到confirm之后,发送请求获取的文件名
			do_get(sockfd,parameter);
			cout<<endl;
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
	FILE * fr = fdopen(sockfd,"rb");
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
		FILE * fw = fopen(absolute_path,"wb+");
		if(fw == NULL)
		{
			cout<<"fail to open file"<<endl;
			return;
		}

		int flags = fcntl(sockfd,F_GETFL,0);
		bool start = false;
		fcntl(sockfd,F_SETFL,flags | O_NONBLOCK);//将socket变成非阻塞的
		while(true)
		{				
			char buffer[MAXLINE];
			cout<<"in while"<<endl;
			int n = read(sockfd,buffer,MAXLINE);
			cout<<"n:"<<n<<endl;
			if((n == 0 || n == -1) && start)
				break;
			if(n != 0 && n != -1)
			{
				start = true;
				fwrite(buffer,1,n,fw);
				fflush(fw);
			}
		}
		fcntl(sockfd,F_SETFL,flags);//将socket还原成原来的属性
		cout<<"get file successfully"<<endl;
		fclose(fw);
	}
	else if(result == 3)
		cout<<"the file you request is not exist or you have no right to access it"<<endl;
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
	fscanf(fr,"%d",&result);
	if(result == 1)
		cout<<"change directory successfully"<<endl;
	else if(result == 2)
		cout<<"fail to change directory : the directory you request is not a path"<<endl;
	else if(result == 3)
		cout<<"fail to change directory : the directory you request is not exist or you have no right to access it"<<endl;
}
