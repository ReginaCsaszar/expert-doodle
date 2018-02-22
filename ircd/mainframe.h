#pragma once

#include <map>
#include <string>

#include "server.h"
#include "user.h"
#include "channel.h"

typedef std::shared_ptr<Channel> ChPtr;
typedef std::map<std::string, UserPtr>    UserMap; 
typedef std::map<std::string, ChPtr> ChannelMap; 

class Mainframe {

public:
		
		~Mainframe();
        static Mainframe&   instance();
		void    start();
		
		bool    doesNicknameExists(const std::string& nick);
        bool    addUser(UserPtr user);
		void    removeUser(const std::string& nick);
		bool    changeNickname(const std::string& old, const std::string& recent);
        UserPtr   getUserByName(const std::string& nick);

        bool    doesChannelExists(const std::string& name);
        void    addChannel(ChPtr chan);
        void    removeChannel(const std::string& name);

		ChPtr getChannelByName(const std::string& name);
        void    updateChannels();
        ChannelMap channels() const;

    private:

		Mainframe() = default;
		Mainframe(const Mainframe&) = delete;
		Mainframe& operator=(const Mainframe&) = delete;

        void removeAllChannels();

        static Mainframe mInstance;

        UserMap mUsers;
        ChannelMap mChannels;
};
