#include "mainframe.h"

std::unique_ptr<Mainframe> Mainframe::mInstance;;
std::once_flag Mainframe::onceFlag;

Mainframe& Mainframe::instance() {
	std::call_once(Mainframe::onceFlag, []() {
		mInstance.reset(new Mainframe);
	});
	return *(mInstance.get());
}

Mainframe::~Mainframe() {
    removeAllChannels();
}

void Mainframe::start() {
    boost::asio::io_service ios;
    Server server(ios);
    server.start();
    ios.run();
}

bool Mainframe::doesNicknameExists(const std::string& nick) {
    return mUsers.count(nick);
}

bool Mainframe::addUser(UserPtr user) {
    if(doesNicknameExists(user->nick())) return false;
    mUsers[user->nick()] = user;
    return true;
}

bool Mainframe::changeNickname(const std::string& old, const std::string& recent) {
    if(doesNicknameExists(recent))    return false;
    if(!doesNicknameExists(old))    return false;

	UserPtr tmp = mUsers[old];
    mUsers.erase(old);
    mUsers[recent] = tmp;
    return true;
}

void Mainframe::removeUser(const std::string& nick) { mUsers.erase(nick); }

UserPtr Mainframe::getUserByName(const std::string& nick) {
    if(! doesNicknameExists(nick) ) return NULL;
    return mUsers[nick];
}

bool Mainframe::doesChannelExists(const std::string& name) {
    ChannelMap::iterator it = mChannels.begin();
    return ((mChannels.find(name)) != mChannels.end());
}

void Mainframe::addChannel(Channel* chan) {
    std::string channame = chan->name();
    if(!doesChannelExists(channame)) {
        mChannels[channame] = chan;
    }
}

void Mainframe::removeChannel(const std::string& name) { mChannels.erase(name); }

Channel* Mainframe::getChannelByName(const std::string& name) {
    if(!doesChannelExists(name))    return NULL;
    return mChannels[name];
}

void Mainframe::removeAllChannels() {
    ChannelMap::iterator it = mChannels.begin();
    for(; it != mChannels.end(); ++it) {
        delete (it->second);
    }
}

void Mainframe::updateChannels() {
    ChannelMap::iterator it = mChannels.begin();
    while(it != mChannels.end()) {
        if(it->second->empty()) {
            delete (it->second);
            mChannels.erase(it);
        }
         it++;
     }
}

ChannelMap Mainframe::channels() const { return mChannels; }