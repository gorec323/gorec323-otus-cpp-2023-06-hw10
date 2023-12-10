#include <iostream>
#include <sstream>
#include <thread>
#include <utility>
#include <async.h>
#include <streamredirect.hpp>
#include "version.h"
#include "bulkserver.hpp"

// using namespace std;

int main(int argc, const char *argv[])
{
    std::cout << "Async app version: " << PROJECT_VERSION << std::endl;

    const auto bulkSize = argc;
    if (bulkSize != 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " [number of commands]" << std::endl;
        return 1;
    }

    // обработка параметра командной строки с числом команд в блоке
    const int bulkCommandsLimit = std::atoi(argv[1]);
    if (bulkCommandsLimit < 1)
        return 0;

    // обработка набора команд
    auto cinCmdProcessing = [bulkCommandsLimit]()
    {
        std::string strCommand;
        auto connectHandler = async::connect(static_cast<std::size_t>(bulkCommandsLimit));

        while (std::cin >> strCommand) {
            async::receive(connectHandler, strCommand.c_str(), strCommand.size());
        }

        async::disconnect(connectHandler);
    };

    try {
        boost::asio::io_context io_context(1);

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ io_context.stop(); });

        co_spawn(io_context, listener(), detached);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::printf("Exception: %s\n", e.what());
    }

    return 0;

}
