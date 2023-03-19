/**
 * @file frame.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef FRAME_HPP
#define FRAME_HPP

#include <vector>
#include <array>
#include <string>
#include <stdint.h>
#include <utility>

#include "cross_sockets.hpp"
#include "message.hpp"
#include "message_codes.hpp"
#include "connection_expections.hpp"


namespace ASE
{

    class Frame
    {
        private:
            std::vector<Message> messages_;


        public:

            /**
             * @brief Create an empty Frame
             * 
             */
            Frame(/* args */);
            
            ~Frame();

            // ~~~~~~~~~~ GET ~~~~~~~~~~

            /**
             * @brief Get the length of the list of messages contained in the Frame
             * 
             * @return int 
             */
            inline int getLength() const
            {
                return int(messages_.size());
            }

            /**
             * @brief Get a ref of the vector messages 
             * 
             * @return std::vector<Message>& 
             */
            inline std::vector<Message>& getMessages()
            {
                return messages_;
            }

            /**
             * @brief Get a const ref of the vector messages 
             * 
             * @return const std::vector<Message>& 
             */
            inline const std::vector<Message> &getMessagesConstRef() const
            {
                return messages_;
            }





            /**
             * @brief Add a message at the end of the frame
             * 
             * @param message Message to had
             */
            void addMessage(Message message);

            /**
             * @brief Send the frame through receiver_socket
             * 
             * @param receiver_socket Socket of the receiver
             */
            void sendFrame(SOCKET receiver_socket);

            /**
             * @brief Reset Frame to empty
             * 
             */
            void clear();
            

            std::string toString();

            
    };
    
    /**
     * @brief Wait and receive a frame from a sender through sender_socket
     * 
     * @param sender_socket 
     * @return Frame 
     */
    Frame recvFrame(SOCKET sender_socket);

    

}


#endif