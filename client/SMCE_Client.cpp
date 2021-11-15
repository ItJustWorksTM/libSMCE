/*                                                                                                                               
*  client/SMCE_Client.cpp                                                                                                        
*  Created by Rylander and Mejborn, Team 1, DAT265.                                                                              
*                                                                                                                                
*  A terminal interface for libSMCE, that allows sketches to be ran without the use of smce-gd.                                  
*  Functionally to test and debug sketches, as GPIO pins can be set with values and                                              
*  messages can be sent to board through uart.                                                                                   
*                                                                                                                                
*/                                                                                                                               
                                                                                                                                 
#ifndef SMCE_RESOURCES_DIR                                                                                                       
#    error "SMCE_RESOURCES_DIR is not set"                                                                                       
#endif                                                                                                                           
                                                                                                                                 
#include <bit>                                                                                                                   
#include <atomic>                                                                                                                
#include <chrono>                                                                                                                
#include <cstdlib>                                                                                                               
#include <iostream>                                                                                                              
#include <string_view>                                                                                                           
#include <thread>                                                                                                                
#include <utility>                                                                                                               
#include <string>                                                                                                                
#include <vector>
#include <termcolor.hpp>  
#include <lyra.hpp>
#include <UartToFile.hpp>                                                                                                                   
#include <SMCE/Board.hpp>                                                                                                        
#include <SMCE/BoardConf.hpp>                                                                                                    
#include <SMCE/BoardView.hpp>                                                                                                    
#include <SMCE/Sketch.hpp>                                                                                                       
#include <SMCE/SketchConf.hpp>                                                                                                   
#include <SMCE/Toolchain.hpp>
                                                                                               
                                                                                                                                 
using namespace std::literals;                                                                                                   
                                                                                                                                                                                                                                                                                                    
void print_help(const char* argv0){                                                                                              
    std::cout << "Usage: " << argv0 << " <fully-qualified-board-name> <path-to-sketch>" << std::endl;                            
}                                                                                                                                
                                                                                                                                 
void exit_sketch(int code){                                                                                                      
    std::cerr << "Error code:  "<< code << std::endl;                                                                            
}                                                                                                                                

// automic bool for handling threads                                                                                             
std::atomic_bool run_threads = true;
std::atomic_bool mute_uart = false;
std::mutex t_lock;                                                                                                                                                                                                                                                        
//Listen to the uart input from board, writes what it read to terminal                                                           
void uart_listener(smce::VirtualUart uart,bool file_write,std::string path){                                                                                      
    auto tx = uart.tx();
    std::string file = path+"/uartlog"+get_time()+".txt";                                                                                                         
    while(run_threads){                                                                                                          
        std::string buffer;
        t_lock.lock(); // if recompiling sketch, lock thread here untill recompiling is done.                                                                                                 
        buffer.resize(tx.max_size());                                                                                            
        const auto len = tx.read(buffer);
        t_lock.unlock();                                                                                        
        if(len == 0){                                                                                                            
            std::this_thread::sleep_for(1ms);                                                                                    
            continue;                                                                                                            
        }                                                                                                                        
        buffer.resize(len);
        if(file_write){
            uart_to_file(buffer,file);
        }else{
            if(!mute_uart){
                std::cout << termcolor::red << buffer << termcolor::reset <<std::endl << "$>" << std::flush; 
            }  
        }                                                                                                                                                                             
    }                                                                                                                            
}                                                                                                                                
// Prints a command menu for SMCE_Client                                                                                         
void print_menu(){                                                                                                               
    std::cout << "SMCE Client menu:" << std::endl;                                                                               
    std::cout << "-h -> See menu" << std::endl;                                                                                  
    std::cout << "-p -> Pause or resume the board" << std::endl;                                                                 
    std::cout << "-rc -> Recompile the sketch" << std::endl;                                                                     
    std::cout << "-m <message> -> Send message to board through uart (serial)" << std::endl;
    std::cout << "-mt -> disable uart prints in console" << std::endl;                                     
    std::cout << "-wa <pin> <value> -> Set a specific value on a analog pin" << std::endl;                                       
    std::cout << "-wd <pin> <value> -> Set a specific value on a digital pin, value should be 0 or 1" << std::endl;              
    std::cout << "-ra <pin> -> Read the value on a analog pin" << std::endl;                                                     
    std::cout << "-rd <pin> -> Read the value on a digital pin" << std::endl;                                                    
    std::cout << "-q -> Power off board and quit program" << std::endl << std::endl;                                             
}                                                                                                                                
                                                                                                                                   
