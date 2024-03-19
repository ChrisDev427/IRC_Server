/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCclient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/16 18:59:10 by chris             #+#    #+#             */
/*   Updated: 2024/03/17 13:06:09 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCclient.hpp"



IRCclient::IRCclient() {
    ft_connect();
    ft_run();
}
IRCclient::~IRCclient( void ) {}

void    IRCclient::ft_connect() {

    // char buffer[bufferSize];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        throw std::invalid_argument("Error: socket init failed.");
    }

    // Set server address details
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) <= 0) {
        
        throw std::invalid_argument("Error: invalid server address.");
    }

    // Connect to the server
    std::cout << B_GRAY "Connecting to server..." RESET << std::endl;
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    
        throw std::invalid_argument(B_RED "Server is not running..." RESET);
    }
    std::cout << B_GREEN "Done." RESET << std::endl;
}

void    IRCclient::ft_run() {

    while (1) {
        if( clientName.empty()) {
              
            askClientName(clientName);
            std::cout << B_GREEN "Wellcome !\nYou are connected to IRC-Server as: " << clientName << RESET << std::endl << std::endl;
        }
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);  // Ajoutez stdin à l'ensemble
        FD_SET(sockfd, &readfds);        // Ajoutez le socket à l'ensemble

        int ready = select(sockfd + 1, &readfds, NULL, NULL, NULL);
        if (ready < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        handleUserInput();
        handleServerResponse();
    }
}

void    IRCclient::handleUserInput() {
    
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
        // Code pour traiter l'entrée utilisateur
        char inputBuffer[BUFFERSIZE];
        std::string toSend;
        std::string tmp;
        while(true){

            fgets(inputBuffer, sizeof(inputBuffer), stdin);
    
            if( strcmp(inputBuffer, ":quit\n") == 0){
                system("clear");
                std::cout << B_GREEN "You are disconnected from the server." RESET << std::endl;
                close(sockfd);
                exit(EXIT_SUCCESS);
            }

            // Send user input to the server
            if(strcmp(inputBuffer, "\n") == 0 && toSend.empty()) {
                memset(inputBuffer, 0, BUFFERSIZE);
                break;
            }
            tmp = inputBuffer;
            if(!toSend.empty()) {
                tmp.insert(0, clientName.size() +2, ' ');
            }
            if(strcmp(inputBuffer, "\n") != 0)
                toSend += tmp;
                
            if( strcmp(inputBuffer, ":log\n") == 0){
                
                send(sockfd, toSend.c_str(), toSend.size(), 0);
                toSend.clear();
                tmp.clear();
                memset(inputBuffer, 0, BUFFERSIZE);
                return;
            }
            if(strcmp(inputBuffer, "\n") == 0) {

                send(sockfd, toSend.c_str(), toSend.size(), 0);
                toSend.clear();
                tmp.clear();
                memset(inputBuffer, 0, BUFFERSIZE);
                return;
            }
            else {
                continue;
            }
        }
    }
}

void    IRCclient::handleServerResponse() {
    
    if (FD_ISSET(sockfd, &readfds)) {
        // Code pour traiter la réception de données du serveur
        char buffer[BUFFERSIZE];
        std::string message;
        ssize_t bytesRead;

        while(true) {
            bytesRead = 0;
            ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
            buffer[bytesRead] = '\0';
            if (bytesRead <= 0) {
                std::cout << "Error: the server is turned off" << std::endl;
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            message += buffer;
            if ( message.back() == '\n' ) {
				break;
			}
        }
        std::cout << B_BLUE << message << RESET << std::endl;
        message.clear();
    }
}

void    IRCclient::askClientName(std::string& clientName) {

    while ( true ) {
        std::cout << B_GRAY "Please, enter the name you want to use into this channel: " RESET;
        std::getline(std::cin, clientName);
        if (errorEof( clientName ) == true ) {
            continue;
        }
        if (clientName.empty()) {
            std::cout << RED << "No input provided." << RESET << std::endl;
            continue;
        }
        else {
            send(sockfd, clientName.c_str(), clientName.size(), 0);
            char serverResponse[BUFFERSIZE];
            ssize_t bytesRead = recv(sockfd, serverResponse, sizeof(serverResponse) - 1, 0);
            if (bytesRead <= 0) {
                // Gestion de la déconnexion du serveur
                return;
            }
            serverResponse[bytesRead] = '\0';
            if( strcmp(serverResponse, "doubleName") == 0 ) {
                std::cout << "The name you choose is already used !" << std::endl;
                memset(serverResponse, 0, BUFFERSIZE);
                clientName.clear();
                continue;
            }
            return;
        }
    }
}

bool    IRCclient::errorEof( std::string& str ) {

    if ( std::cin.eof() ) {

        str.clear();
        std::cin.clear();
        std::clearerr( stdin );
        std::cout << RED << "\nInvalid input." << RESET << std::endl;
        return true;
    }
    return false;
}