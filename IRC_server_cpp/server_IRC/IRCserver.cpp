/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/16 18:51:25 by chris             #+#    #+#             */
/*   Updated: 2023/12/18 08:05:46 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCserver.hpp"

IRCserver::IRCserver(){

    id = 0;
    connectedClients = 0;
   
    launchServer(port);
    ft_listen();
    ft_run();
    
}
IRCserver::~IRCserver( void ){

}

void IRCserver::launchServer(size_t port) {

    // socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
	if (sockfd == -1) { 
		close(sockfd);
        throw std::logic_error(B_RED "Error: socket init failed." RESET);
	} 
	else {
		std::cout << B_GREEN "Socket successfully created..." RESET << std::endl;
    }
	bzero(&servaddr, sizeof(servaddr)); 
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // mon adresse IP
	servaddr.sin_port = htons(PORT); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		close(sockfd);
        throw std::logic_error(B_RED "Error: bind failed." RESET);
	} 
	else {
		std::cout << B_GREEN "Socket successfully binded - port::" << PORT << RESET << std::endl;
    }

}

void IRCserver::ft_listen() {

    if (listen(sockfd, CONNECTION_MAX) != 0) {
		close(sockfd);
        throw std::logic_error( B_RED "Error: listen failed." ST RESET);
	}
    else {
        std::cout << IT B_GRAY "IRCServer listenning...\n" ST RESET << std::endl;
    }
}

void IRCserver::ft_run() {

    while (1) {

		// Si il n'y a aucun client connecté, le premier client aura l'id: 0
		if ( connectedClients == 0 ) 
			id = 0;
    	// Initialiser les descripteurs pour la lecture
    	FD_ZERO(&readfds);
		// Ajouter le socket du serveur
    	FD_SET(sockfd, &readfds);

    	maxfd = sockfd;

    	// Ajouter les descripteurs des clients connectés
    	for ( std::map<std::string, int>::iterator it = clients.begin(); it != clients.end(); it++ ) {
    
    	    FD_SET(it->second, &readfds);
    	    if (it->second > maxfd) {
    			maxfd = it->second;
			}
    	}
    	
    	// Utiliser select pour surveiller les descripteurs de fichiers
    	if ( select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0 ) {
    	    // Gestion de l'erreur de select
			write(2, "Fatal error\n", 12);
    	    exit(1);
    	}
       
    	// Vérifier s'il y a une nouvelle connexion
		ft_accept();

    	// Vérifier les clients existants pour la lecture
		ft_recv();
	}
}

void IRCserver::ft_accept() {

        
    // Vérifier s'il y a une nouvelle connexion
    if (FD_ISSET(sockfd, &readfds)) {
        
		len = sizeof(cli);
		int newConnection = accept(sockfd, (struct sockaddr *)&cli, &len);
		
		if (newConnection < 0) { 
    	    printf("server acccept failed...\n"); 
    	} 
    	else {
            // std::string newClient = "Chris" + std::to_string(id);
            std::string clientName;
			while(getClientName(newConnection, clientName) == false) {}
		
            id++;
            clients[clientName] = newConnection;
            connectedClients++;
			
            std::cout << IT B_ORANGE "Client: " << clientName << " connected..." ST RESET << std::endl;
            std::cout << IT B_ORANGE "Connected clients [" << connectedClients << "]" ST RESET << std::endl << std::endl;
            printLogin(clientName);
		}
    }
}

bool IRCserver::getClientName( int newConn, std::string & clientName ) {

	while(1) {

    	// Code pour traiter la réception de données du serveur
    	char clientResponse[256];
    	ssize_t bytesRead = recv(newConn, clientResponse, sizeof(clientResponse) - 1, 0);
    	if (bytesRead <= 0) {
    	    // Gestion de la déconnexion du serveur
    	    throw std::logic_error("Error: recv");
    	}
    	clientResponse[bytesRead] = '\0';
    	std::map<std::string, int>::iterator it = clients.find(clientResponse);
		if( it == clients.end() ) {
			clientName = clientResponse;
			send(newConn, clientResponse, strlen(clientResponse), 0);

			return true;
		}
		else {
			std::string response = "doubleName";
			send(newConn, response.c_str(), response.size(), 0);
			memset(clientResponse, 0, 256);
			return false;
		}
		

	}
}

void IRCserver::ft_recv() {

	// Vérifier les clients existants pour la lecture
    std::map<std::string, int>::iterator it = clients.begin();

    for ( ; it != clients.end(); it++ ) {
		
        if (FD_ISSET(it->second, &readfds)) {
            // Code pour lire des données du client, similaire à votre exemple
			int bufferSize = 4096;
            char buffer[bufferSize];
			
			int bytesRead;
			while (1) {
				
				bytesRead = 0;
    			bytesRead = recv(it->second, buffer, sizeof(buffer), 0);
				// printf("read = %d\n", bytesRead);
				buffer[bytesRead] = '\0';
				
				message = message + buffer;
				// printf("message = %s\n", message);
				memset(buffer, 0, bufferSize);
    			if (bytesRead < 0) {
					printf("recv failed\n");
					continue;
				}
				if ( bytesRead == 0 ) {
                    
					printLogout(it->first);
					connectedClients--;
                    std::cout << IT B_ORANGE "Client: " << it->first << " disconnected..." ST RESET << std::endl;
            		std::cout << IT B_ORANGE "Connected clients [" << connectedClients << "]" ST RESET << std::endl << std::endl;

					close(it->second);
					clients.erase(it);
					return;
				}
				if ( message.back() == '\n' ) {
					break;
				}
			}
			if(bytesRead > 0) {
				writeToClients(it->first);
				message.clear();
    		}
        }
    }
}

void IRCserver::printLogin(std::string clientName) {
    
    std::map<std::string, int>::iterator it = clients.begin();
    std::string announce = "server: " + clientName + " logged in\n";
    for ( ; it != clients.end(); it++ ) {
        if( it->first != clientName) {
            send( it->second, announce.c_str(), announce.size(), 0 );   
        }
    }
    announce.clear();
}

void IRCserver::printLogout(std::string clientName) {

    std::map<std::string, int>::iterator it = clients.begin();
    std::string announce = "server: " + clientName + " logged out\n";
    for ( ; it != clients.end(); it++ ) {
        if( it->first != clientName) {
            send( it->second, announce.c_str(), announce.size(), 0 );   
        }
    }
    announce.clear();
}

// void IRCserver::writeToClients( std::string clientName ) {

// 	if (message.find('\n') == std::string::npos) {
// 		return;
// 	}
//     std::string prefix = clientName + ": ";

//     std::map<std::string, int>::iterator it = clients.begin();
//     for ( ; it != clients.end(); it++ ) {

//         if( it->first != clientName) {

// 		    int i = 0;
//             while ( message[i] ) {
// 		    	send( it->second, prefix.c_str(), prefix.size(), 0 );
// 		    	while ( message[i] != '\n' ) {
                
// 		    		send( it->second, &message[i], 1, 0 );
// 		    		i++;
// 		    	}
// 		    	send( it->second, "\n", 1, 0 );
    
// 		    	i++;
//                 if (message.find('\n', i) == std::string::npos) {
// 		            break;
// 	            }
// 		    }
//         }
// 	}
// 	prefix.clear();
// }
void IRCserver::writeToClients( std::string clientName ) {

    std::string toSend = clientName + ": " + message;
    std::map<std::string, int>::iterator it = clients.begin();
    for ( ; it != clients.end(); it++ ) {

        if( it->first != clientName) {
		    send( it->second, toSend.c_str(), toSend.size(), 0 );
		}
	}
	toSend.clear();
}
// void    IRCserver::askClientName(std::string& clientName) {

//     while ( true ) {
        
//         std::cout << "Please, enter the name you want to use into this channel: ";
        
//         std::getline(std::cin, clientName);
//         if (errorEof( clientName ) == true ) {
            
//             continue;
//         }
//         if (clientName.empty()) {
//             std::cout << RED << "No input provided." << RESET << std::endl;
//             continue;
//         }
//         else {
//             return;
//         }
//     }
// }


// bool    IRCserver::errorEof( std::string& str ) {

//     if ( std::cin.eof() ) {

//         str.clear();
//         std::cin.clear();
//         std::clearerr( stdin );
//         std::cout << RED << "\nInvalid input." << RESET << std::endl;
//         return true;
//     }
//     return false;
// }