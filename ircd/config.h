#pragma once

#include <string>
#include <stdint.h>

namespace Config
{
    extern uint16_t           Port;
    extern std::string      EOFMessage;
    extern std::string      ServerName;
    extern std::string      FormatServerName;
};

namespace Response
{
    //RFC 1459 Error codes
    namespace Error
    {
        enum error_code
        {
            ERR_NOSUCHNICK = 401,
            ERR_NOSUCHSERVER,
            ERR_NOSUCHCHANNEL,
            ERR_CANNOTSENDTOCHAN,
            ERR_TOOMANYCHANNELS,
            ERR_WASNOSUCHNICK,
            ERR_TOOMANYTARGETS,
            ERR_NOORIGIN = 409,
            ERR_NORECIPIENT = 411,
            ERR_NOTEXTTOSEND,
            ERR_NOTOPLEVEL,
            ERR_WILDTOPLEVEL,
            ERR_UNKNOWNCOMMAND = 421,
            ERR_NOMOTD,
            ERR_NOADMININFO,
            ERR_FILEERROR,
            ERR_NONICKNAMEGIVEN = 431,
            ERR_ERRONEUSNICKNAME,
            ERR_NICKNAMEINUSE,
            ERR_NICKCOLLISION = 436,
            ERR_USERNOTINCHANNEL = 441,
            ERR_NOTONCHANNEL,
            ERR_USERONCHANNEL,
            ERR_NOLOGIN,
            ERR_SUMMONDISABLED,
            ERR_USERSDISABLED,
            ERR_NOTREGISTERED = 451,
            ERR_NEEDMOREPARAMS = 461,
            ERR_ALREADYREGISTRED,
            ERR_NOPERMFORHOST,
            ERR_PASSWDMISMATCH,
            ERR_YOUREBANNEDCREEP,
            ERR_KEYSET = 467,
            ERR_CHANNELISFULL = 471,
            ERR_UNKNOWNMODE,
            ERR_INVITEONLYCHAN,
            ERR_BANNEDFROMCHAN,
            ERR_BADCHANNELKEY
        };
    }

    //Enum with RFC 1459 Reply codes   
    namespace Reply
    {
        enum rpl_code
        {
            RPL_NONE = 300,
            RPL_AWAY,
            RPL_USERHOST,
            RPL_ISON,
            RPL_ENDOFWHO = 315,
            RPL_LISTSTART = 321,
            RPL_LIST,
            RPL_LISTEND,
            RPL_NOTOPIC = 331,
            RPL_TOPIC = 332,
            RPL_WHOREPLY = 352,
            RPL_NAMREPLY = 353,
            RPL_ENDOFNAMES = 366
        };
    }
}