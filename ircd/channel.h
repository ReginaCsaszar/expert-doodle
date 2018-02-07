#pragma once

#include <set>
#include <string>
#include "user.h"

typedef std::set<User*> UserSet; 

class Channel {
    
public:

        Channel(User* creator, const std::string& name, const std::string& topic = "");
        ~Channel();

        void cmdOPlus(User *user, User *victim);
        void cmdOMinus(User *user);
        void cmdKPlus(const std::string& newPass);
        void cmdKMinus();
        void cmdLPlus(const std::string& newNbPlace);
        void cmdLMinus();
        void cmdTopic(const std::string& topic);

        void addUser(User* user);
        void removeUser(User* user);
        bool hasUser(User* user);
        bool isOperator(User* user);
        void sendUserList();
        void sendWhoList();
        void broadcast(const std::string& message);

        std::string password() const;
        std::string name() const;
        std::string topic() const; 
        unsigned int userCount() const;
        bool empty() const;
        bool full() const;
        bool hasPass() const;
        bool limited() const;

    private:

        const std::string mName;
        std::string mTopic;
        UserSet mUsers;
        UserSet mOperators;
        std::string mPassword;
        unsigned int mSlots;
        bool bPassword;
        bool bLimited;
};