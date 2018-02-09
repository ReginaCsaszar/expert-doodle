#include "server.h"

Server::Server(boost::asio::io_service& io_service)
:   mAcceptor(io_service, tcp::endpoint(tcp::v4(), Config::Port))
{
    mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    mAcceptor.listen();
}

void Server::start() { 
	std::cout << "[Server] Server started "  << std::endl;
	startAccept(); 
}

void Server::startAccept() {
    Session::pointer newclient = Session::create(mAcceptor.get_io_service());
    mAcceptor.async_accept(newclient->socket(),
                           boost::bind(&Server::handleAccept,   this,   newclient,  boost::asio::placeholders::error));
}

void Server::handleAccept(Session::pointer newclient, const boost::system::error_code& error) {
    if(error) {
        std::cerr << "[Server] Error : " << error.message() << std::endl;
    } else {
        std::cout << "[Server] Connection: " << newclient->ip() << std::endl;
        newclient->start();
    }
    startAccept();
}