#include "user.h"
#include "session.h"
#include "mainframe.h"


User::User()
:   mSession(nullptr), bSentUser(false), bSentNick(false), bSentMotd(false), bProperlyQuit(false) {}

User::~User() {
    if(!bProperlyQuit) {
        ChannelSet::iterator it = mChannels.begin();
        for(; it != mChannels.end(); ++it) {
            (*it)->removeUser(mSession->getUser());
            (*it)->broadcast(messageHeader() + "PART " + (*it)->name() + " :Leave the channel" + Config::EOFMessage);
            mChannels.erase((*it));
        }
        Mainframe::instance().removeUser(mNickName);
        Mainframe::instance().updateChannels();
    }
}

void User::cmdNick(const std::string& newnick) {
    if(bSentNick) {
        if(Mainframe::instance().changeNickname(mNickName, newnick)) {
            mSession->sendAsUser("NICK :"+ newnick + Config::EOFMessage);
            ChannelSet::iterator it = mChannels.begin();
            for(; it != mChannels.end(); ++it) {
                (*it)->broadcast(messageHeader() + "NICK " + newnick + Config::EOFMessage);
                (*it)->sendWhoList();
            }
            setNick(newnick);
        } else {
            mSession->sendAsServer(ToString(Response::Error::ERR_NICKCOLLISION) + " " 
				+ mNickName + " " 
				+ newnick + " :Nickname is already in use." 
				+ Config::EOFMessage);
        }
    } else {
        if(!Mainframe::instance().doesNicknameExists(newnick)) {
            setNick(newnick);
            bSentNick = true;
        } else {
            mSession->sendAsServer(ToString(Response::Error::ERR_NICKCOLLISION) + " " 
				+ mNickName + " " 
				+ newnick 
				+ " :Nickname is already in use." 
				+ Config::EOFMessage);
        }
    }
}

void User::cmdUser(const std::string& host, const std::string& realname) {
    if(bSentUser) {
        mSession->sendAsServer(ToString(Response::Error::ERR_ALREADYREGISTRED) + " " 
			+ mNickName 
			+ " You are already registered !" 
			+ Config::EOFMessage);
    } else {
        if(!bSentNick) {
            mSession->sendAsServer(ToString(Response::Error::ERR_NONICKNAMEGIVEN) + " No nickname given, use NICK first !" + Config::EOFMessage);
            return;
        }
        if(!Mainframe::instance().addUser(mSession->getUser())) {
            mSession->sendAsServer(ToString(Response::Error::ERR_NICKCOLLISION) + " " 
				+ mNickName + " This nickname is already used !" 
				+ Config::EOFMessage);
            return;
        }
        mHost = mSession->ip();
        mRealName = realname;
        bSentUser = true;
    }

    if(bSentNick && bSentUser && !bSentMotd) {
        bSentMotd = true;
        mSession->sendAsServer("001 " + mNickName + " :Welcome to " + Config::ServerName + " IRC network, " + mNickName + Config::EOFMessage);
        mSession->sendAsServer("002 " + mNickName + " :Your host is " + mHost + Config::EOFMessage);
        mSession->sendAsServer("003 " + mNickName + " :This server is " + Config::ServerName + Config::EOFMessage);
    }
}

void User::cmdQuit() {
    ChannelSet::iterator it = mChannels.begin();
    for(; it != mChannels.end(); ++it) {
        (*it)->broadcast(messageHeader() + "PART " 
			+ (*it)->name() + " : Leave the channel" 
			+ Config::EOFMessage);
        (*it)->removeUser(mSession->getUser());
        //mChannels.erase((*it));
    }
    Mainframe::instance().removeUser(mNickName);
    Mainframe::instance().updateChannels();
    mSession->close();
    bProperlyQuit = true;
}

void User::cmdPart(Channel* channel) {
    channel->broadcast(messageHeader() + "PART " 
		+ channel->name() + " : Leave the channel" 
		+ Config::EOFMessage);
    channel->removeUser(mSession->getUser());
    mChannels.erase(channel);
    Mainframe::instance().updateChannels();
}

void User::cmdJoin(Channel* channel) {
    mSession->sendAsUser("JOIN " + channel->name() + Config::EOFMessage);
    mChannels.insert(channel);
    channel->addUser(mSession->getUser());
}

void User::cmdKick(UserPtr victim, const std::string& reason, Channel* channel) {
    channel->broadcast(":" + mNickName 
		+ " KICK " + channel->name() + " " 
		+ victim->nick() + " :" 
		+ reason 
		+ Config::EOFMessage);
}

void User::cmdPing() {
    mSession->sendAsServer("PONG" + Config::EOFMessage);
}

SessionPtr User::session() const {
    return mSession;
}

std::string User::name() const {
    return mRealName;
}

std::string User::nick() const {
    return mNickName;
}

std::string User::host() const {
    return mHost;
}

void User::setName(const std::string& name) {
    mRealName = name;
}

void User::setNick(const std::string& nick) {
    mNickName = nick;
}

void User::setHost(const std::string& host) {
    mHost = host;
}

std::string User::messageHeader() const {
    return std::string(":"+mNickName+"!"+mNickName+"@"+mHost+" ");
}
