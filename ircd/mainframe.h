#pragma once

#include <map>
#include <string>

#include "server.h"
#include "user.h"
#include "channel.h"


typedef std::map<std::string, UserPtr>    UserMap; 
typedef std::map<std::string, Channel*> ChannelMap; 

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
        void    addChannel(Channel* chan);
        void    removeChannel(const std::string& name);

        Channel* getChannelByName(const std::string& name);
        void    updateChannels();
        ChannelMap channels() const;

    private:

		Mainframe() = default;

		Mainframe(const Mainframe&) = delete;
		Mainframe& operator=(const Mainframe&) = delete;

        void removeAllChannels();

        static std::unique_ptr<Mainframe> mInstance;
		static std::once_flag onceFlag;

        UserMap mUsers;
        ChannelMap mChannels;
};
