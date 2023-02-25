#include <sstream>
#include <fstream>
#include "SimplePocoHandler.h"
#include "Req.cpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/json_parser/error.hpp>
#include <exception>
#include <nlohmann/json.hpp>
using boost::property_tree::ptree;
using json = nlohmann::json;

std::string mod_console(const std::string& cmd){
    fflush(stdout);
    fflush(stderr);
    int old_out=dup(fileno(stdout));
    int old_err= dup(fileno(stderr));
    FILE *f = tmpfile();
    dup2(fileno(f),fileno(stderr));
    dup2(fileno(f),fileno(stdout));
    system(cmd.c_str());
    const int ret_size = ftell(f);
    rewind(f);
    std::string ret(ret_size, '\00');
    size_t remaining = ret_size;
        while (remaining > 0)
        {
            const size_t read_size = fread(&ret[ret_size - remaining], 1, remaining, f);
            remaining -= read_size;
            if (read_size == 0)
            {
                throw std::runtime_error("WTF!?");
            }
        }
    fclose(f);
    dup2(old_out, fileno(stdout));
    dup2(old_err, fileno(stderr));
    close(old_out);
    close(old_err);
    return ret;
}

std::string name;

int main(void)
{
    json config;
    std::ifstream f ("user.txt");
    std::string pass;
    std::ifstream con_file("../src/config.json");
    if (con_file.is_open())
    {
        con_file>>config;
    }
    std::cout<<config.at("Service").at("host").get<std::string>();
    if (!f.is_open())
    {
        std::cout << "Username: ";
        std::cin >> name;
        std::cout << "\nPassword: ";
        std::cin >> pass;
        std::ofstream of("user.txt");
        of<<name<<'\n';
        of<<pass;
        of.close();
    }
    else
    {
        f >> name;
        f >> pass;
    }
    Req r(config.at("Service").at("host").get<std::string>(),config.at("Service").at("port").get<std::string>());
    std::string body="{\"user\":\"";
    body+=name;
    body+="\",\"pass\":\"";
    body+=pass;
    body+="\"}";
    if (r.Request("/Users",body).get<std::string>("messages")=="error")
        return -1;
    SimplePocoHandler handler(config.at("Rabbit").at("host").get<std::string>(), std::stoi(config.at("Rabbit").at("port").get<std::string>()));
    AMQP::Connection connection(&handler, AMQP::Login(name, pass), "/");
    AMQP::Channel channel(&connection);
    channel.consume(name + "'s_queue", AMQP::noack).onReceived([&channel](const AMQP::Message& message,
        uint64_t deliveryTag,
        bool redelivered)
        {
            std::string cmd (message.body(),message.body()+message.bodySize());
            int p=cmd.find(';');
  	        std::string index=cmd.substr(0,p);
            cmd.erase(0,p+1);
            std::cout<<"Выполнить: "<<cmd<<"\n\n";
            std::string otv=mod_console(cmd);
            std::cout<<"Результат: "<<otv<<"\n\n\n";
            otv=index+';'+name+';'+otv;
            channel.publish("", "response", otv);
        });

    std::cout << " [x] Waiting for commands" << std::endl;
    handler.loop();
    return 0;
}
