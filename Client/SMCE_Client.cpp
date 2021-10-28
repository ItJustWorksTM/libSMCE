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
#include <SMCE/Board.hpp>
#include <SMCE/BoardConf.hpp>
#include <SMCE/BoardView.hpp>
#include <SMCE/Sketch.hpp>
#include <SMCE/SketchConf.hpp>
#include <SMCE/Toolchain.hpp>

using namespace std::literals;

// automic bool for handling threads
std::atomic_bool run_threads = true;

void print_help(const char* argv0){
<<<<<<< Updated upstream
    std::cout << "Usage: " << argv0 << " <fully-qualified-board-name> <path-to-sketch>" << std::endl;
}
void exit_sketch(int code){
    std::cerr << "Error code:  "<< std::hex << std::bit_cast<std::uint16_t>(static_cast<int16_t>(code)) << std::endl;
=======
   std::cout << "Usage: " << argv0 << " <fully-qualified-board-name> <path-to-sketch>" << std::endl;
}

void exit_sketch(int code){
   std::cerr << "Error code:  "<< code << std::endl;
>>>>>>> Stashed changes
}

//Listen to the uart input from board, writes what it read to terminal
void uart_listener(smce::VirtualUart uart){
   auto tx = uart.tx();
   while(run_threads){
       std::string buffer;
       buffer.resize(tx.max_size());
       const auto len = tx.read(buffer);
       if(len == 0){
           std::this_thread::sleep_for(1ms);
           continue;
       }
       buffer.resize(len);
       std::cout << buffer;
   }
}
// Prints a command menu for SMCE_Client
void print_menu(){
<<<<<<< Updated upstream
    std::cout << "SMCE Client menu:" << std::endl;
    std::cout << "-h -> See menu" << std::endl;
    std::cout << "-p -> Pause or resume the board" << std::endl;
    std::cout << "-r -> Reset the board" << std::endl;
    std::cout << "-wa <pin> <value> -> Set a specific value on a analog pin" << std::endl;
    std::cout << "-wd <pin> <value> -> Set a specific value on a digital pin" << std::endl;
    std::cout << "-m <message> -> Send message to board through uart (serial)" << std::endl;
    std::cout << "-q -> Power off board and quit program" << std::endl << std::endl;
}

int main(int argc, char** argv){
    if (argc == 2 && (argv[1] == "-h" || argv[1] == "--help")) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    } else if (argc != 3) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }
    //Saves <fully-qualified-board-name> and <path-to-sketch>
    char* fqbn = argv[1];
    char* path_to_sketch = argv[2];

    std::cout << std::endl << "Starting SMCE Client" << std::endl;
    std::cout << "Given Fqbn: " << fqbn << "\n" << "Given path to sketch: " << path_to_sketch << std::endl;


    // Create the toolchain
    smce::Toolchain toolchain{SMCE_RESOURCES_DIR};
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

    // Compile the sketch on the toolchain
    std::cout << "Compiling..." << std::endl;
    if (const auto ec = toolchain.compile(sketch)) {
        std::cerr << "Error: " << ec.message() << std::endl;
        auto [_, log] = toolchain.build_log();
        if (!log.empty())
            std::cerr << log << std::endl;
        return EXIT_FAILURE;
    }

    // Create the virtual Arduino board
    std::cout << "Creating board..." << std::endl;
    smce::Board board(exit_sketch);
    board.attach_sketch(sketch);

    // Create board config
    smce::BoardConfig board_conf{
      .uart_channels = { {} }, // use standard configuration of uart_channels
      .sd_cards = { smce::BoardConfig::SecureDigitalStorage{ .root_dir = "." } }
    };
    board.configure(std::move(board_conf));
    std::cout << "Done" << std::endl;

    //Start board
    if (!board.start()) {
        std::cerr << "Error: Board failed to start sketch" << std::endl;
        return EXIT_FAILURE;
    }else{
        std::cout << "Sketch started" << std::endl;
    }

    //Create view and uart (serial) channels
    auto board_view = board.view();
    auto uart0 = board_view.uart_channels[0];

    //start listener thread for uart
    std::thread uart_thread{[=] {uart_listener(uart0);} };

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
        }else if (input == "-r"){ //reset
            board.reset();
        }else if (input.starts_with("-m ")){ //send message on uart
                std::string message = input.substr(3);
                for(std::span<char> to_write = message; !to_write.empty();){
                    const auto written_count = uart0.rx().write(to_write);
                    std::cout << "Bytes written: " << written_count << std::endl;
                    to_write = to_write.subspan(written_count);
                }
        }else if (input == "-q"){ //power off and quit
            std::cout << "Quitting..." << std::endl;
            run_threads = false;
            board.stop();
            uart_thread.join();
            break;
        }else if (input.starts_with("-wa ")){ //write value on analog pin
            //TODO implement writing to analog GPIO pin.
        }else if (input.starts_with("-wd ")){ //write value on digital pin
            //TODO implement writing to digital GPIO pin.
        }else{
            //If input don't match with anything.
            std::cout << "Unknown input, try again." << std::endl;
        }
    }
