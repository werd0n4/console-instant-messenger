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
    std::string msg_in, msg_out, new_file_name;
    ip::tcp::socket& socket;
    std::thread read_thrd, write_thrd;
    std::mutex mtx;
    std::condition_variable cv;

    bool file_sending = false;
    bool new_file_has_name = true;

public:
    //client constructor
    Connection(ip::tcp::socket& _socket, ip::tcp::endpoint& endpoint) : socket(_socket){
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
    Connection(ip::tcp::socket& _socket, ip::tcp::acceptor& acceptor) : socket(_socket){
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
                }else if(msg_in == "sendf"){
                    file_sending = true;
                    new_file_has_name = false;
                    std::cout << "$> User wants to send you a file. Type a name for it: " << std::endl;
                }else if(file_sending){
                    if(!new_file_has_name){
                        std::unique_lock<std::mutex> ul(mtx);
                        cv.wait(ul, [this]{return new_file_has_name;});
                    }
                    std::ofstream file(new_file_name, std::ios::out | std::ios::app | std::ios::binary);

                    file.write(buffer_cast<const char*>(buf.data()), buf.size());
                    file.close();
                    if(buf.size() != 512){
                        file_sending = false;
                        new_file_has_name = false;
                    }
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
            if(file_sending && !new_file_has_name){
                new_file_name = msg_out;
                new_file_has_name = true;
                cv.notify_one();
                continue;
            }

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