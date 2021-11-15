#include <UartToFile.hpp>
#include <ctime>

#include <fstream>
#include <iostream>

using namespace std;

std::string get_time(){
    time_t currentTime;
    struct tm *localTime;

    time( &currentTime );
    localTime = localtime( &currentTime );

    int hour   = localTime->tm_hour;
    int min    = localTime->tm_min;
    int sec    = localTime->tm_sec;
    return(to_string(hour)+":"+to_string(min)+":"+to_string(sec));
}

int uart_to_file(std::string message, std::string path){
    ofstream file;
    file.open(path, std::ios_base::app);
    file << get_time() +": " +message + "\n";
    file.close();
    return 0;
}
