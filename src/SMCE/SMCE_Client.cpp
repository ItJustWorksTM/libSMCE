/*
 *  SMCE_Client.cpp
 *  Created by Rylander and Mejborn, Team 1, DAT265.
 *
 *  A terminal interface for libSMCE, that allows sketches to be ran without a GUI.
 *
*/

#ifndef SMCE_RESOURCES_DIR
#    error "SMCE_RESOURCES_DIR is not set"
#endif

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

void print_help(const char* argv0) {
    std::cout << "Usage: " << argv0 << " <fully-qualified-board-name> <path-to-sketch>" << std::endl;
}

// Prints a simple command menu for SMCE_Cli
void print_menu(){
    std::cout << "SMCE Client menu:" << std::endl;
    std::cout << "-h -> see menu" << std::endl;
    std::cout << "-s -> start/resume" << std::endl;
    std::cout << "-p -> pause or resume" << std::endl;
    std::cout << "-r -> reset board" << std::endl;
    std::cout << "-io <pin> <value> -> set a specific value on pin" << std::endl;
    std::cout << "-m <message> -> send message to board" << std::endl;
    std::cout << "-q -> quit" << std::endl;
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

    std::cout << "Compiling..." << std::endl;
    // Compile the sketch on the toolchain
    if (const auto ec = toolchain.compile(sketch)) {
        std::cerr << "Error: " << ec.message() << std::endl;
        auto [_, log] = toolchain.build_log();
        if (!log.empty())
            std::cerr << log << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Creating board..." << std::endl;
    // Create the virtual Arduino board
    smce::Board board;
    board.attach_sketch(sketch);

    // clang-format off
    smce::BoardConfig board_conf{
      .uart_channels = { {} },
      .sd_cards = { smce::BoardConfig::SecureDigitalStorage{ .root_dir = "." } }
    };
    board.configure(std::move(board_conf));
    // clang-format on

    std::cout << "Done" << std::endl;

    // Print command menu
    print_menu();

    // Main loop, handle the command input
    while(true){
        bool suspended = false;
        std::cout << "$>";
        std::string input;
        std::getline(std::cin,input);
        //TODO
            //Split input, check if it has more then one string.
            // If so its a message, error on more then 2
        if (input == "-h"){
            print_menu();
        }else if (input == "-s"){
            if(!board.start()){
                std::cerr << "Error: Board failed to start sketch" << std::endl;
                return EXIT_FAILURE;
            }
        }else if (input == "-p"){
            suspended = board.suspend();
            if(!suspended){
                suspended = board.suspend();
                std::cout << "Board paused" << std::endl;
            }else if(suspended){
                board.resume();
                suspended = false;
                std::cout << "Board resumed" << std::endl;
            }
        }else if (input.starts_with("-m ")){
            //TODO
            std::cout << "message" << " " << input.substr(2) << std::endl;
        }else if (input == "-q"){
            std::cout << "Quitting..." << std::endl;
            board.stop();
            break;
        }else if (input.starts_with("-io ")){
            //TODO
            std::cout << "GPIO pins..." << std::endl;
        }else{
            std::cout << "Unknown input, try again." << std::endl;
        }
    }



    std::cout << "END";
}