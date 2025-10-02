#pragma once

#include "../request.hpp"
#include "../response.hpp"
#include "../../utils/fields_alloc.hpp"

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class TestController {
public:
    using alloc_t = fields_alloc<char>;
    using request_body_t = http::string_body;

    static void handle(const Request<request_body_t, alloc_t>& req, Response<http::string_body, alloc_t>& res);
};
