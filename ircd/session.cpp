#include "session.h"
#include "parser.h"

Session::Session(boost::asio::io_service& io_service)
:   mUser(this), mSocket(io_service) {}

Session::pointer Session::create(boost::asio::io_service& io_service) {
    return Session::pointer(new Session(io_service));
}

void Session::start() { 
	sendAsServer(" Welcome " + Config::EOFMessage);
	sendAsServer(" Checking ident " + Config::EOFMessage);
	sendAsServer(" No identd(auth) response " + Config::EOFMessage);
	read(); 
}

void Session::close() {
    mSocket.cancel();
    mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}

void Session::read() {
    boost::asio::async_read_until(mSocket, mBuffer, Config::EOFMessage,
                                  boost::bind(&Session::handleRead, shared_from_this(),
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
}

void Session::handleRead(const boost::system::error_code& error, std::size_t bytes) {
    if(error) {
            if(error == boost::asio::error::eof) { 
                std::cout << "[Server] " << ip() << ": user exit" << std::endl;
                mSocket.close();
            } else 
                std::cerr << "[Server] Error (" << __LINE__ <<") : " << error.message() << " at " << ip() << std::endl;
    } else if(bytes == 0) {
        std::cerr << "[Server] Error : empty message from " << ip() << std::endl;
    } else {
        std::string message;
        std::istream istream(&mBuffer);
        std::getline(istream, message);
        message = message.substr(0, message.size()-1);
		// just for see
        std::cout << "[" << ip() << "] : " << message << std::endl;
        Parser::parse(message, &mUser);
        read();
    }
}

void Session::send(const std::string& message) {
    mMessage = message;
    boost::asio::write(mSocket, boost::asio::buffer(mMessage));
}

void Session::sendAsUser(const std::string& message) {
    send(mUser.messageHeader() + message);
}

void Session::sendAsServer(const std::string& message) {
    send(Config::FormatServerName + message);
}

boost::asio::ip::tcp::socket& Session::socket() { return mSocket; }

std::string Session::ip() const {
    return mSocket.remote_endpoint().address().to_string();
}
