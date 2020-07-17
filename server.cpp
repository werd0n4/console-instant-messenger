#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using namespace boost::asio;

void read(ip::tcp::socket& socket){
    while(true){
        streambuf str;
        read_until(socket, str, "");
        std::string data;
        data = buffer_cast<const char*>(str.data()); 
        std::cout << data << std::endl;
    }
}

void write(ip::tcp::socket& socket){
    while(true){
        std::string message;
        std::getline(std::cin, message);

        boost::system::error_code ignored_error;
        write(socket, boost::asio::buffer(message), ignored_error);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2){
        std::cerr << "Usage: ./client.o <port>" << std::endl;
        return 1;
    }

    try
    {
        std::thread read_thd;
        std::thread write_thd;

        io_context io_context;
        ip::tcp::socket socket(io_context);

        ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi(argv[1]));
        ip::tcp::acceptor acceptor(io_context, endpoint);
        
        acceptor.accept(socket);
        std::cout << "Connection established!" << std::endl;

        write_thd = std::thread{[&socket]{write(socket);}};
        read_thd = std::thread{[&socket]{read(socket);}};
        write_thd.join();
        read_thd.join();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    

    return 0;
}