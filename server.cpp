#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main(int argc, char* argv[])
{
    if(argc != 2){
        std::cout << "Usage `./server.o <host address>`" << std::endl;
        return 1;
    }

    try
    {
        io_context io_context;
        ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi(argv[1]));
        ip::tcp::acceptor acceptor(io_context, endpoint);

        std::string message;
        for(;;){
            ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::getline(std::cin, message);

            boost::system::error_code ignored_error;
            write(socket, boost::asio::buffer(message), ignored_error);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    

    return 0;
}