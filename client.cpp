#include "Connection.cpp"

int main(int argc, char* argv[])
{
    if(argc != 2){
        std::cerr << "Usage: ./server.o <port>" << std::endl;
        return 1;
    }

    io_context io_context;
    ip::tcp::socket socket(io_context);
    ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi(argv[1]));

    Connection client{socket, endpoint};

    return 0;
}