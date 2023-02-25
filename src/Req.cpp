#pragma once
#define BOOST_ASIO_DISABLE_CONCEPTS
#include <iostream>
#include <string>
#include <boost/beast.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using boost::property_tree::ptree;
using std::string;
namespace http = boost::beast::http;
using http::field;
class Req
{
	boost::asio::io_context ioc;
	boost::shared_ptr<boost::asio::ip::tcp::socket> s;
	boost::shared_ptr<boost::asio::ip::tcp::resolver> r;
	std::string ip;
	std::string port;
public:
	explicit Req(const std::string& ip, const std::string& port) :ip(ip), port(port)
	{
		r.reset(new boost::asio::ip::tcp::resolver(ioc));
		s.reset(new boost::asio::ip::tcp::socket(ioc));
		boost::asio::connect(*s, r->resolve(ip, port));
	}
	ptree Request(const string& target, const string& body)
	{
		http::request<http::string_body> req{ http::verb::post, target,11 };
		req.set(field::host, ip + ":" + port);
		req.set(field::user_agent, BOOST_BEAST_VERSION_STRING);
		req.set(field::server, "Beast");
		req.body() = body;
		req.set(field::content_type, "application/json");
		req.set(field::content_length, std::to_string(body.length()));
		http::write(*s, req);
		boost::beast::flat_buffer buffer;
		http::response<http::dynamic_body> re;
		http::read(*s, buffer, re);
		std::stringstream stream(boost::beast::buffers_to_string(re.body().data()));
		ptree data;
		read_json(stream, data);
		return data;
	}
	const string& GetIP() const
	{
		return ip;
	}
	const string& GetPort() const
	{
		return port;
	}
	Req(const Req&) = delete;
	Req() = delete;
};