int compile_sketch (smce::Sketch &sketch,smce::Toolchain &toolchain ,smce::Board &board, std::string arduino_root_dir) {                                       
    // Compile the sketch on the toolchain                                                                                                                                                                 
    if (const auto ec = toolchain.compile(sketch)) {                                                                             
        std::cerr << "Error: " << ec.message() << std::endl;                                                                     
        auto [_, log] = toolchain.build_log();                                                                                   
        if (!log.empty())                                                                                                        
            std::cerr << log << std::endl;                                                                                       
        return EXIT_FAILURE;                                                                                                     
    }                                                                                                                                                                                                                   
    board.attach_sketch(sketch);                                                                                                 
    // Create Board Config                                                                                                       
    smce::BoardConfig board_conf{                                                                                                
        .pins = {0,1}, //Creating pins & GPIO drivers                                                                            
        .gpio_drivers = {                                                                                                        
            smce::BoardConfig::GpioDrivers{0,                                                                                    
            smce::BoardConfig::GpioDrivers::DigitalDriver{true, true},                                                           
            smce::BoardConfig::GpioDrivers::AnalogDriver{true, true}                                                             
            },                                                                                                                   
            smce::BoardConfig::GpioDrivers{1,                                                                                    
            smce::BoardConfig::GpioDrivers::DigitalDriver{false, true},                                                          
            smce::BoardConfig::GpioDrivers::AnalogDriver{false, true}                                                            
            }                                                                                                                    
        },                                                                                                                       
        .uart_channels = { {} }, // use standard configuration of uart_channels                                                  
        .sd_cards = { smce::BoardConfig::SecureDigitalStorage{ .root_dir = arduino_root_dir } }                                               
    };                                                                                                                                                                                                            
    board.configure(std::move(board_conf));                                                                                      
    return 0;                                                                                                                    
}

int compile_and_start(smce::Sketch &sketch,smce::Toolchain &toolchain ,smce::Board &board, std::string arduino_root_dir){
    if(board.status() == smce::Board::Status::running){
        board.stop();                                                                                                                                                                        
        board.reset();   
    }
    compile_sketch(sketch,toolchain,board,arduino_root_dir);                                                                                                                                           
    //Start board                                                                                                                
    if (!board.start()) {                                                                                                        
        std::cerr << "Error: Board failed to start sketch" << std::endl;                                                         
        return EXIT_FAILURE;                                                                                                     
    }else{                                                                                                                       
        return 0;                                                                              
    }   
}
                                                                                                                                 
