#pragma once

#include <boost/beast/http.hpp>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "request.hpp"
#include "response.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

template<typename Body, typename Allocator>
class Router {
public:
    using request_type = Request<Body, Allocator>;
    using response_type = Response<http::string_body, Allocator>;
    using handler_fn = std::function<void(const request_type&, response_type&)>;

    struct Route {
        http::verb method;
        std::string path;
        handler_fn handler;
    };

    void add(http::verb method, std::string_view path, handler_fn handler) {
        routes_.push_back({method, std::string(path), std::move(handler)});
    }

    void get(std::string_view path, handler_fn handler) {
        add(http::verb::get, path, std::move(handler));
    }

    void post(std::string_view path, handler_fn handler) {
        add(http::verb::post, path, std::move(handler));
    }

    void put(std::string_view path, handler_fn handler) {
        add(http::verb::put, path, std::move(handler));
    }

    void del(std::string_view path, handler_fn handler) {
        add(http::verb::delete_, path, std::move(handler));
    }

    bool handle(const request_type& req, response_type& res) {
        auto method = req.method();
        auto target = req.target();

        beast::string_view path = target;
        auto query_pos = target.find('?');
        if (query_pos != beast::string_view::npos) {
            path = target.substr(0, query_pos);
        }

        for (const auto& route : routes_) {
            if (route.method == method && route.path == path) {
                route.handler(req, res);
                return true;
            }
        }

        return false;
    }

private:
    std::vector<Route> routes_;
};
