/**
 * @file connection_expections.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef CONNECTION_EXCEPTIONS_HPP
#define CONNECTION_EXCEPTIONS_HPP


using namespace std;

#include <iostream>

/**
 * @brief Exception thrown when the connection with the client has a problem
 * 
 */
class RemoteConnectionException : public std::exception {

private:
    const char* message;


public:
    RemoteConnectionException(const char* msg) : message(msg) {}

    const char* what()
    {
        return message;
    }
};


/**
 * @brief Exception thrown when client send a too large Frame or data package
 * 
 */
class DataSizeException : public std::exception {

private:
    const char* message;

public:
    DataSizeException(const char* msg) : message(msg) {}

    const char* what()
    {
        return message;
    }
};

/**
 * @brief Exception thrown when the server socket as a problem
 * 
 */
class ServerConnectionException : public std::exception {

private:
    const char* message;

public:
    ServerConnectionException(const char* msg) : message(msg) {}

    const char* what()
    {
        return message;
    }
};


class ServerFullException : public std::exception {

private:
    const char* message;

public:
    ServerFullException(const char* msg) : message(msg) {}

    const char* what()
    {
        return message;
    }
};

class ConnectionRefused : public std::exception {

private:
    const char* message;

public:
    ConnectionRefused(const char* msg) : message(msg) {}

    const char* what()
    {
        return message;
    }
};

#endif