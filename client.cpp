#include <iostream>
#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace boost::asio;

void read(ip::tcp::socket& socket){
    while(true){
        std::string data;
        streambuf message;
        read_until(socket, message, "");

        data = buffer_cast<const char*>(message.data()); 
        std::cout << data << std::endl;
    }
}

void write(ip::tcp::socket& socket){
    while(true){
        std::string message2;
        std::getline(std::cin, message2);

        boost::system::error_code ignored_error;
        write(socket, boost::asio::buffer(message2), ignored_error);
    }
}

int main(int argc, char* argv[])
{
    std::thread read_thd;
    std::thread write_thd;
    boost::system::error_code error;
    try
    {
        io_context io_context;
        ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi("127.0.0.1"));
        ip::tcp::socket socket(io_context);
        socket.connect(endpoint);

        read_thd = std::thread{[&socket]{read(socket);}};
        write_thd = std::thread{[&socket]{write(socket);}};
        read_thd.join();
        write_thd.join();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 1;
}