#include "channel.h"
#include "session.h"

#include <iostream>

Channel::Channel(User* creator, const std::string& name, const std::string& topic)
:   mName(name), mTopic(topic), mUsers(),  mOperators(),  mPassword(), 
	mSlots(0), bPassword(false), bLimited(false)
{
    if(!creator) {
        throw std::runtime_error("Invalid user");
    }

    mOperators.insert(creator);
    mUsers.insert(creator);
    std::cout << "[Server] " << mName << " channel created" << std::endl;
}

Channel::~Channel() {
    std::cout << "[" << mName << "] channel deleted" << std::endl;
}

void Channel::addUser(User* user) {
    if(user) {
        mUsers.insert(user);
        sendUserList(); 
    }
}

void Channel::removeUser(User* user) {
	if (!user) return; 
	if (hasUser(user))  mUsers.erase(user);
	if (isOperator(user)) mOperators.erase(user);
    sendUserList(); 
}

bool Channel::hasUser(User* user) { return ((mUsers.find(user)) != mUsers.end()); }

bool Channel::isOperator(User* user) { return ((mOperators.find(user)) != mOperators.end()); }

void Channel::broadcast(const std::string& message) {
    std::cout << "[" << mName << "] broadcast : " << message << std::endl;
    UserSet::iterator it = mUsers.begin();
    for(; it != mUsers.end(); ++it) {
        (*it)->session()->send(message);
    }
}

void Channel::sendUserList() {
    UserSet::iterator it = mUsers.begin();
    for(; it != mUsers.end(); ++it) {
        if((mOperators.find((*it))) != mOperators.end()) {
            broadcast(":" + Config::ServerName + " " + ToString(Response::Reply::RPL_NAMREPLY) 
				+ " @" + (*it)->nick() + " = " + mName + " :" + "@" + (*it)->nick() 
				+ Config::EOFMessage);
        } else {
            broadcast(":" + Config::ServerName + " " + ToString(Response::Reply::RPL_NAMREPLY) 
				+ " " + (*it)->nick() + " = " + mName + " :" + (*it)->nick() 
				+ Config::EOFMessage);
        }
    }
    for(it = mUsers.begin(); it != mUsers.end(); ++it)
        (*it)->session()->sendAsServer(ToString(Response::Reply::RPL_ENDOFNAMES) + " " 
			+ (*it)->nick() + " "  + mName + " :End of /NAMES list." 
			+ Config::EOFMessage);
}

void Channel::sendWhoList() {
    UserSet::iterator it = mUsers.begin();
    for(; it != mUsers.end(); ++it) {
        if((mOperators.find((*it))) != mOperators.end()) {
            broadcast(":" + Config::ServerName + " " 
				+ ToString(Response::Reply::RPL_WHOREPLY) + " " 
				+ (*it)->nick() + " " 
				+ mName + " " 
				+ (*it)->nick() + " " 
				+ (*it)->host() + " " 
				+ Config::ServerName + " " 
				+ (*it)->nick() + " H@ :0 " 
				+ (*it)->name() 
				+ Config::EOFMessage);
        } else {
            broadcast(":" + Config::ServerName + " " 
				+ ToString(Response::Reply::RPL_WHOREPLY) + " " 
				+ (*it)->nick() + " " 
				+ mName + " " 
				+ (*it)->nick() + " " 
				+ (*it)->host() + " " 
				+ Config::ServerName + " " 
				+ (*it)->nick() + " H :0 " 
				+ (*it)->name() 
				+ Config::EOFMessage);
        }
    }

    for(it = mUsers.begin(); it != mUsers.end(); ++it)
        (*it)->session()->sendAsServer(ToString(Response::Reply::RPL_ENDOFWHO) + " " 
			+ (*it)->nick() + " " 
			+ mName + " :End of /WHO list." 
			+ Config::EOFMessage);
}

std::string Channel::name() const { return mName; }

std::string Channel::topic() const { return mTopic; }

bool Channel::empty() const { return (mUsers.empty()); }

std::string Channel::password() const { return mPassword; }

unsigned int Channel::userCount() const { return mUsers.size(); }

bool Channel::hasPass() const { return bPassword; }

bool Channel::full() const { return !(mUsers.size() < mSlots); }

bool Channel::limited() const { return bLimited; }

void Channel::cmdOPlus(User *user, User *victim) {
    if (this->hasUser(victim) && this->hasUser(user)) {
        if (this->isOperator(user)) {
            mOperators.insert(victim);
            sendUserList();
        }
    } else
        user->session()->sendAsServer(ToString(Response::Error::ERR_NOTONCHANNEL) + Config::EOFMessage);
}

void Channel::cmdOMinus(User *user) {
    if (this->isOperator(user)) {
        mOperators.erase(user);
        sendUserList();
    }
}

void Channel::cmdKPlus(const std::string& newPass) {
    bPassword = true;
    mPassword = newPass;
}

void Channel::cmdKMinus() { bPassword = false; }

void Channel::cmdLPlus(const std::string& newNbPlace) {
    bLimited = true;
    std::istringstream iss(newNbPlace);
    iss >> mSlots;
}

void Channel::cmdLMinus() { bLimited = false; }

void Channel::cmdTopic(const std::string& topic) { mTopic = topic; }
