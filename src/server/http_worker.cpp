#include "http_worker.hpp"
#include "request.hpp"
#include "response.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

http_worker::http_worker(tcp::acceptor& acceptor, const std::string& doc_root, router_t& router) :
    acceptor_(acceptor),
    doc_root_(doc_root),
    router_(router),
    socket_(acceptor_.get_executor()),
    alloc_(8192),
    request_deadline_(acceptor_.get_executor(), (std::chrono::steady_clock::time_point::max)())
{
}

void http_worker::start()
{
    accept();
    check_deadline();
}

void http_worker::accept()
{
    beast::error_code ec;
    socket_.close(ec);
    buffer_.consume(buffer_.size());

    acceptor_.async_accept(
        socket_,
        [this](beast::error_code ec)
        {
            if (ec)
            {
                accept();
            }
            else
            {
                request_deadline_.expires_after(
                    std::chrono::seconds(60));

                read_request();
            }
        });
}

void http_worker::read_request()
{
    parser_.emplace(
        std::piecewise_construct,
        std::make_tuple(),
        std::make_tuple(alloc_));

    http::async_read(
        socket_,
        buffer_,
        *parser_,
        [this](beast::error_code ec, std::size_t)
        {
            if (ec)
                accept();
            else
                process_request(parser_->get());
        });
}

void http_worker::process_request(http::request<request_body_t, http::basic_fields<alloc_t>> const& req)
{
    string_response_.emplace(
        std::piecewise_construct,
        std::make_tuple(),
        std::make_tuple(alloc_));

    string_response_->result(http::status::ok);
    string_response_->keep_alive(false);
    string_response_->set(http::field::server, "Beast");

    Request<request_body_t, alloc_t> wrapped_req(req);
    Response<http::string_body, alloc_t> wrapped_res(*string_response_);

    if (router_.handle(wrapped_req, wrapped_res)) {
        wrapped_res.prepare();

        string_serializer_.emplace(*string_response_);

        http::async_write(
            socket_,
            *string_serializer_,
            [this](beast::error_code ec, std::size_t)
            {
                socket_.shutdown(tcp::socket::shutdown_send, ec);
                string_serializer_.reset();
                string_response_.reset();
                accept();
            });
        return;
    }

    send_bad_response(
        http::status::not_found,
        "Not found\r\n");
}

void http_worker::send_bad_response(
    http::status status,
    std::string const& error)
{
    string_response_.emplace(
        std::piecewise_construct,
        std::make_tuple(),
        std::make_tuple(alloc_));

    string_response_->result(status);
    string_response_->keep_alive(false);
    string_response_->set(http::field::server, "Beast");
    string_response_->set(http::field::content_type, "text/plain");
    string_response_->body() = error;
    string_response_->prepare_payload();

    string_serializer_.emplace(*string_response_);

    http::async_write(
        socket_,
        *string_serializer_,
        [this](beast::error_code ec, std::size_t)
        {
            socket_.shutdown(tcp::socket::shutdown_send, ec);
            string_serializer_.reset();
            string_response_.reset();
            accept();
        });
}

void http_worker::check_deadline()
{
    if (request_deadline_.expiry() <= std::chrono::steady_clock::now())
    {
        socket_.close();

        request_deadline_.expires_at(
            (std::chrono::steady_clock::time_point::max)());
    }

    request_deadline_.async_wait(
        [this](beast::error_code)
        {
            check_deadline();
        });
}