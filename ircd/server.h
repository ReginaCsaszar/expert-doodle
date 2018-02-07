#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "defines.h"
#include "session.h"

using boost::asio::ip::tcp;

class Server {
    public:
   
        Server(boost::asio::io_service& io_service);
        void    start();

    private:
        
        void    startAccept();
        void    handleAccept(Session::pointer newclient, const boost::system::error_code& error);
        tcp::acceptor       mAcceptor; ///< Acceptor
};
