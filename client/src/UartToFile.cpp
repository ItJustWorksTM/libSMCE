#include <UartToFile.hpp>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <iostream>


std::string get_time(){
    auto time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm time = *std::localtime(&time_t);
    std::stringstream ss;
    ss << std::put_time(&time, "%T");
    return ss.str();
}

int uart_to_file(std::string message, std::string path){
    std::ofstream file;
    file.open(path, std::ios_base::app);
    file << get_time() +": " +message + "\n";
    file.close();
    return 0;
}
