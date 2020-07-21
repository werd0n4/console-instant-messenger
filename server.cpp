#include "Connection.cpp"

using namespace boost::asio;

int main(int argc, char* argv[])
{
    if(argc != 2){
        std::cerr << "Usage: ./client.o <port>" << std::endl;
        return 1;
    }

    io_context io_context;
    ip::tcp::socket socket(io_context);
    ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi(argv[1]));
    ip::tcp::acceptor acceptor(io_context, endpoint);
        
    Connection server{socket, acceptor};

    return 0;
}