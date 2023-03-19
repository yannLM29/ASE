/**
 * @file message_codes.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef MESSAGE_CODES_H
#define MESSAGE_CODES_H

namespace ASE
{

enum MessageCodes {
  STOP = 0x0,
  LENGTH = 0x1,
  END = 0x2,
  CONNECT = 0x3,
  DISCONNECT = 0x4,
  DATA = 0x5,
  CODATA = 0x6,
  OCONNECT = 0x7,
  ODISCONNECT = 0x8,
  KICK = 0x9,
  CONNECTWINFO = 0xA,
  FULL = 0xB,
  COREFUSED = 0xC,
  BADCODATA = 0xD,
  COACCEPTED = 0xE,
  YOURID = 0xF
};

typedef enum MessageCodes MessageCodes;

}

#endif