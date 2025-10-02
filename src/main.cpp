#include "server/http_worker.hpp"
#include "server/router.hpp"
#include "server/controllers/test_controller.hpp"

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

int main(int argc, char* argv[])
{
    try
    {
        auto const address = net::ip::make_address("0.0.0.0");
        unsigned short port = static_cast<unsigned short>(4545);
        int num_workers = std::atoi("1");
        bool spin = (std::strcmp("block", "spin") == 0);

        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};

        http_worker::router_t router;

        router.get("/test", [](const auto& req, auto& res) {
            TestController::handle(req, res);
        });
        

        std::list<http_worker> workers;
        for (int i = 0; i < num_workers; ++i)
        {
            workers.emplace_back(acceptor, ".", router);
            workers.back().start();
        }

        std::cout << "Server running on http://0.0.0.0:" << port << "\n";
        std::cout << "Try: curl \"http://localhost:4545/test\"\n";

        if (spin)
            for (;;) ioc.poll();
        else
            ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
