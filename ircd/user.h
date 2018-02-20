#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <vector>
#include <set>

class Session;
class Channel;

typedef std::vector<std::string> StrVec;
typedef std::set<Channel*> ChannelSet;
typedef boost::shared_ptr<Session> SessionPtr;

class User : public boost::enable_shared_from_this<User> {

    friend class Session;

public:

	typedef boost::shared_ptr<User> UserPtr;

        User(Session* mysession);
        ~User();

		UserPtr getPtr() { return shared_from_this(); }

        void cmdNick(const std::string& newnick);
        void cmdUser(const std::string& host, const std::string& realname);
		void cmdQuit();
        void cmdJoin(Channel* channel);
        void cmdPart(Channel* channel);
        void cmdKick(UserPtr victim, const std::string& reason, Channel* channel);
        void cmdPing();

		SessionPtr session() const;
        std::string name() const;
        std::string nick() const;
        std::string host() const;
        std::string messageHeader() const;

        void setName(const std::string& name); 
        void setNick(const std::string& nick);
        void setHost(const std::string& host);
		
private:

		SessionPtr mSession;

        std::string mRealName;
        std::string  mNickName;
        std::string mHost;

        bool bSentUser;
        bool bSentNick;
        bool bSentMotd;
        bool bProperlyQuit;

        ChannelSet mChannels;
};
