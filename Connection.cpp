#include <iostream>
#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace boost::asio;

class Connection{
private:
    std::atomic_bool running;
    std::string msg_in, msg_out;
    ip::tcp::socket& socket;
    std::thread read_thrd, write_thrd;

public:
    //client constructor
    Connection(io_context& io_context, ip::tcp::socket& _socket, ip::tcp::endpoint& endpoint) : socket(_socket){
        try
        {
            socket.connect(endpoint);
            std::cout << "Connection established!" << std::endl;

            running.store(true);
            read_thrd = std::thread([this]{read_msg();});
            write_thrd = std::thread([this]{write_msg();});
            read_thrd.join();
            write_thrd.join();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << '\n';
        }
    }

    Connection(io_context& io_context, ip::tcp::socket& _socket, ip::tcp::endpoint& endpoint, ip::tcp::acceptor& acceptor) : socket(_socket){//server constructor
        try
        {
            acceptor.accept(socket);
            std::cout << "Connection established!" << std::endl;

            running.store(true);
            read_thrd = std::thread([this]{read_msg();});
            write_thrd = std::thread([this]{write_msg();});
            read_thrd.join();
            write_thrd.join();
        }
        catch(const std::exception& e)
        {
            std::cerr <<"Exception: " << e.what() << '\n';
        }
    }

    void read_msg(){
        while(running){
            streambuf buf;

            try{
                read_until(socket, buf, "");
                msg_in = buffer_cast<const char*>(buf.data()); 
                std::cout << "$> " + msg_in << std::endl;
                if(msg_in == "exit"){
                    std::cout << "End connection" << std::endl;
                    running=false;
                    break;
                }
            }catch(std::exception& e){
                running = false;
                std::cerr << "Exception: " << e.what() << '\n';
            }
        }
    }

    void write_msg(){
        while(running.load()){
            std::getline(std::cin, msg_out);

            if(msg_out == "exit"){
                running=false;
                break;
            }

            try{
                write(socket, boost::asio::buffer(msg_out));
            }catch(const std::exception& e){
                running=false;
                std::cerr << "Exception: " << e.what() << '\n';
            }
        }
    }
};