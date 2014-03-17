//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "stat_msg.pb.h"

extern "C" int write_cache_hash( const char* redis_ip, short redis_port, const char* hkey, const char* entry, float value ); 

using namespace std;
using boost::asio::ip::udp;

class server
{
public:
  server(boost::asio::io_service& io_service, short port, char* redis_server_ip, short redis_server_port )
    : io_service_(io_service),
      socket_(io_service, udp::endpoint(udp::v4(), port)),
	  r_svr_port_(redis_server_port)

  {
	
	strncpy( r_svr_ip_, redis_server_ip, 16 );
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        boost::bind(&server::handle_receive_from, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

//	cout << r_svr_ip_ << "\t" << r_svr_port_ << endl;
  }
  
  void handle_receive_from(const boost::system::error_code& error,
      size_t bytes_recvd)
  {
    if (!error && bytes_recvd > 0)
    {
	std::string data_str( data_ );
	IDSStatMessage msg_buf;
	msg_buf.ParseFromString( data_str );

	const char* hkey = msg_buf.product().c_str() ;
    const char* entry = msg_buf.module().c_str() ;
    float val = (float)atof( msg_buf.value().c_str() );

    write_cache_hash( r_svr_ip_, r_svr_port_, hkey, entry, val );
	
	char result_cs[10] = "200";
	size_t len_result = strlen( result_cs );
      
	
	socket_.async_send_to(
          boost::asio::buffer(result_cs, len_result), sender_endpoint_,
          boost::bind(&server::handle_send_to, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      socket_.async_receive_from(
          boost::asio::buffer(data_, max_length), sender_endpoint_,
          boost::bind(&server::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }

  void handle_send_to(const boost::system::error_code& /*error*/,
      size_t /*bytes_sent*/)
  {
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        boost::bind(&server::handle_receive_from, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  boost::asio::io_service& io_service_;
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  char data_[max_length];

  char r_svr_ip_[16];
  short r_svr_port_;
};


int main(int argc, char* argv[])
{
  try
  {
    if ( argc != 4 )
    {
      std::cerr << "Usage: server_app <server_port> <cache_ip> <cache_port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    server s(io_service, atoi(argv[1]), argv[2], atoi( argv[3]) );

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
