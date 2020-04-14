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
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <fcntl.h>

# define MAX 100
//For copying the string from src to dst 
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

//For converting string to integer
int str_to_int(char* str) 
{ 
    int res = 0; // Initialize result 
  
    // Iterate through all characters of input string and 
    // update result 
    for (int i = 0; str[i] != '\0'; ++i) 
        res = res * 10 + str[i] - '0'; 
  
    // return result. 
    return res; 
}

//For comparing two strings
int my_strcmp(char *strg1, char *strg2)
{
    while(( *strg1 != '\0' && *strg2 != '\0' ) && *strg1 == *strg2 ){
   		strg1++;
        strg2++;
    
    }
	if(*strg1 == *strg2){
		return 0;
	}
    else{
	    return *strg1 - *strg2;
    }
}

void connect_server(int socket_id,int port){
	//Buffer to read and write data into socket
	char buffer[1024];
	memset(buffer,'\0',sizeof(buffer));

	//Sending CONNECT message along with port
	sprintf(buffer,"REGISTER:%d", port);
    send(socket_id, buffer, sizeof(buffer),0);
    printf("Mssg Sent : %s\n", buffer); 

    memset(buffer,'\0',sizeof(buffer)); 
    // Waiting for EXIT from server to terminate
    recv(socket_id, buffer, sizeof(buffer),0);

    printf("Message Received : %s\n", buffer); 
    if ((my_strcmp(buffer, "EXIT")) == 0) { 	
        printf("Registered successfully on Server\n"); 
    }  
    else{
    	perror("Unexpected message from server\n");
    	exit(EXIT_FAILURE);
    }

    return;


}


//This is for sending the file to client
void send_file(FILE *ptr, int socket_id){
    int size = 0;
    char buff[MAX];
    char curr;

    //If buffer is filled we have to initialise it again
    while((curr = getc(ptr)) != EOF)
    {
        // printf("%c", curr);
        buff[size++] = curr;
        if(size == MAX){    // if buffer is filled sending the message
              // and reinitializing buffer size to be empty
            int sent = write(socket_id, buff, sizeof(buff));
            size = 0;
            if(sent == 0) break;
            if(sent < MAX){
                int pos = sent;
                for(; pos < MAX; pos++){
                    buff[pos - sent] = buff[pos];
                }
                size = MAX - sent;
            }
        }
    }
     //Now sending what is left in the buffer
    if(size != 0){     
        buff[size] = '\0';                
        write(socket_id, buff, sizeof(buff));
        size = 0;   
    }
}



//function for communication between client/server and node
void *socket_conversation(void *args){
	int socket_id = (int)args; 
    char buff[MAX]; 
    int n;  

    read(socket_id, buff, sizeof(buff)); 
    printf("Message Received : %s\n", buff);
    //Ensuring peer node is still active
    char *token = strtok(buff, ":");	// 1st token before :
    if(strcmp(token, "CHECK") == 0) {
    	printf("Closing Connection Socket.\n");
    	pthread_exit(NULL);
	}
	// message of form FILE:<filename>
	if(strcmp(token, "FILE") != 0){
		printf("Invalid Message\n");
		pthread_exit(NULL);
		return;
	}
	token = strtok(NULL, ":");	//This token consistes of FILENAME:<>

    printf("Client asking for : %s\n", token); 

    FILE *fptr = fopen(token,"r");	// opening the file

    // If unable to open the file (file is not present) then
    // Sending a 0 signifying file not present
    if(fptr == NULL){
        printf("No Such File Present\n");
        strcpy(buff, "0");	
        write(socket_id, buff, sizeof(buff)); 
    }
    else{	//This case is for when the file is found
        printf("File successfully found\n");

		fseek(fptr, 0L, SEEK_END);
		int size = ftell(fptr);
		rewind(fptr);

		sprintf(buff, "1:%d", size);
        write(socket_id, buff, sizeof(buff)); 
        printf("Sending File...\n");
        send_file(fptr, socket_id);
        fclose(fptr);				// closing the filepointer
    }
    printf("Closing Connection Socket.\n");
    close(socket_id);	// closing the socket and thread for this connection
    pthread_exit(NULL);
} 



int main(int argc,char* argv[]){
	char serv_ip[500];
	int serv_port = str_to_int(argv[2]);
	size_t sz = sizeof argv[1];
	safe_strcpy(serv_ip,sz+2,argv[1]);
	

	int sock_id, serv_sock_id; 
    struct sockaddr_in serv_addr, cli_addr, node_addr; 
    int len = sizeof(node_addr);

    sock_id = socket(AF_INET,SOCK_STREAM,0);
    if(sock_id<0){
    	perror("Socket failed\n");
    	exit(EXIT_FAILURE);
    }
    else
    	printf("Socket is successfully created for connection\n");

    serv_sock_id = socket(AF_INET,SOCK_STREAM,0);
    if(serv_sock_id==-1){
    	perror("Server socket failed\n");
    	exit(EXIT_FAILURE);
    }

    else
    	printf("Server socket successfully created\n");

    bzero(&serv_addr, sizeof(serv_addr));

    //Setting IP and Port of the server for the connection
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip); 
    serv_addr.sin_port = htons(serv_port); 


    //Node listening for server
    if ((listen(serv_sock_id, 10)) != 0) { 
        perror("Error in Listen\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
        printf("Node listening for server\n");

    //Getting port of the serv_sock created
    if (getsockname(serv_sock_id, (struct sockaddr *)&node_addr, &len) == -1)
    	perror("getsockname");
	else
    	printf("port number %d\n", ntohs(node_addr.sin_port));

    //Connecting to the server socket
    if (connect(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) !=0) { 
        perror("Connection with server failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
        printf("Connected to server\n"); 

    connect_server(sock_id,ntohs(node_addr.sin_port));
    printf("Closing connection to server\n");
    close(sock_id);
    

    while(1){
    	len = sizeof(cli_addr);
    	int new_sock_id = accept(serv_sock_id, (struct sockaddr *)&cli_addr, &len);
    	if(new_sock_id <0){
    		perror("Not accepted");
    		exit(EXIT_FAILURE);
    	}
    	else{
    		printf("Message from Client/server accepted\n");
    	}

    	pthread_t p_id;
        pthread_create(&p_id, NULL, socket_conversation, (void*)new_sock_id);

    }

    close(serv_sock_id);
    printf("Closing socket which was listening to server/client\n");
    return 0;

}