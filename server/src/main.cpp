/**
 * @file main.cpp
 * @author Yann Le Masson
 * 
 */
#include <iostream>

#include <functional>
#include <memory>
#include <iostream>
#include <thread>

#include "client_list.hpp"
#include "cross_sockets.hpp"
#include "server.hpp"
#include "frame.hpp"
#include "message.hpp"
#include "internal_message.hpp"




int main(int argc, char const *argv[])
{

    ASE::init();

    ASE::Server<int,int> server;
    server.Start(25565,5);
    
    

    server.connection_control_lambda = [](ASE::Server<int,int> &server_ref, ASE::Message client_message, SOCKADDR_IN client_addr_infos)
    {
        std::tuple<bool, ASE::Message> test = {true, ASE::Message(ASE::DISCONNECT, std::vector<uint8_t>())};
        
        return test;
    };

    server.init_client_and_prepare_package_to_send_lambda = [](ASE::Server<int,int> &server_ref, ASE::Message client_init_message)
    {
        
    };

    server.client_routine_data_recv_lambda = [](ASE::Server<int,int> &server_ref, const auto &data, int my_id)
    {


    };

    server.internal_custom_message_analysis_lambda = [](ASE::Server<int,int> &server_ref, ASE::InternalMessage msg)
    {

    };

    server.client_routine_data_to_send_lambda = [](ASE::Server<int,int> &server_ref, auto &data, int my_id)
    {

    };

    server.global_routine_lambda = [](ASE::Server<int,int> &server_ref)
    {
        
    };

    server.disconnection_lambda = [](ASE::Server<int,int> &server_ref, auto &data, auto &to_send)
    {

    };

    server.launchThreads();

    while(true)
    {
        
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }


    ASE::end();
    return 0;
}