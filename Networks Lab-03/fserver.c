#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <fcntl.h>
#define MAX 100 
//memset(bufferer,'\0',sizeof(bufferer));
int port_no;
struct sockaddr_in client;
int node_count =0;
//struct to store the info of nodes when they connect to the relay server for the first time
struct peers{
	char ip_address[500];
	int port_no;

};

struct peers array_peers[10];

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

//function for conversation between server and client/node

void socket_function(int socket_id) 
{ 
	char buffer[100]; 
	// read the message from client/node and copy it in bufferer 
	recv(socket_id, buffer, sizeof(buffer),0);
	//read(socket_id, buffer, sizeof(buffer)); 
	// print bufferer which contains the client contents 
	printf("Message Received is : %s\n ", buffer);
	// when the nodes want to register themselves to the relay server
	char *token = strtok(buffer, ":"); 
	if(my_strcmp("REGISTER",token)==0)   
	{
		struct peers new_node;
		
		
		inet_ntop(AF_INET, &(client.sin_addr), new_node.ip_address, 100);
		token = strtok(NULL, ":");
		new_node.port_no = atoi(token);
		int i=0;
		int flag=0;
		while(i<node_count)
		{
			if(array_peers[i].ip_address==new_node.ip_address && array_peers[i].port_no==new_node.port_no)
			{
				flag=1;
			}
			i++;
		}
		
		if(flag==0)
		{
			array_peers[node_count++] = new_node;

		}	
		safe_strcpy(buffer,100,"EXIT");
		//strcpy(buffer,"EXIT");
		send(socket_id, buffer, sizeof(buffer),0); // It sends a message to close the connection.
		printf("Message Sent : %s\n",buffer);
		return;
	}
	else if(my_strcmp("GETNODES",buffer)==0)
	{
		// if the connection is with peer_client. Relay client  
		//sends the list of ip_address and port_no to the client.
		int i = 0;
		for(; i < node_count; i++){     
			sprintf(buffer,"%s:%d",array_peers[i].ip_address, array_peers[i].port_no);
			write(socket_id, buffer, sizeof(buffer));
			printf("%s\n",buffer);
		}
		safe_strcpy(buffer,100,"EXIT");
		//strcpy(buffer,"EXIT");        // It sends a message to close the connection.
		send(socket_id, buffer, sizeof(buffer),0);	
		printf("%s\n",buffer);
		return;
	}
	else             // Just return if no valid message is received.
	{
		return;
	} 
} 

//will check if connection to the node is active
void check_connection(int i)
{
	int socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if(socket_fd<0)
	{
		perror("Error in Socket creation");
	}

	struct sockaddr_in node;
	node.sin_family = AF_INET;
	
	node.sin_addr.s_addr = inet_addr(array_peers[i].ip_address);
	node.sin_port = htons(array_peers[i].port_no);

	//will connect to the node with the above ip and port_no no.
	//if connection fails, will remove the node from the active array_peers list

	int connect_fd = connect(socket_fd,(struct sockaddr *)&node,sizeof(node));
	// 0 denotes successfully connected
	if(connect_fd!=0)
	{
		//will remove the node
		printf("removing node with ip %s and port_no %d \n",array_peers[i].ip_address,array_peers[i].port_no);
		int pos =i;
		while(pos < node_count-1)
		{
			array_peers[pos] = array_peers[pos+1];
			pos++;
		}
		node_count--;
	}else
	{
		//connection is alive
		char buffer[100];
    	strcpy(buffer, "CHECK");
		write(socket_fd, buffer, sizeof(buffer));
	}

	close(socket_fd);
	printf("Checking node is active or not done\n");

}


//main function
int main(int argc, char const *argv[])
{
	/* code */

	if(argc != 2)
	{
		printf("invalid command\n");
		return 0;
	}
	port_no = atoi(argv[1]);
	printf("%d\n",port_no);
	struct sockaddr_in server_info;
	int server_info_len = sizeof(server_info);
	int server_fd ,new_socket_fd;
	server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_fd==0)
	{
		perror("error in socket creation");
		return 0;
	}else{
		printf("socket created successfully\n");
	}
	bzero(&server_info,sizeof(server_info));
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = INADDR_ANY;
	server_info.sin_port = htons(port_no);

	//we need to bind socket to the entered server information
	int bind_value;
	bind_value = bind(server_fd,(struct sockaddr *) &server_info,sizeof(server_info));

	//bind returns 0 upon success
	if(bind_value!= 0)	
	{
		perror("bind failed");
		return 0;
	}
	else
		printf("Socket successfully binded\n"); 
	//5 denotes no of parallel active connections
	if(listen(server_fd,5) <0)
	{
		perror("listen");
		return 0;
	}else{
		printf("server listening successfully\n");
	}

	int client_len = sizeof(client);

	//the server will remain on always during which two types of connection can 
	//take place. Either a node can connect to the server to register itself or
	//client can connect to the server to get the nodes information
	while(1)
	{
		//new socket is created on tcp connection 
		new_socket_fd = accept(server_fd,(struct sockaddr *)&client,&client_len);
		if(new_socket_fd <0 )
		{
			perror("accept issue");
			return 0;
		}
		else{
			printf("connection by either node/client is accepted\n");
		}
		//checking active nodes. We have to do this every time a new connection is accepted

		int i =node_count-1;
		while(i>=0)
		{
			//will check if connection to the node is active
			check_connection(i);
			i--;
		}

		//function for further operations between server and client/node such as
		//registering node or getting node info for client
		socket_function(new_socket_fd);
		//closing the new socket after transfer is done
		close(new_socket_fd);
		printf("Closing connection to the accepted node/client\n");
	}

	//closing the initially created socket
	close(server_fd);
	printf("Closing server connection\n");

	return 0;
}