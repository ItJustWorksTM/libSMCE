

#ifndef SMCE_UARTTOFILE_HPP
#define SMCE_UARTTOFILE_HPP

#include<string>

std::string get_time();

int uart_to_file(std::string message,std::string path);

#endif