/**
 * @file server.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef SERVER_HPP
#define SERVER_HPP

#include <functional>
#include <shared_mutex>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

#include "cross_sockets.hpp"
#include "client_list.hpp"
#include "welcome_thread_functions.hpp"
#include "frame.hpp"
#include "internal_message.hpp"

namespace ASE
{   
    // ~~~~~~~~~~~~~~~~ Declarations ~~~~~~~~~~~~~~~~
    template<typename ClientDataStructure,typename ServerDataStructure>
    class Server;

    template<typename ClientDataStructure,typename ServerDataStructure>
    void ClientRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref, int my_id);

    template<typename ClientDataStructure,typename ServerDataStructure>
    void WelcomeRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref);

    template<typename ClientDataStructure,typename ServerDataStructure>
    void MainServeurRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref);

    template<typename ClientDataStructure,typename ServerDataStructure>
    void LocalInputRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref);




    // ~~~~~~~~~~~~~~~~ Server Class ~~~~~~~~~~~~~~~~
    template<typename ClientDataStructure,typename ServerDataStructure>
    class Server
    {
    private:

        SOCKET server_socket;
        SOCKADDR_IN server_socket_addr_in;

        // ~~~~ Welcome Thread ~~~~
        std::thread welcome_thread;
        
        bool welcome_thread_running;
        std::mutex welcome_thread_running_mutex;

        bool welcome_thread_welcoming;
        std::mutex welcome_thread_welcoming_mutex;



        // ~~~~ Local Input Thread ~~~
        std::thread local_input_thread;
        bool local_input_thread_running_cond;
        std::mutex local_input_thread_running_cond_mutex;


        // ~~~~ Remote Input Thread ~~~
        std::thread remote_input_thread;
        bool remote_input_thread_running_cond;
        std::mutex remote_input_thread_running_cond_mutex;

        // ~~~~ Main Thread ~~~
        std::thread main_thread;
        bool main_thread_running_cond;
        std::mutex main_input_thread_running_cond_mutex;




        ServerDataStructure global_data_;
        std::shared_mutex global_data_lock_;

        ClientList<ClientDataStructure> client_list_;
        // std::shared_mutex client_list_lock_;

        


    public:
        std::function<void(Server<ClientDataStructure,ServerDataStructure> &server_ref)> global_routine_lambda; 
        std::function<void(Server<ClientDataStructure,ServerDataStructure> &server_ref, const std::vector<uint8_t> &data, int my_id)> client_routine_data_recv_lambda;
        std::function<void(Server<ClientDataStructure,ServerDataStructure> &server_ref, Frame &to_send, int my_id)> client_routine_data_to_send_lambda;   

        std::function<std::tuple<bool, Message>(Server<ClientDataStructure,ServerDataStructure> &server_ref, Message client_message, SOCKADDR_IN client_addr_infos)> connection_control_lambda; 
        std::function<void(Server<ClientDataStructure,ServerDataStructure> &server_ref, Message &to_send)> init_client_and_prepare_package_to_send_lambda; 
        std::function<void(Server<ClientDataStructure,ServerDataStructure> &server_ref, const std::vector<uint8_t> &data, std::vector<uint8_t> &to_send)> disconnection_lambda;

        std::function<void(Server<ClientDataStructure,ServerDataStructure> &server_ref, InternalMessage &custom_message)> internal_custom_message_analysis_lambda;

        int timeout_limit;
        int server_id;
        int number_max_of_remote_command_threads;
        int main_thread_delay_milli;
        
        

    public:
        /**
         * @brief Create a server, which must be started 
         * 
         */
        Server(/* args */)
        {
            welcome_thread_running = true;
            welcome_thread_welcoming = true;
            timeout_limit = 5;
            server_id = 0;
            number_max_of_remote_command_threads = 2;
            main_thread_delay_milli = 10;
        }
        // ~Server();

       

        // ~~~~~~~~~~ GET ~~~~~~~~~~

        /**
         * @brief Get the server socket
         * 
         * @return SOCKET 
         */
        inline SOCKET getSocket() const
        {
            return server_socket;
        }

        /**
         * @brief Get the Timeout Limit of server sockets
         * 
         * @return int 
         */
        inline int getTimeoutLimit() const
        {
            return timeout_limit;
        }

        /**
         * @brief Get the Welcome Thread Running bool
         * 
         * @return bool 
         */
        inline bool getWelcomeThreadRunning()
        {
            welcome_thread_running_mutex.lock();
            bool output = welcome_thread_running;
            welcome_thread_running_mutex.unlock();

            return output;
        }


        /**
         * @brief Get the Welcome Thread Welcoming bool
         * 
         * @return bool
         */
        inline bool getWelcomeThreadWelcoming()
        {
            welcome_thread_welcoming_mutex.lock();
            bool output = welcome_thread_welcoming;
            welcome_thread_welcoming_mutex.unlock();
        
            return output;
        }

        /**
         * @brief Get a ref to server clientlist, you should use an access function after
         * 
         * @param ClientList 
         */
        ClientList<ClientDataStructure>& getClientList()
        {
            return client_list_;
        }

        /**
         * @brief Get the number Of clients in client_list
         * 
         * @return int 
         */
        int getNumberOfClients()
        {
            return client_list_.getNumberOfClients();
        }

        /**
         * @brief Get main_thread_delay_milli
         * 
         * @return int milliseconds
         */
        int getMainDelayMilli()
        {
            return main_thread_delay_milli;
        }


        // ~~~~~~~~~~ SET ~~~~~~~~~~

        /**
         * @brief Set the Welcome Thread Running boolean
         * 
         * @param new_status 
         */
        inline void setWelcomeThreadRunning(bool new_status)
        {
            welcome_thread_running_mutex.lock();
            welcome_thread_running = new_status;
            welcome_thread_running_mutex.unlock();
        }

        /**
         * @brief Set the Welcome Thread Welcoming boolean
         * 
         * @param new_status 
         */
        inline void setWelcomeThreadWelcoming(bool new_status)
        {
            welcome_thread_welcoming_mutex.lock();
            welcome_thread_welcoming = new_status;
            welcome_thread_welcoming_mutex.unlock();
        }


        // ~~~~~~~~~~ LAMBDA ACCESS ~~~~~~~~~~

        /**
         * @brief Reading access to the server's user data <Thread Safe>
         * 
         * @param access_lambda Lambda closure inside ypu can read client's data
         */
        void accessGlogalDataReading(std::function<void(const ServerDataStructure& data)> access_lambda)
        {
            std::shared_lock<std::shared_mutex> reader_lock(global_data_lock_);
            access_lambda(global_data_);

        }

        /**
         * @brief Reading and writing access to the servers's user data <Thread Safe>
         * 
         * @param access_lambda Lambda closure inside ypu can modify client's data
         */
        void accessGlobalDataWriting(std::function<void(ServerDataStructure& data)> access_lambda)
        {
            std::unique_lock<std::shared_mutex> writer_lock(global_data_lock_);
            access_lambda(global_data_);

        }

        void accessClient(int client_id, std::function<void(Client<ClientDataStructure>& data)> access_lambda)
        {
            client_list_.getClientAccess(client_id,[&](auto &client){
                access_lambda(client);
            });
        }

        
        // ~~~~~~~~~~ INTERNAL MESSAGES ~~~~~~~~~~

        /**
         * @brief Send an internal message to all connected clients
         * 
         * @param msg 
         */
        void sendInternalMessageToAllClients(InternalMessage msg)
        {
            client_list_.forEach([&](auto &client){
                client.giveInternalMessage(msg);
            });
        }

        /**
         * @brief Send an internal message to the specified client
         * 
         * @param client_id Client that will received the message
         * @param msg Messag to send
         */
        void sendInternalMessageToClient(int client_id, InternalMessage msg)
        {
            client_list_.getClientAccess(client_id, [&](auto &client){
                client.giveInternalMessage(msg);
            });
        }

        /**
         * @brief Send an NEWCLIENT internal message to all connected clients with client_id
         * 
         * @param client_id 
         */
        void announceNewClient(int client_id)
        {
            sendInternalMessageToAllClients(CreateNewClientInternalMessage(client_id));
        }

        /**
         * @brief Send an REMOVECLIENT internal message to all connected clients with client_id
         * 
         */
        void announceRemoveClient(int client_id)
        {
            sendInternalMessageToAllClients(CreateRemoveClientInternalMessage(client_id));
        }

        



        // ~~~~~~~~~~ OTHER ~~~~~~~~~~

        /**
        * @brief Start the server, server threads must be lauch then
        * 
        */
        void Start(int port, int queue_length)
        {
            SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
            if(sock == INVALID_SOCKET)
            {
                std::cerr << "Error during server start: " << strerror(errno);
                exit(errno);
            }


            SOCKADDR_IN sin = { 0, 0, 0, 0 }; /* initialise la structure avec des 0 */


            sin.sin_addr.s_addr = htonl(INADDR_ANY); /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
            sin.sin_port = htons(port); /* on utilise htons pour le port */
            sin.sin_family = AF_INET;


            
            #ifdef WIN32
            DWORD timeout = timeout_limit * 1000;
            setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
            
            #elif defined(__linux__)
                struct timeval tv;
                tv.tv_sec = timeout_limit;
                tv.tv_usec = 0;
                setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
            #endif
            

            bind(sock, (SOCKADDR*)&sin, sizeof(sin));

            
            listen(sock, queue_length);

            server_socket = sock;
            server_socket_addr_in = sin;

            printf("ready listening...\n");

            
        }

        /**
         * @brief Close the server and all its threads except the local input, You need to use ^C afer
         * 
         */
        void CloseFromCommand()
        {
            std::string leave_message = "serveur closed";
            sendInternalMessageToAllClients(InternalMessage(KICK_YOU,std::vector<uint8_t>(leave_message.begin(), leave_message.end())));

            setWelcomeThreadRunning(false);
            closesocket(server_socket);
            welcome_thread.join();
        }

        /**
         * @brief Launch important server threads, must be use after start and after lambdas definitions
         * 
         */
        void launchThreads()
        {
            welcome_thread = std::move(std::thread(WelcomeRoutine<ClientDataStructure,ServerDataStructure>,std::ref(*this)));

            main_thread = std::move(std::thread(MainServeurRoutine<ClientDataStructure,ServerDataStructure>, std::ref(*this)));
            main_thread.detach();

            local_input_thread = std::move(std::thread(LocalInputRoutine<ClientDataStructure,ServerDataStructure>, std::ref(*this)));
            local_input_thread.detach();

            
            
        }

        void launchRemoteCommandThreads(std::function<void(Server<ClientDataStructure,ServerDataStructure> &server_ref)> remote_command_lambda)
        {

            std::thread remote_command_thread(remote_command_lambda, std::ref(*this));
            remote_command_thread.detach();

        }

    };



    // ~~~~~~~~~~~~~~~~ Classic Functions ~~~~~~~~~~~~~~~~

    /**
     * @brief Disconnect a client and aware all others of it
     * 
     * @tparam ClientDataStructure 
     * @tparam ServerDataStructure 
     * @param server_ref 
     * @param client_id client to disconnect
     */
    template<typename ClientDataStructure,typename ServerDataStructure>
    void disconnectClient(Server<ClientDataStructure,ServerDataStructure> &server_ref, int client_id)
    {
        server_ref.getClientList().getClientAccess(client_id,[&](auto &client){
            closesocket(client.getSocket());
        });

        server_ref.getClientList().removeClient(client_id);
        server_ref.announceRemoveClient(client_id);

    }







    // ~~~~~~~~~~~~~~~~ Routines Functions ~~~~~~~~~~~~~~~~

    /**
     * @brief Routine function of all client threads
     * 
     * @tparam ClientDataStructure 
     * @tparam ServerDataStructure 
     * @param server_ref 
     * @param my_id client's id
     */
    template<typename ClientDataStructure,typename ServerDataStructure>
    void ClientRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref, int my_id)
    {
        
        SOCKET my_socket = 0;
        server_ref.getClientList().getClientAccess(my_id,[&](auto &client){
            my_socket = client.getSocket();
        });


        while (true)
        {
            

            Frame to_send;
            Frame recv_from_client;
            
            try
            {

                recv_from_client = recvFrame(my_socket);
                
            }
            catch(const std::exception& e)
            {
                #if DEBUG
                std::cout << "error during client " << my_id << " data recv: "<< e.what() << "\n";
                #endif
                
                disconnectClient(server_ref, my_id);
                return;
            }
            
            const std::vector<Message> message_list_from_client = recv_from_client.getMessagesConstRef();
            std::vector<uint8_t> end_data_to_send;

            for(const Message &message : message_list_from_client)
            {
                
                switch (message.getHat())
                {
                case DATA:
                    server_ref.client_routine_data_recv_lambda(server_ref, message.getDataConstRef(), my_id);
                    break;
                
                case DISCONNECT:

                    #if DEBUG
                    std::cout << "client " << my_id << " asked for disconnection\n";
                    #endif

                    
                    server_ref.disconnection_lambda(server_ref, message.getDataConstRef(), end_data_to_send);

                    to_send.addMessage(Message(DISCONNECT, std::move(end_data_to_send)));
                    to_send.sendFrame(my_socket);


                    disconnectClient(server_ref, my_id);
                    return;
                    break;
                
                default:

                    std::cout << "client " << my_id << " sended wrong hat\n";
                    disconnectClient(server_ref, my_id);
                    return;

                    break;
                }
            }

            for(;;)
            {
                InternalMessage msg;
                server_ref.getClientList().getClientAccess(my_id, [&](auto &me){
                    msg = me.getFirstInternalMessage();
                });

                if(msg.getHat() == EMPTY)
                    break;


                switch (msg.getHat())
                {
                case NEWCLIENT:
                    to_send.addMessage(Message(OCONNECT,msg.getDataCopy()));
                    break;
                
                case REMOVECLIENT:
                    to_send.addMessage(Message(ODISCONNECT,msg.getDataCopy()));
                    break;

                case KICK_YOU:
                    to_send.addMessage(Message(KICK, msg.getDataCopy()));
                    to_send.sendFrame(my_socket);

                    disconnectClient(server_ref, my_id); 
                    return;
                    break;

                case CUSTOM:
                    server_ref.internal_custom_message_analysis_lambda(server_ref, msg);
                    break;
                
                default:
                    break;
                }
            }

            
            server_ref.client_routine_data_to_send_lambda(server_ref, to_send, my_id);
            try
            {
                
                to_send.sendFrame(my_socket);
                
            }
            catch(const std::exception& e)
            {
                #if DEBUG
                std::cout << "error during client " << my_id << " data send: "<< e.what() << "\n";
                #endif
                
                disconnectClient(server_ref, my_id);
                return;
            }

            
        }
        
        
    }

    /**
     * @brief Routine function of the Welcome thread
     * 
     * @tparam ClientDataStructure 
     * @tparam ServerDataStructure 
     * @param server_ref 
     */
    template<typename ClientDataStructure,typename ServerDataStructure>
    void WelcomeRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref)
    {

        while (server_ref.getWelcomeThreadRunning())
        {
            
            #if DEBUG
            std::cout << "New wait\n";
            #endif

            std::tuple<SOCKET,SOCKADDR_IN> accept_out;
            

            try
            {
                accept_out = waitClient(server_ref.getSocket());
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                return;
            }
            
            SOCKET client_socket = std::get<0>(accept_out);
            SOCKADDR_IN client_addr = std::get<1>(accept_out);

            #ifdef WIN32
            DWORD timeout = timeout_limit * 1000;
            setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
            
            #elif defined(__linux__)
                // No tcp delay, because we use recv for little data packages
                int yes = 1;
                setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&yes, sizeof(int));

                // Timeout
                struct timeval tv;
                tv.tv_sec = server_ref.timeout_limit;
                tv.tv_usec = 0;
                setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
            #endif


            if(client_socket < 0)
            {
                continue;       // error during connection, pass to next client waiting
            }

            #if DEBUG
            std::cout << "New connection\n";
            #endif

            Frame client_hello;
            try
            {
                client_hello = recvFrame(client_socket); 
            }
            catch(RemoteConnectionException& e)
            {
                std::cerr << e.what() << '\n';
                continue;
            }
            
              
            message_code client_hello_code = client_hello.getMessages()[0].getHat();

            if(!server_ref.getWelcomeThreadWelcoming())
            {
                // Server currently not accepting clients
                dontAcceptClient(client_socket, FULL);
                continue;
            }

            if(client_hello.getLength() != 1 || (client_hello_code != CONNECT && client_hello_code != CONNECTWINFO))
            {
                // Bad connect message from client
                dontAcceptClient(client_socket, BADCODATA);
                continue;
            }

            #if DEBUG
            std::cout << "Checking\n";
            #endif

            //lambda welcome decision
            auto [accept_client, message_to_client] =  server_ref.connection_control_lambda(server_ref,  client_hello.getMessages()[0], client_addr);
            if(!accept_client)
            {
                try
                {
                     refuseClient(client_socket, message_to_client);
                }
                catch(RemoteConnectionException& e)
                {
                    std::cerr << e.what() << '\n';
                }
                

                closesocket(client_socket);
                continue;
            }


            #if DEBUG
            std::cout << "Adding\n";
            #endif


            // ~~~~~ adding client in client list ~~~~~
            int new_client_id = 0;
            new_client_id = server_ref.getClientList().addClient(client_socket, "basic_client");
            
            // ~~~~~ Warning all clients of the arrival (even me)~~~~~
            server_ref.announceNewClient(new_client_id);


            // ~~~~~ Send intial datas to client ~~~~~
            Frame init_client_frame;

            // Create message with client's ids list
            std::vector<int> client_id_list = {};
            server_ref.getClientList().forEach([&](auto &client){
                client_id_list.emplace_back(client.getId());
            });

            Message client_id_list_to_send(COACCEPTED,client_id_list.data(), client_id_list.size() * sizeof(int));
            std::cout << "sending client_id_list = " << client_id_list_to_send.toString() << "\n";
            // Create custom user connect message
            Message init_user_msg_to_send(CODATA,{});
            server_ref.init_client_and_prepare_package_to_send_lambda(server_ref, init_user_msg_to_send) ;

            init_client_frame.addMessage(std::move(client_id_list_to_send));
            init_client_frame.addMessage(Message(YOURID, &new_client_id, sizeof(int)));
            init_client_frame.addMessage(std::move(init_user_msg_to_send));


            // Sending initial infos to client
            try
            {
                init_client_frame.sendFrame(client_socket);
            }
            catch(RemoteConnectionException& e)
            {
                std::cout << "Client disconnected during sending initial infos\n";
                server_ref.getClientList().removeClient(new_client_id);
                server_ref.announceRemoveClient(new_client_id);

                closesocket(client_socket);
                
                continue;
            }


            #if DEBUG
            std::cout << "Launch thread\n";
            #endif



            // ~~~~~ Launch of client thread ~~~~~
            std::thread new_client_thread(ClientRoutine<ClientDataStructure, ServerDataStructure>, std::ref(server_ref), new_client_id);
           
            server_ref.getClientList().getClientAccess(new_client_id, [&](auto &client){
                client.setThread(new_client_thread.get_id());
            });


            #if DEBUG
            std::cout << "Detach thread\n";
            #endif


            new_client_thread.detach();
            
        }
        
    }
    /**
     * @brief Main thread routine, all user game logic should be done here by lambda
     * 
     * @tparam ClientDataStructure 
     * @tparam ServerDataStructure 
     * @param server_ref 
     */
    template<typename ClientDataStructure,typename ServerDataStructure>
    void MainServeurRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref)
    {
        while(true)
        {
            server_ref.global_routine_lambda(server_ref);
            std::this_thread::sleep_for(std::chrono::milliseconds(server_ref.getMainDelayMilli()));
        }
    }


    /**
     * @brief Local input thread routine, get and execute admin's commands from cin
     * 
     * @tparam ClientDataStructure 
     * @tparam ServerDataStructure 
     * @param server_ref 
     */
    template<typename ClientDataStructure,typename ServerDataStructure>
    void LocalInputRoutine(Server<ClientDataStructure,ServerDataStructure> &server_ref)
    {
        std::string command;
        while(true)
        {
            std::cout << "Enter a command: ";
            std::getline(std::cin, command);

            
            if(command.compare("getlist") == 0)
            {   
                server_ref.getClientList().forEach([&](auto &client){
                    std::cout << client.getId() << ": " <<  client.getName() << "\n";
                }); 
            }
            else if(command.compare("stop") == 0)
            {
                server_ref.CloseFromCommand();
                return;
            }
            else if(command.compare("kick") == 0)
            {
                int id_to_kick;
                std::cout << "Enter id of client to kick: ";
                std::cin >> id_to_kick;

                server_ref.sendInternalMessageToClient(id_to_kick,InternalMessage(KICK_YOU, {}));
            }

            /**
             * TODO: Add custom commands ?
             * 
             */
            
        }
    }

    

    // ~~~~~~~~~~~~~~~~ Lambda Functions ~~~~~~~~~~~~~~~~



}



#endif