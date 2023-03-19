/**
 * @file message.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <stdint.h>
#include <vector>
#include <array>
#include <tuple>
#include <string>

#include "cross_sockets.hpp"
#include "message_codes.hpp"
#include "connection_expections.hpp"

namespace ASE
{
    using message_code = uint8_t;

    class Message
    {
        message_code hat_;
        std::vector<uint8_t> data_;

        public:

            /**
             * @brief Construct a new Message object form a data vector
             * 
             * @param hat the MessageCode (normaly DATA)
             */
            Message(MessageCodes hat, std::vector<uint8_t>data): hat_(hat), data_(std::move(data))
            {

            }
            
            /**
             * @brief Construct a new Message object from anything
             * 
             * @param hat the MessageCode (normaly DATA)
             * @param data address of the data of the message 
             * @param size Size of the messsage's data
             */
            Message(MessageCodes hat, const void *data, std::size_t size): hat_(hat)
            {
                auto converted_data = (const uint8_t*)(data);

                for(const uint8_t *data_byte = converted_data; data_byte < converted_data+size; data_byte++)
                {
                    data_.emplace_back(*data_byte);
                }
            }

            /**
             * @brief Get the message's code
             * 
             * @return message_code 
             */
            inline message_code getHat() const
            {
                return hat_;
            }

            /**
             * @brief Get the Size Of Data
             * 
             * @return std::size_t 
             */
            inline std::size_t getSizeOfData() const
            {
                return data_.size();
            }

            /**
             * @brief Get the Data Copy object
             * 
             * @return std::vector<uint8_t> 
             */
            inline std::vector<uint8_t> getDataCopy() const
            {
                return data_;
            }
            
            /**
             * @brief Get a const ref of data
             * 
             * @return const std::vector<uint8_t>& 
             */
            inline const std::vector<uint8_t> &getDataConstRef() const
            {
                return data_;
            }

            /**
             * @brief Get a ref of data
             * 
             * @return const std::vector<uint8_t>& 
             */
            inline std::vector<uint8_t> &getDataRef() 
            {
                return data_;
            }

            /**
             * @brief Get a void pointer on data
             * 
             * @return const std::vector<uint8_t>& 
             */
            inline void *getRawData() 
            {
                return (void*)(data_.data());
            }

            /**
             * @brief Get data's iterators cbegin and cend
             * 
             * @return std::tuple<std::vector<uint8_t>::const_iterator, std::vector<uint8_t>::const_iterator> 
             */
            inline std::tuple<std::vector<uint8_t>::const_iterator, std::vector<uint8_t>::const_iterator> getDataIt() const
            {
                return {data_.cbegin(), data_.cend()};
            }

            /**
             * @brief Send the message to the receiver through receiver_socket 
             * DON'T USE IT, use sendFrame instead
             * 
             * @param receiver_socket 
             */
            void sendMessage(SOCKET receiver_socket) const;

            
            /**
             * @brief get string representation of the message
             * 
             * @return std::string 
             */
            std::string toString() const;

    };

    /**
     * @brief Wait and receive message through sender_socket
     * DON'T USE IT, use recvFrame instead
     * 
     * @param sender_socket 
     * @return Message 
     */
    Message recvMessage(SOCKET sender_socket);
}


#endif