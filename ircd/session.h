#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <string>

#include <iostream>

#include "defines.h"
#include "user.h"

typedef boost::shared_ptr<User> UserPtr;

class Session : public boost::enable_shared_from_this<Session> {
    
public:

        static SessionPtr create(boost::asio::io_service& io_service);
		
		SessionPtr getPtr();
		UserPtr getUser() { return mUser; }

		void start();

		void sendAsServer(const std::string& message);
        void sendAsUser(const std::string& message);

        void send(const std::string& message);
		void close();
		boost::asio::ip::tcp::socket& socket();
        std::string ip() const;

private:

		Session(boost::asio::io_service& io_service);
		void read();
		void handleRead(const boost::system::error_code& error, std::size_t bytes);

		UserPtr mUser;

		boost::asio::ip::tcp::socket mSocket;
        boost::asio::streambuf mBuffer;
        std::string mMessage;
};