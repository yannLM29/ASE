/**
 * @file client.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <queue>
#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <shared_mutex>


#include "cross_sockets.hpp"
#include "internal_message.hpp"

namespace ASE
{

    

    template<typename ClientDataStructure>
    class Client
    {
    private:
        static long int cpt_id;
        long int id_;
        std::thread::id thread_id_;
        SOCKET socket_;
        SOCKADDR addr_;

        // Pile des messages internes
        std::queue<InternalMessage> internal_messages_queue_;
        std::mutex internal_messages_queue_mutex_;

        // Data of client, that can be used by dev
        ClientDataStructure data_;
        std::shared_mutex data_lock_;

        std::string name_;


    public:

        /**
         * @brief Construct a new Client object
         * 
         * @param thread_id 
         * @param client_socket 
         * @param name 
         */
        Client(std::thread::id thread_id, SOCKET client_socket,const std::string name): thread_id_(thread_id), socket_(client_socket), name_(name)
        {
            id_ = cpt_id++;
        }

        /**
         * @brief Construct a new Client object
         * 
         * @param thread_id 
         * @param client_socket 
         * @param name 
         * @param out_id 
         */
        Client(std::thread::id thread_id, SOCKET client_socket, const std::string name, int &out_id): thread_id_(thread_id), socket_(client_socket), name_(name)
        {
            id_ = cpt_id++;
            out_id = id_;
        }

        /**
         * @brief Construct a new Client object
         * 
         * @param client_socket client's socket
         * @param out_id created client's id
         */
        Client(SOCKET client_socket, int &out_id): thread_id_(0), socket_(client_socket)
        {
            id_ = cpt_id++;
            name_ = "basic_user_" + std::to_string(id_);
            out_id = id_;
        }

        /**
         * @brief Get the Id
         * 
         * @return long int 
         */
        long int getId() const
        {
            return id_;
        }

        /**
         * @brief Get the Name
         * 
         * @return std::string 
         */
        std::string getName() const
        {
            return name_;
        }

        /**
         * @brief Set the Thread
         * 
         * @param id 
         */
        void setThread(std::thread::id id)
        {
            thread_id_ = id;
        }

        /**
         * @brief Get the Socket
         * 
         * @return SOCKET 
         */
        SOCKET getSocket()
        {
            return socket_;
        }

        /**
         * @brief Reading access to the client's user data <Thread Safe>
         * 
         * @param access_lambda Lambda closure inside ypu can read client's data
         */
        void accessDataReading(std::function<void(const ClientDataStructure&)> access_lambda)
        {
            std::shared_lock<std::shared_mutex> reader_lock(data_lock_);
            access_lambda(data_);

        }

        /**
         * @brief Reading and writing access to the client's user data <Thread Safe>
         * 
         * @param access_lambda Lambda closure inside ypu can modify client's data
         */
        void accessDataWriting(std::function<void(ClientDataStructure&)> access_lambda)
        {
            std::unique_lock<std::shared_mutex> writer_lock(data_lock_);
            access_lambda(data_);

        }


        /**
         * @brief Add internal message to his internal_message_queue <Thread Safe> A TESTER ! !
         * 
         * @param message Message to give
         */
        void giveInternalMessage(InternalMessage message)
        {
            internal_messages_queue_mutex_.lock();

                internal_messages_queue_.emplace(message);

            internal_messages_queue_mutex_.unlock();
        }


        /**
         * @brief Get the first internal message object  A TESTER ! !
         *        <Thread Safe>
         * 
         * @return InternalMessage or ?? if empty
         */
        InternalMessage getFirstInternalMessage()
        {
            InternalMessage output;
            internal_messages_queue_mutex_.lock();

                if(!internal_messages_queue_.empty())
                {
                    output = internal_messages_queue_.front();
                    internal_messages_queue_.pop();
                }
                else
                {
                    output = InternalMessage(EMPTY, {});
                }
                

            internal_messages_queue_mutex_.unlock();

            return output;
        }

        /**
         * @brief Create a string describing the client
         * TODO: think to a more precise presentation
         * 
         * @return std::string 
         */
        std::string internalMessageListToString()
        {
            std::string output = "";
            internal_messages_queue_mutex_.lock();
                output += "front = " + internal_messages_queue_.front().to_string();
            internal_messages_queue_mutex_.unlock();

            return output;
        }

    };

    
}

template<typename ClientDataStructure>
long int ASE::Client<ClientDataStructure>::cpt_id = 0;


#endif