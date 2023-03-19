/**
 * @file message.cpp
 * @author Yann Le Masson
 * 
 */
#include "message.hpp"
#include "security_properties.hpp"

#include <stdexcept>
#include <iostream>

namespace ASE
{

    void Message::sendMessage(SOCKET receiver_socket) const
    {
        uint8_t header[4] = {hat_,0,0,0};
        int size = int(data_.size());

        if(size > 0xFFFFFF)
        {
            throw std::length_error("Message Data to high");
        }

        header[1] = uint8_t(size);
        header[2] = uint8_t(size >> 8);
        header[3] = uint8_t(size >> 16); 


        if(send(receiver_socket,header,size_t(4), MSG_NOSIGNAL) != 4)
        {
            throw RemoteConnectionException("Client disconnected during message header sending");
            return;
        }

        auto test = data_.data();

        if(send(receiver_socket,data_.data(), size_t(size), MSG_NOSIGNAL) != ssize_t(size))
        {
            throw RemoteConnectionException("Client disconnected during message data sending");
        }


    }

    Message recvMessage(SOCKET sender_socket)
    {
        uint8_t header[4] = {0};

        if(recv(sender_socket, header, size_t(4), 0) != 4)
        {
            throw RemoteConnectionException("Client disconnected during header receiving");
        }
        
        

        int size = header[1] | (header[2] >> 8) | (header[3] >> 16);

        if(size > MESSAGE_SIZE_LIMIT)
        {
            throw RemoteConnectionException("Client sended too long frame");
        }

        std::vector<uint8_t> data_received;
        data_received.resize(size);
        ssize_t rcv_size = recv(sender_socket, data_received.data(), size_t(size), 0);

        if(size_t(rcv_size) != data_received.size())
        {
            throw RemoteConnectionException("Error during client data receiving");
        }

        return Message(MessageCodes(header[0]),std::move(data_received));

    }

    /**
     * @brief 
     * 
     * @return std::string 
     */
    std::string Message::toString() const
    {
        std::string output = std::to_string(data_.size()) + " ";

        for(const auto &d : data_)
        {
            output+= std::to_string(d) + " ";
        }
        return output;
    }
}