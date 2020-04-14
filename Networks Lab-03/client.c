#include <stdio.h> 
#include <stdlib.h> 
#include <netdb.h> 
#include <string.h> 
#include <sys/socket.h> 


#define STR struct sockaddr 

int PORT;
struct sockaddr_in client;
int node_count =0;//count of all node peers


//struct to store the info of nodes when they connect to the relay server for the first time
struct peers{
	char ip_address[50];
	int port_no;

};

struct peers node_peer[8];// node peers

//to compare two strings
int my_strcmp(char *strg1, char *strg2)
{
 
    while( ( *strg1 != '\0' && *strg2 != '\0' ) && *strg1 == *strg2 )
    {
        strg1++;
        strg2++;
    }
 
    if(*strg1 == *strg2)
    {
        return 0; // strings are identical
    }
 
    else
    {
        return *strg1 - *strg2;
    }
}


//function to copy one string to another
char *safe_strcpy(char *dest, size_t size, char *src) {
    if (size > 0) {
        size_t i;
        for (i = 0; i < size - 1 && src[i]; i++) {
             dest[i] = src[i];
        }
        dest[i] = '\0';
    }
    return dest;
}


void getting_ip_address(int sock_id)//function to get all ip address from server
{
	
	char buffer[100]; 
    memset(buffer,'\0',sizeof(buffer));
    strcpy(buffer,"GETNODES");
    send(sock_id,buffer,sizeof(buffer),0);
    printf("Message Sent : %s\n",buffer);
    memset(buffer,'\0',sizeof(buffer));
    printf("Message Received : \n");
    int g = 8; 
    for(;g>0;g--){                 // Loop to accept IPs and PORTs one by one till EXIT msg. is received.
        int val=recv(sock_id, buffer, sizeof(buffer),0);
    		if (val == 0 || strcmp(buffer, "EXIT") == 0){ 
            	printf("IPs received\n"); 
            	break; 
        	}    
       	 char *fir = strtok(buffer, ":"); 
    		strcpy(node_peer[node_count].ip_address,fir);
    		fir = strtok(NULL, ":");
    		node_peer[node_count].port_no=atoi(fir);
    		node_count++;
    }
}


int file_get_node(int sock_id,char file[])//function for  getting file from a node peer
{
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	sprintf(buffer,"FILE:%s",file);
	send(sock_id,buffer,sizeof(buffer),0);
	memset(buffer,'\0',sizeof(buffer));
	recv(sock_id,buffer,sizeof(buffer),0);
	if(buffer[0]=='0')
	{	
		printf("Node peer does not have this file.\n");
		return 0;
	}
		
	printf("file:%s recieved successfully\n",file);
	int total_size=atoi(buffer+2);
	int rec=0;
	char file_buffer[101];
	file_buffer[100]='\0';
	while(rec<total_size)//receiving full file in pieces and printing the pieces one by one
	{
		int val=recv(sock_id,file_buffer,100,0);
		rec+=val;
		printf("%s\n",file_buffer);
	}
	return 1;
}

int main(int argc,char *argv[])
{
	char server_ip[100];
	int server_port;
	int sock_id,fsock_id;
	struct sockaddr_in server_address,cl;
	bzero(&server_address, sizeof(server_address));

	if(argc<3)//checking if all the arguments are not given
	{
		perror("All the arguments are not given!\n");
		return 0;
	}

	safe_strcpy(server_ip,100,argv[1]);
	server_port=atoi(argv[2]);

	sock_id=socket(AF_INET,SOCK_STREAM,0);//creating socket
	if(sock_id==-1)
	{
		perror("socket can't be created\n");
		return 0;
	}
	printf("socket is created successfully\n");

	server_address.sin_family = AF_INET; 
    	server_address.sin_addr.s_addr = inet_addr(server_ip); 
    	server_address.sin_port = htons(server_port);

	if(connect(sock_id, (STR*)&server_address, sizeof(server_address)) != 0)//connecting to server
	{
		perror("connection with relay server can't be established\n");
		return 0;
	}
	printf("connected to relay server successfully\n");
	
	getting_ip_address(sock_id);//getting all ip addresses from server

	close(sock_id);
	printf("connection with relay server is closed\n");

	printf("Enter filename\n");
	char file[100];
	scanf("%s",file);

	struct sockaddr_in new_node;
	int i=0;
	int f=0;
	while(i<node_count)//loop to find out if any node peer has the file
	{
		fsock_id=socket(AF_INET, SOCK_STREAM, 0);
		if(fsock_id==-1)
		{
			perror("file transfer socket can't be created\n");
			return 0;
		}
		printf("file transfer socket is created successfully\n");
		new_node.sin_family=AF_INET;
		new_node.sin_addr.s_addr=inet_addr(node_peer[i].ip_address);
		new_node.sin_port=htons(node_peer[i].port_no);

		printf("Connecting to peer node :%s :%d\n",node_peer[i].ip_address,node_peer[i].port_no);
		
		if(connect(fsock_id,(STR*)&new_node,sizeof(server_address))!=0)
		{
			perror("Connection with node peer is failed\n");
			return 0;
		}
		printf("connection successfully established with node peer\n");
		f|=file_get_node(fsock_id,file);
		close(fsock_id);
		printf("connection closed\n");
		
		if(f==1)
		{
			break;
		}
		i++;

	}	
	if(f==0)
	{
		printf("file can't be retrieved\n");
	}
	return 0;
}


