#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "config.h"

char *path="null";

void get_response(int sock)
{
	char replymessage[1024];
	memset(replymessage, 0, sizeof replymessage);
    ssize_t numBytesRecv = 0;
    numBytesRecv = recv(sock,replymessage,1024,0);
    if(numBytesRecv < 0)
    {
	    printf("ERROR");
		exit(0);
    }
    //printf("%s\n",replymessage);
}

void send_request(int sock, const char* buf_request)
{
	if(send(sock,buf_request, strlen(buf_request), 0) < 0)
    {
	    printf("ERROR REQUEST\n");
		exit(0);
    }
}

void create_socket_data(int sock, int port)
{
	const int size_buf = 4096;
    char buf[size_buf];
	char buf_stor[255];
	char temp[255];
	char fn[50];
	int sockfd2 = 0;
	struct sockaddr_in serv_addr2;
	//path = "/tmp/kupa.txt";
    FILE *file;
    file = fopen(path, "r");
    if(file == NULL) 
    {
		fclose(file);
		strcat(temp,ROOT_DIR);
		strcat(temp,"/");
		strcat(temp,path);
		printf("%s",temp);
		file = fopen(temp,"r");
		if(file == NULL)
		{
			printf("ERROR FILE\n");
			exit(0);
		}
        //printf("ERROR FILE\n");
		//exit(0);
    }
    struct stat st;
    stat(path, &st);
	
	if((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        exit(0);
    }
	
	memset(&serv_addr2, '0', sizeof(serv_addr2));

    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_port = htons(port);

    if(inet_pton(AF_INET, ADDR, &serv_addr2.sin_addr)<=0)
     {
		printf("\n inet_pton error occured\n");
		exit(0);
     }

     if( connect(sockfd2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
	
	int lenght = strlen(path);
	int index=0;
	for(int i=0;i<lenght;i++)
	{
		if(path[i]=='/')
		{
			index=i;
		}
	}
	int j=0;
	index=(index+1);
	
	char tab[50];
	memset(tab, 0, sizeof tab);
	for(int i=index;i<lenght;i++)
	{
		tab[j]=path[i];
		j++;
	}
	
	sprintf(buf_stor, "STOR %s\r\n",tab);
	
	
	send_request(sock,buf_stor);
	
	get_response(sock);
	
	char buf_file[st.st_size];
    for(int i=0;i<st.st_size;i++)
    {
        buf_file[i]=fgetc(file);
    }
	
	send_request(sockfd2,buf_file);
	
	fclose(file);
	close(sockfd2);
	
	get_response(sock);
}

void filename(char *file)
{
	path=file;
}

void send_file(int sock)
{
	char buf_pasv[255];
	sprintf(buf_pasv, "PASV \r\n");
	if(send(sock,buf_pasv, strlen(buf_pasv), 0) < 0)
    {
	    printf("ERROR PASV\n");
    }

    char repl[1024];
	memset(repl, 0, sizeof repl);
    ssize_t numb = 0;
    numb = recv(sock,repl,1024,0);
    if(numb < 0)
    {
	    printf("RESPONSE PASV ERROR\n");
    }
  
	int count_comma=0;
	int counter=0;
	int p1=0,p2=0;
	int index=0;
	for(int i=(numb-5);i>=0;i--)
	{
		if(repl[i]==',')
		{
			count_comma++;
			if(count_comma==1)
			{
				p2=counter;
				counter=0;
			}
			else if(count_comma==2)
			{
				index=i;
				p1=(counter-1);
				break;
			}
		}
		counter++;
	}
	char port[p1];
	char port2[p2];
	count_comma=0;
	int j=0;
	for(int i=(index+1);i<=(numb-5);i++)
	{
		if((repl[i]!=',')&& (count_comma<1))
		{
			port[j]=repl[i];
			j++;
		}
		else if(repl[i]==',')
		{
			count_comma++;
			j=0;
		}
		else if((repl[i]!=',') && (count_comma==1))
		{
			port2[j]=repl[i];
			j++;
		}
	}
	int po1,po2;
	sscanf(port,"%d",&po1);
	sscanf(port2,"%d",&po2);
	int server_port=(po1 * 256 + po2);
	create_socket_data(sock,server_port);
}

void create_socket(void)
{
    int sockfd = 0;
	struct sockaddr_in serv_addr;
	char buf_request[255];
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(21);

    if(inet_pton(AF_INET, ADDR, &serv_addr.sin_addr)<=0)
    {
		printf("\n inet_pton error occured\n");
		exit(0);
	}


    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
		exit(0);
    }
    
	get_response(sockfd);
	
	sprintf(buf_request, "USER %s\r\n",USER);
	
	send_request(sockfd,buf_request);
	
	get_response(sockfd);
	
	sprintf(buf_request, "PASS %s\r\n",PASS);
	
	send_request(sockfd,buf_request);
	
	get_response(sockfd);
	
	send_file(sockfd);
	
	sprintf(buf_request, "QUIT\r\n");
	
	send_request(sockfd,buf_request);
	
	get_response(sockfd);
	
	close(sockfd);
	
	printf("TRANSFER COMPLETED\n");
	
}