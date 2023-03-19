/**
 * @file welcome_thread_functions.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef WELCOME_THREAD_FUNCTIONS
#define WELCOME_THREAD_FUNCTIONS

#include <tuple>
#include <iostream>
#include <cstring>
#include <errno.h>

#include "cross_sockets.hpp"
#include "message_codes.hpp"
#include "frame.hpp"

namespace ASE
{

    /**
     * @brief Use accept to wait new connections, block the server after closing which is a problem
     * 
     * @param server_socket 
     * @return std::tuple<SOCKET,SOCKADDR_IN> 
     */
    std::tuple<SOCKET,SOCKADDR_IN> waitClient(SOCKET server_socket)
    {
        SOCKET client_socket = 0;
        SOCKADDR_IN client_addr = {0,0,0,0};
        size_t client_addr_len = sizeof(client_addr);

        client_socket = accept(server_socket, (SOCKADDR*)&client_addr, (socklen_t*) &client_addr_len);

        if(client_socket == SOCKET_ERROR)
        {
            #if DEBUG
                std::cout << strerror(errno) << "\n";
            #endif 


            throw ServerConnectionException(strerror(errno));
        }

        return {client_socket, client_addr};

    }

    /**
     * @brief Close the client socket after sending him a code with an explanation of why
     * 
     * @param client_socket client socket to close
     * @param reason_code can be FULL or BADCODATA
     */
    void dontAcceptClient(SOCKET client_socket, MessageCodes reason_code)
    {
        Frame to_send;
        Message infos(reason_code, std::vector<uint8_t>());
        to_send.addMessage(std::move(infos));

        try
        {
            to_send.sendFrame(client_socket);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        
        
        closesocket(client_socket);
    }

    /**
     * @brief Close the client socket after sending him a message with an explanation of why, 
     * this function is call if the connection control lambda refuse a client
     * 
     * @param client_socket client socket to close
     * @param reason custom message by the user
     */
    void refuseClient(SOCKET client_socket, Message reason)
    {
        Frame to_send;
        Message infos(COREFUSED, std::vector<uint8_t>());
        to_send.addMessage(std::move(infos));

        to_send.sendFrame(client_socket);
        
        closesocket(client_socket);
    }
}



#endif