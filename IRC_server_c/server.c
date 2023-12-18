/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 09:58:05 by chris             #+#    #+#             */
/*   Updated: 2023/12/16 08:35:58 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef struct  s_data {
    
    int 	port;
    int 	sockfd;
    int 	id;
	int 	connectedClients;
	int 	connMax;
    fd_set 	readfds;

	socklen_t len;
	struct sockaddr_in servaddr, cli;
}               t_data;

typedef struct  s_clients {
    
	int id;
	int connfd;
}               t_clients;

char 	*str_join(char *buf, char *add);
int 	find_nl( char* s );
void 	serverLaunch( t_data *data );
void 	printLogin( t_clients* client, int clientID, int connectionMax );
void 	printLogout( t_clients* client, int clientID, int connectionMax );
void 	writeToClients( t_clients *client, char* message, int clientID, int connectionMax );
void 	ft_run(t_data* data, t_clients* client);
void 	ft_accept( t_data* data, t_clients* client );
void 	ft_recv( t_data* data, t_clients* client );


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

void serverLaunch( t_data *data ) {

    // socket create and verification 
	data->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
	if (data->sockfd == -1) { 
		write(2, "Fatal error\n", 12); 
		close(data->sockfd);
		exit(1); 
	} 
	else
		// printf("Socket successfully created..\n"); 
	bzero(&data->servaddr, sizeof(data->servaddr)); 
	// assign IP, PORT 
	data->servaddr.sin_family = AF_INET; 
	data->servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // mon adresse IP
	// data->servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 -- (127 * 256) + (0 * 256) + (0 * 256) + (1 * 256) = 2130706433
	data->servaddr.sin_port = htons(data->port); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(data->sockfd, (const struct sockaddr *)&data->servaddr, sizeof(data->servaddr))) != 0) { 
		write(2, "Fatal error bind\n", 17);
		close(data->sockfd);
		exit(1); 
	} 
	else
		printf("Socket successfully binded..\n");
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

void printLogin( t_clients* client, int clientID, int connectionMax ) {

	char prefix[50];
	sprintf(prefix, "server: client %d just arrived\n", clientID);
    for ( int i = 0; i < connectionMax; i++ ) {
        if ( client[i].id != clientID && client[i].id != -1  )
            send( client[i].connfd, prefix, strlen(prefix), 0 );
	}
	memset(prefix, 0, 50);
}

void printLogout( t_clients* client, int clientID, int connectionMax ) {

	char prefix[50];
	sprintf(prefix, "server: client %d just left\n", clientID);
    for ( int i = 0; i < connectionMax; i++ ) {
        if ( client[i].id != clientID && client[i].id != -1  )
            send( client[i].connfd, prefix, strlen(prefix), 0 );
	}
	memset(prefix, 0, 50);
}

void writeToClients( t_clients* client, char* message, int clientID, int connectionMax ) {

	if (find_nl(message) == 0) {
		return;
	}

	char prefix[50];
	sprintf(prefix, "client %d: ", clientID);

    for ( int i = 0; i < connectionMax; i++ ) {

        if ( client[i].id != clientID && client[i].id != -1 ) {
			
			int j = 0;
			
            while ( message[j] ) {

				send( client[i].connfd, prefix, strlen(prefix), 0 );

				while ( message[j] != '\n' ) {
					
					send( client[i].connfd, &message[j], 1, 0 );
					j++;
				}
				send( client[i].connfd, "\n", 1, 0 );
				
				j++;
				if ( find_nl(message + j) == 0 ) {
					break;
				}
			}
		}
	}
    memset(prefix, 0, 50);
}

void ft_accept( t_data* data, t_clients* client ) {

	// Vérifier s'il y a une nouvelle connexion
    if (FD_ISSET(data->sockfd, &data->readfds)) {
        
		data->len = sizeof(data->cli);
		int newConnection = accept(data->sockfd, (struct sockaddr *)&data->cli, &data->len);
    	// fcntl(newConnection, F_SETFL, O_NONBLOCK); // TEST
		
		if (newConnection < 0) { 
    	    printf("server acccept failed...\n"); 
    	} 
    	else {
			int i = 0;
			for ( ; i < data->connMax; i++) {
				if ( client[i].id == -1 ) {
					client[i].id = data->id;
					data->id++;
					client[i].connfd = newConnection;
					break;
				}
			}
			if ( i < data->connMax ) {
				data->connectedClients++;
				puts("Server accept new client.");
				printLogin(client, client[i].id, data->connMax);
			}
			else {
				close(newConnection);
			}
		}
    }

}

void ft_recv( t_data* data, t_clients* client ) {

	// puts("server recv");
	// Vérifier les clients existants pour la lecture
    for (int i = 0; i < data->connMax; i++) {
		
        if (client[i].connfd != -1 && FD_ISSET(client[i].connfd, &data->readfds)) {
            // Code pour lire des données du client, similaire à votre exemple
			int bufferSize = 4096;
            char buffer[bufferSize];
			char *message = calloc(1, sizeof(char));
			int bytesRead;
			while (1) {
				
				bytesRead = 0;
    			bytesRead = recv(client[i].connfd, buffer, sizeof(buffer), 0);
				// printf("read = %d\n", bytesRead);
				buffer[bytesRead] = '\0';
				
				message = str_join(message, buffer);
				// printf("message = %s\n", message);
				memset(buffer, 0, bufferSize);
    			if (bytesRead < 0) {
					printf("recv failed\n");
					continue;
				}
				if ( bytesRead == 0 ) {
				
					writeToClients(client, message, client[i].id, data->connMax);
					free(message);
					printLogout(client, client[i].id, data->connMax);
					close(client[i].connfd);
					client[i].connfd = -1;
					client[i].id = -1;
					data->connectedClients--;
					break;
				}
				if ( message[strlen(message) -1] == '\n' ) {
					memset(buffer, 0, bufferSize);
					break;
				}
			}
			if(bytesRead > 0) {
				writeToClients(client, message, client[i].id, data->connMax);
				free(message);
    		}
        }
    }

}

void ft_run(t_data* data, t_clients* client) {

	while (1) {

		// Si il n'y a aucun client connecté, le premier client aura l'id: 0
		if ( data->connectedClients == 0 ) 
			data->id = 0;

    	// Initialiser les descripteurs pour la lecture
    	FD_ZERO(&data->readfds);

		// Ajouter le socket du serveur
    	FD_SET(data->sockfd, &data->readfds);

    	int maxfd = data->sockfd;

    	// Ajouter les descripteurs des clients connectés
    	for (int i = 0; i < data->connMax; i++) {
    	    if (client[i].connfd != -1) {
    	        FD_SET(client[i].connfd, &data->readfds);
    	        if (client[i].connfd > maxfd) {
    				maxfd = client[i].connfd;
				}
    	    }
    	}

    	// Utiliser select pour surveiller les descripteurs de fichiers
    	if ( select(maxfd + 1, &data->readfds, NULL, NULL, NULL) < 0 ) {
    	    // Gestion de l'erreur de select
			write(2, "Fatal error\n", 12);
    	    exit(1);
    	}

    	// Vérifier s'il y a une nouvelle connexion
		ft_accept( data, client );

    	// Vérifier les clients existants pour la lecture
		ft_recv( data, client );
	}

}
int main(int ac, char**av) {

	if ( ac != 2 ) {
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}

	t_data 		data;
	data.connectedClients = 0;
	data.connMax = 100;
	data.port = atoi(av[1]);
	if (data.port < 1024 || data.port > 65535 ) {
		write(2, "Fatal error\n", 12);
		exit(1);
	}

	t_clients 	client[data.connMax];
	for (int i = 0; i < data.connMax; i++) {
		client[i].connfd = -1;
		client[i].id = -1;
	}
	
	serverLaunch( &data );

	if (listen(data.sockfd, data.connMax) != 0) {
		write(2, "Fatal error\n", 12); 
		exit(1); 
	}
	ft_run ( &data, client );

	return 0;
	
}