#pragma once

#include <boost/beast/http.hpp>
#include <boost/utility/string_view.hpp>
#include <string>
#include <unordered_map>

namespace beast = boost::beast;
namespace http = beast::http;

template<typename Body, typename Allocator>
class Request {
public:
    using request_type = http::request<Body, http::basic_fields<Allocator>>;

    explicit Request(const request_type& req)
        : req_(req) {}

    http::verb method() const { return req_.method(); }
    beast::string_view target() const { return req_.target(); }
    beast::string_view body() const { return req_.body(); }

    beast::string_view header(http::field field) const {
        return req_[field];
    }

    beast::string_view header(beast::string_view name) const {
        return req_[name];
    }

    const request_type& raw() const { return req_; }

private:
    const request_type& req_;
};
