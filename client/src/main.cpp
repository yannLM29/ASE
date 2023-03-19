/**
 * @file main.cpp
 * @author Yann Le Masson
 * 
 */
#include "player_list.hpp"
#include "server_link.hpp"
#include <chrono>

int main()
{
    ASE::init();
    ASE::ServerLink<int> serveur;

    serveur.onDisconnectLambda = [](ASE::ServerLink<int> &server_link)
    {
        std::cout << "Disconnected !\n";
    };

    serveur.onKickLambda = [](ASE::ServerLink<int> &server_link)
    {
        std::cout << "Kicked !\n";
    };

    serveur.connectLink("127.0.0.1", 25565,nullptr,0);
    std::cout << "got id: " << serveur.getMyId() << "\n";

    while (true)
    {


        try
        {
            serveur.sendData();   
        }
        catch(RemoteConnectionException& e)
        {
            break;
        }

        try
        {

            ASE::Frame received = serveur.recvData();

        }    
        catch(RemoteConnectionException& e)
        {
            break;
        }
        
    }
    

    serveur.closeConnection();
    ASE::end();

    return 0;
}