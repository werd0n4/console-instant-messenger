#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main(int argc, char* argv[])
{
    if(argc != 2){
        std::cout << "Usage `./Server some_number` in terminal" << std::endl;
        return 1;
    }

    try
    {
        io_context io_context;
        ip::tcp::socket socket(io_context);
        ip::tcp::endpoint endpoint(ip::tcp::v4(), atoi(argv[1]));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    

    return 0;
}