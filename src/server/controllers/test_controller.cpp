#include "test_controller.hpp"

void TestController::handle(const Request<request_body_t, alloc_t>& req, Response<http::string_body, alloc_t>& res) {
    res.status(http::status::ok)
       .json(R"({"message":"hello world"})");
}
