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

    //server constructor
    Connection(io_context& io_context, ip::tcp::socket& _socket, ip::tcp::endpoint& endpoint, ip::tcp::acceptor& acceptor) : socket(_socket){
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
        while(running.load()){
            streambuf buf;

            try{
                read_until(socket, buf, "");
                msg_in = buffer_cast<const char*>(buf.data()); 
                if(msg_in == "exit"){
                    std::cout << "$> Connection ended" << std::endl;
                    running.store(false);
                    socket.close();
                    exit(0);
                }else
                    std::cout << "$> " + msg_in << std::endl;

            }catch(std::exception& e){
                std::cerr << "Exception: " << e.what() << '\n';
            }
        }
    }

    void write_msg(){
        while(running.load()){
            std::getline(std::cin, msg_out);

            try{
                if(msg_out == "exit"){
                    running.store(false);
                    write(socket, boost::asio::buffer("exit"));
                    socket.close();
                    exit(0);
                }else if(msg_out.substr(0, msg_out.find(" ")) == "sendf"){
                    std::string fpath = msg_out.substr(msg_out.find(" ") + 1);
                    write(socket, boost::asio::buffer("I wanto to send you: " + fpath));
                }else{
                    write(socket, boost::asio::buffer(msg_out));
                }
            }catch(const std::exception& e){
                std::cerr << "Exception: " << e.what() << '\n';
            }
        }
    }
};