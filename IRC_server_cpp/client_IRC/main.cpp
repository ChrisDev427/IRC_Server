/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/16 19:20:56 by chris             #+#    #+#             */
/*   Updated: 2024/03/15 09:56:28 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCclient.hpp"


static void printTitle(void) {
    
    system("clear");
    std::cout << B_CYAN << "\n\n   ******************" << std::endl;
    std::cout << "   *** IRC client ***" << std::endl;
    std::cout << "   ******************\n\n" << RESET << std::endl;
    
}

int main() {

	// if ( ac != 2 ) {
	// 	std::cout << "Wrong number of arguments" << std::endl;
	// 	exit(1);
	// }
    
    printTitle();
    try {
        IRCclient client;
    }
    catch( std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

	

	return 0;
	
}