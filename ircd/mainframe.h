#pragma once

#include <map>
#include <string>

#include "server.h"
#include "user.h"
#include "channel.h"


typedef std::map<std::string, User*>    UserMap; 
typedef std::map<std::string, Channel*> ChannelMap; 

class Mainframe {

public:

        static Mainframe*   instance();
		void    start();
		
		bool    doesNicknameExists(const std::string& nick);
        bool    addUser(User* user);
		void    removeUser(const std::string& nick);
		bool    changeNickname(const std::string& old, const std::string& recent);
        User*   getUserByName(const std::string& nick);

        bool    doesChannelExists(const std::string& name);
        void    addChannel(Channel* chan);
        void    removeChannel(const std::string& name);

        Channel* getChannelByName(const std::string& name);
        void    updateChannels();
        ChannelMap channels() const;

    private:

		Mainframe() = default;
        ~Mainframe(); 
        Mainframe(const Mainframe&);
		Mainframe& operator=(Mainframe&) {};

        void removeAllChannels();

        static Mainframe* mInstance; 
        UserMap mUsers;
        ChannelMap mChannels;
};