=======
   std::cout << "SMCE Client menu:" << std::endl;
   std::cout << "-h -> See menu" << std::endl;
   std::cout << "-p -> Pause or resume the board" << std::endl;
   std::cout << "-r -> Reset the board" << std::endl;
   std::cout << "-wa <pin> <value> -> Set a specific value on a analog pin" << std::endl;
   std::cout << "-wd <pin> <value> -> Set a specific value on a digital pin" << std::endl;
   std::cout << "-m <message> -> Send message to board through uart (serial)" << std::endl;
   std::cout << "-q -> Power off board and quit program" << std::endl << std::endl;
}

int main(int argc, char** argv){
   if (argc == 2 && (argv[1] == "-h" || argv[1] == "--help")) {
       print_help(argv[0]);
       return EXIT_SUCCESS;
   } else if (argc != 3) {
       print_help(argv[0]);
       return EXIT_FAILURE;
   }
   //Saves <fully-qualified-board-name> and <path-to-sketch>
   char* fqbn = argv[1];
   char* path_to_sketch = argv[2];

   std::cout << std::endl << "Starting SMCE Client" << std::endl;
   std::cout << "Given Fqbn: " << fqbn << "\n" << "Given path to sketch: " << path_to_sketch << std::endl;


   // Create the toolchain
   smce::Toolchain toolchain{SMCE_RESOURCES_DIR};
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

   // Compile the sketch on the toolchain
   std::cout << "Compiling..." << std::endl;
   if (const auto ec = toolchain.compile(sketch)) {
       std::cerr << "Error: " << ec.message() << std::endl;
       auto [_, log] = toolchain.build_log();
       if (!log.empty())
           std::cerr << log << std::endl;
       return EXIT_FAILURE;
   }

   // Create the virtual Arduino board
   std::cout << "Creating board..." << std::endl;
   smce::Board board(exit_sketch);
   board.attach_sketch(sketch);

   // Create Board Config
   smce::BoardConfig board_conf{
       .pins = {0,1}, //Creating pins & GPIO drivers
       .gpio_drivers = {
           smce::BoardConfig::GpioDrivers{0,
           smce::BoardConfig::GpioDrivers::DigitalDriver{false, true},
           smce::BoardConfig::GpioDrivers::AnalogDriver{false, true}
           },
           smce::BoardConfig::GpioDrivers{1,
           smce::BoardConfig::GpioDrivers::DigitalDriver{false, true},
           smce::BoardConfig::GpioDrivers::AnalogDriver{false, true}
           }
       },
       .uart_channels = { {} }, // use standard configuration of uart_channels
       .sd_cards = { smce::BoardConfig::SecureDigitalStorage{ .root_dir = "." } },
       .gpio_drivers = { {} } // use standard configuration of gpio_pins
   };
   board.configure(std::move(board_conf));
   std::cout << "Done" << std::endl;

   //Start board
   if (!board.start()) {
       std::cerr << "Error: Board failed to start sketch" << std::endl;
       return EXIT_FAILURE;
   }else{
       std::cout << "Sketch started" << std::endl;
   }

   //Create view and uart (serial) channels
   auto board_view = board.view();
   auto uart0 = board_view.uart_channels[0];

   //start listener thread for uart
   std::thread uart_thread{[=] {uart_listener(uart0);} };

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
       }else if (input == "-r"){ //reset
           board.reset();
       }else if (input.starts_with("-m ")){ //send message on uart
               std::string message = input.substr(3);
               for(std::span<char> to_write = message; !to_write.empty();){
                   const auto written_count = uart0.rx().write(to_write);
                   std::cout << "Bytes written: " << written_count << std::endl;
                   to_write = to_write.subspan(written_count);
               }
       }else if (input == "-q"){ //power off and quit
           std::cout << "Quitting..." << std::endl;
           run_threads = false;
           board.stop();
           uart_thread.join();
           break;

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
           std::cout << "Successful" << std::endl;
           write=false;
           }
           if(write){
           apin.write(value);
           std::cout << "Value from board: " << apin.read();
           }
       }else if (input.starts_with("-wd ")){ //write value on digital pin
           bool write = true;
           int pin = stoi(input.substr(3,5));
           uint16_t value = stoi(input.substr(5));
           auto apin = board_view.pins[pin].digital();
           if(!apin.exists()){
           std::cout << "Pin does not exist!" << std::endl;
           std::cout << "Successful" << std::endl;
           write=false;
           }
           if(!apin.can_write()){
           std::cout << "Can't write to pin!" << std::endl;
           write=false;
           }
           if(write){
           apin.write(value);
           std::cout << "Value from board: " << apin.read();
           }
       }else{
           //If input don't match with anything.
           std::cout << "Unknown input, try again." << std::endl;
       }
   }
>>>>>>> Stashed changes
}