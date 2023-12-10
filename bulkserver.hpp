#pragma once

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <cstdio>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

// awaitable<void> echo_once(tcp::socket& socket)
// {
//   char data[128];
//   std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
//   co_await async_write(socket, boost::asio::buffer(data, n), use_awaitable);
// }

// awaitable<void> echo(tcp::socket socket)
// {
//   try
//   {
//     for (;;)
//     {
//       // The asynchronous operations to echo a single chunk of data have been
//       // refactored into a separate function. When this function is called, the
//       // operations are still performed in the context of the current
//       // coroutine, and the behaviour is functionally equivalent.
//       co_await echo_once(socket);
//     }
//   }
//   catch (std::exception& e)
//   {
//     std::printf("echo Exception: %s\n", e.what());
//   }
// }

awaitable<void> listener()
{
  auto executor = co_await this_coro::executor;
  tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
  for (;;)
  {
    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
    co_spawn(executor, echo(std::move(socket)), detached);
  }
}