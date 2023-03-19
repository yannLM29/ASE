/**
 * @file frame.cpp
 * @author Yann Le Masson
 * 
 */
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <chrono>

#include "frame.hpp"
#include "security_properties.hpp"


namespace ASE
{
    Frame::Frame(/* args */)
    {
        
    }

    Frame::~Frame()
    {
    }

    void Frame::addMessage(Message message)
    {
        messages_.emplace_back(std::move(message));

    }

    void Frame::sendFrame(SOCKET receiver_socket)
    {
        int frame_length = int(messages_.size());    // number of messages to send

        uint8_t header[4] = {LENGTH,0,0,0};

        if(frame_length > 0xFFFFFF)
        {
            throw std::length_error("Frame Length to high");
        }

        header[1] = uint8_t(frame_length);
        header[2] = uint8_t(frame_length >> 8);
        header[3] = uint8_t(frame_length >> 16); 

        if(send(receiver_socket, header, size_t(4), MSG_NOSIGNAL) != 4)
        {
            throw RemoteConnectionException("Client disconnected during header sending");
        }

        for(const auto &m : messages_)
        {

            m.sendMessage(receiver_socket);

            
            

        }
        
        uint8_t end = END;
        if(send(receiver_socket, &end, size_t(1), MSG_NOSIGNAL) != 1)
        {
            throw RemoteConnectionException("Client disconnected during end sending");
        }

    }


    std::string Frame::toString()
    {
        std::string output = "";

        for(auto &m : messages_)
        {
            output += m.toString() + " - ";
        }

        return output;
    }

    void Frame::clear()
    {
        messages_.clear();
    }


    Frame recvFrame(SOCKET sender_socket)
    {
        Frame output;

        uint8_t header[4] = {0};
        


        if(recv(sender_socket, header, size_t(4), 0) != 4)
        {
            throw RemoteConnectionException("Client disconnected during header receiving");
        }
        
        if(header[0] != LENGTH)
        {
            std::cout << "header= " << header[0] << "\n";
            throw RemoteConnectionException("Client sended bad code in header");
        }

        int size = header[1] | (header[2] >> 8) | (header[3] >> 16);

        if(size > FRAME_SIZE_LIMIT)
        {
            throw DataSizeException("Client sended too long frame");
        }

        for(int i = 0; i < size; i++)
        {
            output.addMessage(recvMessage(sender_socket));
        }

        uint8_t end;
        if(recv(sender_socket, &end, size_t(1), 0) != 1 || end != END)
        {   
            throw RemoteConnectionException("Client sended bad end code");
        }

        return output;

    }

    
}

