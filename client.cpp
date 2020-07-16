#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace boost::asio;

int main(int argc, char* argv[])
{
    if(argc != 2){
        std::cout << "Usage `./client.o <host address>`" << std::endl;
        return 1;
    }
    

        std::string data;
        boost::system::error_code error;
    try
    {
        io_context io_context;
        ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi(argv[1]));

        for(;;){
            streambuf message;
            ip::tcp::socket socket(io_context);
            socket.connect(endpoint);
            read_until(socket, message, "");

            data = buffer_cast<const char*>(message.data()); 
            std::cout << data << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return 1;
}