#include <iostream>
#include <fstream>
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
        bool file_sending = false;

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
                }else if(msg_in == "sendf"){
                    file_sending = true;
                }else if(file_sending){
                    std::streampos size;
                    std::ofstream file("odebrane.txt", std::ios::out | std::ios::app | std::ios::binary);

                    file.write(buffer_cast<const char*>(buf.data()), buf.size());
                    file.close();
                    if(size != 512)
                        file_sending = false;
                }else{
                    std::cout << "$> " + msg_in << std::endl;
                }
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
                    write(socket, boost::asio::buffer("sendf"));

                    std::streampos size;
                    char* memblock;
                    std::string fpath = msg_out.substr(msg_out.find(" ") + 1);
                    std::ifstream file(fpath, std::ios::in|std::ios::binary|std::ios::ate);

                    size = file.tellg();
                    memblock = new char[size];
                    file.seekg(0, std::ios::beg);
                    file.read(memblock, size);
                    file.close();

                    write(socket, boost::asio::buffer(memblock, size));
                }else{
                    write(socket, boost::asio::buffer(msg_out));
                }
            }catch(const std::exception& e){
                std::cerr << "Exception: " << e.what() << '\n';
            }
        }
    }
};