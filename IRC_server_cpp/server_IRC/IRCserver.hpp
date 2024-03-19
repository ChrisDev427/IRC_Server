/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/16 18:51:14 by chris             #+#    #+#             */
/*   Updated: 2024/03/17 19:04:03 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "policeColor.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <istream>
#include <fstream>
#include <map>
#include <chrono>
#include <ctime>
#include <csignal>

#define CONNECTION_MAX 100
#define PORT 16000

class IRCserver {

public:
    IRCserver();
    ~IRCserver();
private:
    // Functions
    bool        errorEof();
    void        launchServer();
    void        ft_listen();
    void        ft_run();
    void        ft_accept();
    void        ft_recv();
    void        printLogin(std::string clientName);
    void        printLogout(std::string clientName);
    void        printLoggedClients(int client);
    std::string getTime();
    void        writeToClients( std::string clientName );
    bool        getClientName(int newConn, std::string & clientName);
    void        inputThread();
    static void sigintHandler(int signum);
    // Attributes
    std::ofstream                       ofs;
    std::string                         input;
    int 	                            sockfd;
    int 	                            id;
	int 	                            connectedClients;
    int                                 maxfd;
    std::string                         message;
    std::map<std::string, int>          clients;
    std::map<std::string, std::string>  timeDate;
    fd_set 	                            readfds;
	socklen_t                           len;
	struct sockaddr_in                  servaddr, cli;
};