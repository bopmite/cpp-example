#pragma once

#include "../utils/fields_alloc.hpp"
#include "router.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class http_worker
{
public:
    http_worker(http_worker const&) = delete;
    http_worker& operator=(http_worker const&) = delete;

    using alloc_t = fields_alloc<char>;
    using request_body_t = http::string_body;
    using router_t = Router<request_body_t, alloc_t>;

    http_worker(tcp::acceptor& acceptor, const std::string& doc_root, router_t& router);

    void start();

private:

    tcp::acceptor& acceptor_;

    std::string doc_root_;

    router_t& router_;

    tcp::socket socket_;

    beast::flat_static_buffer<8192> buffer_;

    alloc_t alloc_;

    boost::optional<http::request_parser<request_body_t, alloc_t>> parser_;

    net::steady_timer request_deadline_;

    boost::optional<http::response<http::string_body, http::basic_fields<alloc_t>>> string_response_;

    boost::optional<http::response_serializer<http::string_body, http::basic_fields<alloc_t>>> string_serializer_;

    boost::optional<http::response<http::file_body, http::basic_fields<alloc_t>>> file_response_;

    boost::optional<http::response_serializer<http::file_body, http::basic_fields<alloc_t>>> file_serializer_;

    void accept();
    void read_request();
    void process_request(http::request<request_body_t, http::basic_fields<alloc_t>> const& req);
    void send_bad_response(http::status status, std::string const& error);
    void check_deadline();
};