/**
 * @file internal_message.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef INTERNAL_MESSAGE_HPP
#define INTERNAL_MESSAGE_HPP


namespace ASE
{


enum InternalMessageCodes {
    NEWCLIENT = 0x00,
    REMOVECLIENT = 0x01,
    KICK_YOU = 0x02,
    CUSTOM = 0x03,
    EMPTY = 0x04
};


class InternalMessage
{
private:
    int hat_;
    std::vector<uint8_t> data_;
public:

    InternalMessage()
    {

    }

    InternalMessage(int hat, std::vector<uint8_t> data) : hat_(hat), data_(data)
    {

    }


    // ~~~~~ GET ~~~~~

    int getHat() const
    {
        return hat_;
    }

    std::vector<uint8_t> getDataCopy() const
    {
        return data_;
    }

    const std::vector<uint8_t> &getDataRef() const
    {
        return data_;
    }
    
    std::string to_string()
    {
        return std::to_string(hat_);
    }

};

InternalMessage CreateNewClientInternalMessage(int client_id)
{
    return InternalMessage(NEWCLIENT, {uint8_t(client_id), uint8_t(client_id >> 8), uint8_t(client_id >> 16), uint8_t(client_id >> 24)});
}

InternalMessage CreateRemoveClientInternalMessage(int client_id)
{
    return InternalMessage(REMOVECLIENT, {uint8_t(client_id), uint8_t(client_id >> 8), uint8_t(client_id >> 16), uint8_t(client_id >> 24)});
}

InternalMessage CreateCustomInternalMessage(std::vector<uint8_t> data)
{
    return InternalMessage(CUSTOM, std::move(data));
}


}

#endif