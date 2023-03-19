/**
 * @file client_list.hpp
 * @author Yann Le Masson
 * 
 */

#ifndef CLIENT_LIST_HPP
#define CLIENT_LIST_HPP

#include "client.hpp"

#include <list>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <functional>

namespace ASE
{
    template<typename ClientDataStructure>
    class ClientList
    {
    private:
        std::list<Client<ClientDataStructure>> all_clients;
        std::shared_mutex all_clients_lock_;

    public:
        ClientList()
        {

        }
        
        ~ClientList()
        {
        
        }

        /**
         * @brief Get the number Of clients in all_clients list
         * 
         * @return int 
         */
        int getNumberOfClients()
        {
            std::shared_lock<std::shared_mutex> reader_lock(all_clients_lock_);
            return int(all_clients.size());
        }

        /**
         * @brief Create Client and add it in the client list <ThreadSafe>
         * 
         * @param thread_id id of client's thread (each client should have his own thread)
         * @param client_socket (int)
         * @param name client internal name
         */
        int addClient(std::thread::id thread_id, SOCKET client_socket,const std::string name)
        {
            std::unique_lock<std::shared_mutex> writer_lock(all_clients_lock_);

            int id = 0;
            all_clients.emplace_back(thread_id,client_socket,name,id);

            return id;
        }

        /**
         * @brief Create Client and add it in the client list <ThreadSafe>
         * 
         * @param client_socket (int)
         * @param name client internal name
         * @return int: id of the new client
         */
        int addClient(SOCKET client_socket,const std::string name)
        {
            std::unique_lock<std::shared_mutex> writer_lock(all_clients_lock_);

            int id = 0;
            all_clients.emplace_back(client_socket,id);

            return id;
        }




        /**
         * @brief Remove and disallocate client from client list <ThreadSafe>
         * 
         * @param client_id id of client to remove
         */
        void removeClient(int client_id)
        {
            std::unique_lock<std::shared_mutex> writer_lock(all_clients_lock_);

            auto client_to_remove_it = std::find_if(all_clients.cbegin(), all_clients.cend(), [&](auto& client){
                return client.getId() == client_id;
            });

            if(client_to_remove_it == all_clients.cend())
            {
                throw std::runtime_error("Error: this client doesn't exist");
            }

            all_clients.erase(client_to_remove_it);
        }


        /**
         * @brief Get the Access to a specified client and possibility to modify it <ThreadSafe>
         * 
         * @param client_id id of client to get
         * @param access_lambda lambda in where you can access client
         */
        void getClientAccess(int client_id, std::function<void(Client<ClientDataStructure>&)> access_lambda)
        {
            std::shared_lock<std::shared_mutex> reader_lock(all_clients_lock_);

            auto my_client = std::find_if(all_clients.begin(), all_clients.end(),[&](auto &client){
                return client.getId() == client_id;
            });

            if(my_client == all_clients.cend())
            {
                throw std::runtime_error("Error: this client doesn't exist");
            }
            access_lambda(*my_client);

        }

        /**
         * @brief Get the Access to all clients in a for each and have the possibility to modify it <ThreadSafe>
         * 
         * @param client_id id of client to get
         * @param access_lambda lambda in where you can access client
         */
        void forEach(std::function<void(Client<ClientDataStructure>&)> access_lambda)
        {
            std::shared_lock<std::shared_mutex> reader_lock(all_clients_lock_);

            for (auto client_it = all_clients.begin(); client_it != all_clients.end(); ++client_it)
            {
                access_lambda(*client_it);
            }


        }


        /**
         * @brief Create string with client list on format "-id name"  <ThreadSafe>
         * 
         * @return std::string 
         */
        std::string toString()
        {
            std::string output ="";
            std::shared_lock<std::shared_mutex> reader_lock(all_clients_lock_);

            for(auto client = all_clients.cbegin(); client != all_clients.cend(); ++client)
            {
                output += "- " + std::to_string(client->getId()) + " " + client->getName() + std::string("\n");
            }

            return output;
        }
       
    };
}


#endif