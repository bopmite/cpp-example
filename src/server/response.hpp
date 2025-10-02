#pragma once

#include <boost/beast/http.hpp>
#include <boost/utility/string_view.hpp>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;

template<typename Body, typename Allocator>
class Response {
public:
    using response_type = http::response<Body, http::basic_fields<Allocator>>;

    explicit Response(response_type& res)
        : res_(res) {}

    Response& status(http::status s) {
        res_.result(s);
        return *this;
    }

    Response& header(http::field field, beast::string_view value) {
        res_.set(field, value);
        return *this;
    }

    Response& header(beast::string_view name, beast::string_view value) {
        res_.set(name, value);
        return *this;
    }

    Response& body(beast::string_view content) {
        res_.body() = std::string(content.data(), content.size());
        return *this;
    }

    Response& json(beast::string_view content) {
        res_.set(http::field::content_type, "application/json");
        res_.body() = std::string(content.data(), content.size());
        return *this;
    }

    Response& text(beast::string_view content) {
        res_.set(http::field::content_type, "text/plain");
        res_.body() = std::string(content.data(), content.size());
        return *this;
    }

    void prepare() {
        res_.prepare_payload();
    }

    response_type& raw() { return res_; }

private:
    response_type& res_;
};
