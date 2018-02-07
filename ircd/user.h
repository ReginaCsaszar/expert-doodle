#pragma once

#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <set>

class Session;
class Channel;

typedef std::vector<std::string> StrVec;
typedef std::set<Channel*> ChannelSet;

class User {
    friend class Session;

    public:

        User(Session*   mysession);
        ~User();
        void cmdNick(const std::string& newnick);
        void cmdUser(const std::string& host, const std::string& realname);
		void cmdQuit();
        void cmdJoin(Channel* channel);
        void cmdPart(Channel* channel);
        void cmdKick(User* victim, const std::string& reason, Channel* channel);
        void cmdPing();

        Session* session() const;
        std::string name() const;
        std::string nick() const;
        std::string host() const;
        std::string messageHeader() const;

        void setName(const std::string& name); 
        void setNick(const std::string& nick);
        void setHost(const std::string& host);
		
private:

	Session* mSession;

        std::string mRealName;
        std::string  mNickName;
        std::string mHost;

        bool bSentUser;
        bool bSentNick;
        bool bSentMotd;
        bool bProperlyQuit;

        ChannelSet mChannels;
};
