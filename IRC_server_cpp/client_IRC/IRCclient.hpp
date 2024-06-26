/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCclient.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/16 18:52:09 by chris             #+#    #+#             */
/*   Updated: 2024/03/15 09:03:29 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "policeColor.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <map>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 16000
#define BUFFERSIZE 1024

class IRCclient {

public:

    IRCclient();
    ~IRCclient( void );

private:
    // IRCclient();
    IRCclient( int const n );
    IRCclient( IRCclient const & src );
    IRCclient & operator=( IRCclient const & rhs );

    // Functions
    void    ft_connect();
    void    ft_run();
    void    handleUserInput();
    void    handleServerResponse();
    bool    errorEof( std::string& str );
    void    askClientName(std::string& clientName);
    

    // Attributes
    int sockfd;
    struct sockaddr_in server_addr;
    fd_set readfds;
    std::string clientName;
    
    
    
};