#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_ADDR "192.168.0.42"
#define SERVER_PORT 6500
#define bufferSize 1024

void error(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}
int find_nl(char* s) {

	int i = 0;
	while( s[i] ) {
		if(s[i] == '\n') {
			return 1;
		}
		i++;
	}
	return 0;
}
char *str_join(char *buf, char *add) {
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}
void sendToServer( int sockfd, char* message ) {

    puts(message);
	if (find_nl(message) == 0) {
		return;
	}
	
	int j = 0;
	
    while ( message[j] ) {
		while ( message[j] != '\n' ) {
			
			send( sockfd, &message[j], 1, 0 );
			j++;
		}
		send( sockfd, "\n", 1, 0 );
		
		j++;
		if ( find_nl(message + j) == 0 ) {
			break;
		}
	}
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    
        // char buffer[bufferSize];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        error("Socket creation failed");
    }

    // Set server address details
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) <= 0) {
        error("Invalid server address");
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error("Connection failed");
    }

    printf("You are connected to chmassa IRC-Server.\n");

    fd_set readfds;


    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);  // Ajoutez stdin à l'ensemble
        FD_SET(sockfd, &readfds);        // Ajoutez le socket à l'ensemble

        int ready = select(sockfd + 1, &readfds, NULL, NULL, NULL);
        
        if (ready == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (ready > 0) {
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                // Code pour traiter l'entrée utilisateur
                
                char inputBuffer[256];
                fgets(inputBuffer, sizeof(inputBuffer), stdin);
                if( strcmp(inputBuffer, "quit\n") == 0){
                    printf("You are disconnected from the server.\n");
                    break;
                }
                // Code pour envoyer inputBuffer au serveur
                // Send user input to the server
                send(sockfd, inputBuffer, strlen(inputBuffer), 0);
            }

            if (FD_ISSET(sockfd, &readfds)) {
                // Code pour traiter la réception de données du serveur
                char serverBuffer[256];
                ssize_t bytesRead = recv(sockfd, serverBuffer, sizeof(serverBuffer) - 1, 0);
                if (bytesRead <= 0) {
                    // Gestion de la déconnexion du serveur
                    break;
                }
                serverBuffer[bytesRead] = '\0';
                printf("%s", serverBuffer);
            }
       
        }

    }
    // Close the socket
    
    close(sockfd);

    return 0;
}
