/**
 * @file server_link.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef SERVER_LINK_HPP
#define SERVER_LINK_HPP

#include <exception>
#include <errno.h>
#include <iostream>
#include <cstring>
#include <functional>

#include "cross_sockets.hpp"
#include "connection_expections.hpp"
#include "frame.hpp"
#include "player_list.hpp"

namespace ASE
{

template <typename PlayerDataStructure>
class ServerLink;


template<typename PlayerDataStructure>
SOCKET connectToServer(std::string server_address, int port, void *connect_data, size_t connect_data_size, PlayerList<PlayerDataStructure> &player_list_ref, int &my_id)
{
    
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        throw ServerConnectionException(strerror(errno));
    }

    #ifdef WIN32
    DWORD timeout = timeout_limit * 1000;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    
    #elif defined(__linux__)
        int yes = 1;
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&yes, sizeof(int));
    #endif


    SOCKADDR_IN sin = { 0, 0, 0, 0 };


    sin.sin_addr.s_addr = inet_addr(server_address.c_str());
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;

    if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        throw ServerConnectionException(strerror(errno));
    }   

    Frame hello;
    if(connect_data_size == 0)
    {
        hello.addMessage(Message(CONNECT,nullptr,0));
    }
    else
    {
        hello.addMessage(Message(CONNECTWINFO, connect_data, connect_data_size));
    }


    hello.sendFrame(sock);      // not catched

    
    Frame server_answer = recvFrame(sock);
    std::cout << "recv from server " << server_answer.getLength() << " messages with "<< int(server_answer.getMessages()[0].getHat()) << " and " << int(server_answer.getMessages()[1].getHat()) << "\n";
    switch (server_answer.getMessages()[0].getHat())
    {
    case FULL:
        throw ServerFullException("full");
        break;

    case COREFUSED:
        throw ConnectionRefused("");
        break;

    case COACCEPTED:
        break;

    default:
        throw RemoteConnectionException("unknown");
        break;
    }
    
    /**
     * ! Maybe dangerous 
     */
    int *server_initial_infos = (int *)(server_answer.getMessages()[0].getRawData());       // attetion big/little endian

    for(int i = 0; i < server_answer.getMessages()[0].getSizeOfData()/4; i++)               // a rendre plus propre
    {
        player_list_ref.addPlayer(server_initial_infos[i]);
    }

    if(server_answer.getMessages()[1].getHat() != YOURID)
    {
        throw RemoteConnectionException("bad codata sended");
    }
    my_id = *(int *)(server_answer.getMessages()[1].getRawData());

    if(server_answer.getMessages()[2].getHat() != CODATA)
    {
        throw RemoteConnectionException("bad codata sended");
    }
    
    // lambda codata
    
    return sock;

}

void closeConnection(SOCKET sock)
{
    closesocket(sock);
}

// template<typename PlayerDataStructure>
// Frame recvDataFromServer(ServerLink, PlayerList<PlayerDataStructure> &player_list_ref)
// {
   
// }

template <typename PlayerDataStructure>
class ServerLink
{
private:
    SOCKET link_socket;
    PlayerList<PlayerDataStructure> all_players;
    Frame to_send;
    int my_id_;

public:
    std::function<void(ServerLink &server_link)> onDisconnectLambda;
    std::function<void(ServerLink &server_link)> onKickLambda;


public:
    ServerLink(/* args */)
    {

    }

    PlayerList<PlayerDataStructure> &getPlayerListRef()
    {
        return all_players;
    }

    /**
     * @brief Get an iterator to player selected by id
     * 
     * @param id 
     * @return Player Iterator
     */
    auto getPlayerItById(int id)
    {
        return all_players.getPlayerItById(id);
    }

    int getMyId()
    {
        return my_id_;
    }

    void connectLink(std::string server_address, int port, void *connect_data, size_t connect_data_size)
    {

        link_socket = connectToServer(server_address, port, connect_data, connect_data_size, all_players, my_id_);
    }

    Frame recvData()
    {
        to_send.clear();


        Frame received = recvFrame(link_socket);

        for(Message &message : received.getMessages())
        {
            int id;
            switch (message.getHat())
            {
            case OCONNECT:
                id = *((int *)(message.getRawData()));
                if(id != my_id_)
                {
                    std::cout << "New player !\n";
                    all_players.addPlayer(id);
                }
                break;
            
            case ODISCONNECT:
                id = *((int *)(message.getRawData()));
                std::cout << "Player disconnected\n";
                all_players.removePlayer(id);
                break;

            case KICK:
                onKickLambda(std::ref(*this));
                closesocket(link_socket);
                throw RemoteConnectionException("kicked");
                break;

            case DISCONNECT:
                onDisconnectLambda(std::ref(*this));
                closesocket(link_socket);
                throw RemoteConnectionException("disconnection");
                break;
            default:
                break;
            }
        }

    return received;
    }


    void addDataToSend(void *data, size_t data_size)
    {
        to_send.addMessage(Message(DATA, data, data_size));
    }

    void sendData()
    {
        to_send.sendFrame(link_socket);
    }

    void sendData(void *data, size_t data_size)
    {
        to_send.addMessage(Message(DATA, data, data_size));
        to_send.sendFrame(link_socket);
    }

    void closeConnection()
    {
        closesocket(link_socket);
    }
};


} // namespace ASE

#endif