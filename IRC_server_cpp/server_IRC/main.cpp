/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 08:00:35 by chris             #+#    #+#             */
/*   Updated: 2024/03/17 12:35:24 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCserver.hpp"

int main() {

    system("clear");
    std::cout << B_CYAN << "\n\n   ******************" << std::endl;
    std::cout << "   *** IRC server ***" << std::endl;
    std::cout << "   ******************\n\n" << RESET << std::endl;
    
    try {
        
        IRCserver server;
    }
    catch( std::exception& e ) {
        std::cerr << e.what() << std::endl;
    }
   
	return 0;
}