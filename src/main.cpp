#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "message.pb.h"

using namespace boost::asio;
using ru::spbau::chat::commons::protocol::Message;

class chat_user
{
public:
    virtual void accept_message() = 0;
    virtual void deliver_message() = 0;
};
typedef std::shared_ptr<chat_user> user_ptr;

class chat_room
{
public:
    chat_room()
    {
    }

    void add_new_user(user_ptr user)
    {
        rw_mutex.lock();
        users.push_back(user);
        rw_mutex.unlock();
#ifdef DEBUG
        std::cout << "User connected" << std::endl;
#endif
        user->accept_message();
    }

    void on_user_leave(chat_user *user)
    {
        rw_mutex.lock();
        auto pos = std::find_if(users.begin(), users.end(), [user](user_ptr &ptr)
        {
            return ptr.get() == user;
        });
#ifdef DEBUG
        assert(pos != users.end());
#endif
        users.erase(pos);
        rw_mutex.unlock();
#ifdef DEBUG
        std::cout << "User leaved" << std::endl;
#endif
    }

    void deliver_message_to_all()
    {
        rw_mutex.lock_shared();
        for(auto &user_p: users)
        {
            user_p->deliver_message();
        }
        rw_mutex.unlock_shared();
    }

private:
    std::vector<user_ptr> users;
    boost::shared_mutex rw_mutex;
};

class user : public chat_user
{
public:
    user(ip::tcp::socket socket, chat_room &chat)
        :
          socket(std::move(socket)),
          chat(chat)
    {
    }

    void accept_message()
    {
        socket.close();
        chat.on_user_leave(this);
    }

    void deliver_message()
    {
        // TODO
    }

private:
    ip::tcp::socket socket;
    chat_room &chat;
};

class connection_handler
{
public:
    connection_handler(io_service &service, int port, chat_room &chat)
        :
          sock(service),
          acc(service, ip::tcp::endpoint(ip::tcp::v4(), port)),
          chat(chat)
    {
        accept_new_connection();
    }

private:
    void accept_new_connection()
    {
        acc.async_accept(sock, [this](boost::system::error_code)
        {
            chat.add_new_user(std::make_shared<user>(std::move(sock), chat));
            accept_new_connection();
        });
    }

private:
    ip::tcp::socket sock;
    ip::tcp::acceptor acc;
    chat_room &chat;
};

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cout
                << "Usage: server $PORT$ $CONCURRENCY$"
                << std::endl;
        return -1;
    }
    size_t port = std::atoi(argv[1]);
    size_t concurrency_level = std::atoi(argv[2]);

    std::cout << "Starting chat server on port "
              << port
              << " with concurrency level "
              << concurrency_level
              << " ..."
              << std::endl;

    io_service service(concurrency_level);
    chat_room chat;
    connection_handler handler(service, port, chat);

    boost::thread_group workers;
    auto const worker =
            boost::bind(&io_service::run, &service);
    for (size_t idx = 0; idx < concurrency_level; ++idx)
    {
        workers.create_thread(worker);
    }

    std::cout << "The server is started" << std::endl;
    std::cout << "Press 'enter' to stop the server"
              << std::endl;
    std::cin.get();
    std::cout << "Stopping the server..." << std::endl;
    service.stop();
    workers.join_all();
    std::cout << "The server is stopped" << std::endl;

    return 0;
}