int main(int argc, char** argv){  

    std::string fqbn;                                                                                                        
    std::string path_to_sketch;
    std::string arduino_root_dir = ".";    // path to root dir for arduino, standard is in start folder.
    std::string smce_resource_dir = SMCE_RESOURCES_DIR; // smce_resource_dir path, given at runtime. 
    bool file_write = false; //DEFAULT is to write in console
    bool show_help = false;

    //Setup lyra start arguments for the parser. 
    auto cli 
            = lyra::help(show_help)
            | lyra::opt(fqbn,"fqbn")
                ["--fpqn"]["-f"]("Fully qualified board name")
            | lyra::opt(path_to_sketch,"path-to-sketch")
                ["--path"]["-p"]("The path to the sketch")
            | lyra::opt(arduino_root_dir,"Ardunio home folder")
                ["--dir"]["-d"]("The absolute path to the desired location of arduino root")
            | lyra::opt(smce_resource_dir,"Alternativ path to SMCE_RESOURCE") // Makes it possible to change path to SMCE_RESOURCES at start.
                ["--SMCE"]["-s"]("Tha alternative path to SMCE_RESOURCES for runtime, should be absolute.")
            | lyra::opt(file_write,"file_write")
                ["--file"]["-u"]("Set to true if uart should write to file (created in the set arduino root folder)");

    auto result = cli.parse({argc,argv});

    // If something is not right with parser of input, show error
    if(!result){
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        return 1;
    }         
    // If user input -h or --help, display help.                                                 
    if(show_help) 
    {
        std::cout << cli << "\n";
        return 0;
    }
                                                                                          
                                                                                                                                 
    std::cout << std::endl << "Starting SMCE Client" << std::endl;                                                               
    std::cout << "Given Fqbn: " << fqbn << std::endl << "Given path to sketch: " << path_to_sketch << std::endl;                      
                                                                                                                                 
     // Create the toolchain                                                                                                     
     smce::Toolchain toolchain{smce_resource_dir};    
     if (const auto ec = toolchain.check_suitable_environment()) {                                                               
         std::cerr << "Error: " << ec.message() << std::endl;                                                                    
         return EXIT_FAILURE;                                                                                                    
     }                                                                                                                           
                                                                                                                                 
    // Create the sketch, and declare that it requires the WiFi and MQTT Arduino libraries during preprocessing                  
    // clang-format off                                                                                                          
    smce::Sketch sketch{path_to_sketch, {                                                                                        
        .fqbn = fqbn,                                                                                                            
        .legacy_preproc_libs = { {"WiFi"}, {"MQTT"} }                                                                            
    }};                                                                                                                          
    // // clang-format on                                                                                                        
                                                                                                                                 
    // Create the virtual Arduino board                                                                                          
    std::cout << "Creating board and compiling sketch" << std::endl;                                                                               
    smce::Board board(exit_sketch);
    //Compile and start board
    compile_and_start(sketch,toolchain,board,arduino_root_dir);                                                                                      
    std::cout << "Complete" << std::endl;                                                                                                                                                                                                                                          
    //Create view and uart (serial) channels                                                                                     
    auto board_view = board.view();                                                                                              
    auto uart0 = board_view.uart_channels[0];                                                                                    
                                                                                                                                 
    //start listener thread for uart                                                                                             
    std::thread uart_thread{[=] {uart_listener(uart0,file_write,arduino_root_dir);} };                                                                       
    std::cout << "Messages recived on uart from arduino is shown as " 
        << termcolor::red << "red" << termcolor::reset << " text." << std::endl;                                                                                                                             
    // Print command menu                                                                                                        
    print_menu();                                                                                                                                                                                                                                              
    // Main loop, handle the command input                                                                                       
    bool suspended = false;                                                                                                      
    while(true){                                                                                                                 
        std::cout << "$>";                                                                                                       
        std::string input;                                                                                                       
        std::getline(std::cin,input);                                                                                            
        board.tick();                                                                                                            
        if (input == "-h"){ //help menu                                                                                          
            print_menu();

        }else if (input == "-p"){ //pause or resume                                                                               
            if(!suspended){                                                                                                      
                suspended = board.suspend();                                                                                     
                if(suspended)                                                                                                    
                    std::cout << "Board paused" << std::endl;                                                                    
                else                                                                                                             
                    std::cout << "Board could not be paused" << std::endl;                                                       
            }else if(suspended){                                                                                                 
                board.resume();                                                                                                  
                suspended = false;                                                                                               
                std::cout << "Board resumed" << std::endl;                                                                       
            }

        }else if (input == "-rc"){ //recompile
            std::cout << "Recompiling.." << std::endl; 
            t_lock.lock(); // aquire lock before recompiling, so uart_listener is forced to wait                                                                                                                                                                        
            compile_and_start(sketch,toolchain,board,arduino_root_dir);
            // update boardview and uart0 after the sketch has been recompiled.
            board_view = board.view();                                                                                       
            uart0 = board_view.uart_channels[0];                                                                                                   
            t_lock.unlock(); // unlock as recompile is done                                                                        
            std::cout << "Complete" << std::endl;

        }else if (input.starts_with("-m ")){ //send message on uart                                                              
                std::string message = input.substr(3);                                                                           
                for(std::span<char> to_write = message; !to_write.empty();){                                                     
                    const auto written_count = uart0.rx().write(to_write);                                                       
                    to_write = to_write.subspan(written_count);                                                                  
                }

        }else if(input.starts_with("-mt")){
            if(mute_uart){
                mute_uart = false;
                std::cout <<"Unmuted uart"<<std::endl;
            }else{
                mute_uart = true;
                std::cout <<"Muted uart"<<std::endl;
            }

        }else if (input.starts_with("-wa ")){ //write value on analog pin                                                        
            bool write = true;                                                                                                   
            int pin = stoi(input.substr(3,5));                                                                                   
            uint16_t value = stoi(input.substr(5));                                                                              
            auto apin = board_view.pins[pin].analog();                                                                           
            if(!apin.exists()){                                                                                                  
                std::cout << "Pin does not exist!" << std::endl;                                                                 
                write=false;                                                                                                     
            }                                                                                                                    
            if(!apin.can_write()){                                                                                               
                std::cout << "Can't write to pin!" << std::endl;                                                                 
                write=false;                                                                                                     
            }                                                                                                                    
            if(write){                                                                                                           
                apin.write(value);                                                                                               
            }

        }else if (input.starts_with("-wd ")){ //write value on digital pin                                                       
            bool write = true;                                                                                                   
            int pin = stoi(input.substr(3,5));                                                                                   
            uint16_t value = stoi(input.substr(5));                                                                              
            auto apin = board_view.pins[pin].digital();                                                                          
            if(!apin.exists()){                                                                                                  
                std::cout << "Pin does not exist!" << std::endl;                                                                 
                write=false;                                                                                                     
            }                                                                                                                    
            if(!apin.can_write()){                                                                                               
                std::cout << "Can't write to pin!" << std::endl;                                                                 
                write=false;                                                                                                     
            }                                                                                                                    
            if(write){                                                                                                           
                if(value == 0){                                                                                                  
                apin.write(false);                                                                                               
                }else if(value == 1){                                                                                            
                    apin.write(true);                                                                                            
                }else{                                                                                                           
                    std::cout << "Value must be 0 or 1 for digital pins" << std::endl;                                           
                }                                                                                                                
            }

        }else if (input.starts_with("-ra ")){                                                                                    
            bool read = true;                                                                                                    
            int index_pin = stoi(input.substr(3));                                                                               
            auto pin = board_view.pins[index_pin].analog();                                                                      
            if(!pin.exists()){                                                                                                   
                std::cout << "Pin does not exist!" << std::endl;                                                                 
                read=false;                                                                                                      
            }                                                                                                                    
            if(!pin.can_read()){                                                                                                 
                std::cout << "Can't read from pin!" << std::endl;                                                                
                read=false;                                                                                                      
            }                                                                                                                    
            if(read){                                                                                                            
                std::cout << "Value from pin " << index_pin << " is " << pin.read() << std::endl;                                
            } 

        }else if (input.starts_with("-rd ")){
            bool read = true;
            int index_pin = stoi(input.substr(3));
            auto pin = board_view.pins[index_pin].digital();
            if(!pin.exists()){
                std::cout << "Pin does not exist!" << std::endl;
                read=false;
            }
            if(!pin.can_read()){
                std::cout << "Can't read from pin!" << std::endl;
                read=false;
            }
            if(read){
                std::cout << "Value from pin " << index_pin << " is " << pin.read() << std::endl;
            }

        }else if (input == "-q"){ //power off and quit
            std::cout << "Quitting..." << std::endl;
            run_threads = false;
            board.stop();
            uart_thread.join();
            break;

        }else{
            //If input don't match with anything.
            std::cout << "Unknown input, try again." << std::endl;
        }
    }
}                                                                                                                   