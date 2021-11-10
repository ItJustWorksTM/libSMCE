# libSMCE command line frontend

Like SMCE, this sample program requires a C++20 toolchain (for `<span>` mostly).

## Build instructions
To beable to build, the enviroment variable SMCE_ROOT needs to point to a current installed release.
These can be found under releases on git. Or be build directly from the source code with the commands below.
These should be ran in the libSMCE folder. 
```shell
cmake -S . -B build/
cmake --build build/
cmake --build build/ --config Release
cmake --install build/ --prefix <path to installation folder>
```

After SMCE_ROOT is added, run:

```shell
cd client
cmake -S . -B build/
cmake --build build/
```

You can now find the executable in the `./build` directory.

### Run instructions
```
SMCE_client <fqbn> <sketch-path>
```
where
- FQBN: [Fully Qualified Board Name](https://arduino.github.io/arduino-cli/latest/FAQ/#whats-the-fqbn-string)
- Sketch path: Relative or absolute path to the sketch to run

#### Start arguments
    TODO

### Configuration of board

As is, configuring of GPIO pins on the board is done in the source file SMCE_Client.cpp, as seen here:

```
smce::BoardConfig board_conf{                                                                                                
        .pins = {0,1},                                                                          
        .gpio_drivers = {                                                                                                        
            smce::BoardConfig::GpioDrivers{0,                                                                                    
            smce::BoardConfig::GpioDrivers::DigitalDriver{true,true},                                                           
            smce::BoardConfig::GpioDrivers::AnalogDriver{true,true}                                                             
            },                                                                                                                   
            ....                                                                                                             
        } ......
```
.pins = a list of all a pins on the board. 
.gpio_drivers = specifies the drivers for each pin, configured as: 
``` 
smce::BoardConfig::GpioDrivers{<pin>,                                                                                    
            smce::BoardConfig::GpioDrivers::DigitalDriver{<read>,<write>},                                                     
            smce::BoardConfig::GpioDrivers::AnalogDriver{<read>,<write>}                                                             
            }
```
DigitalDriver and AnalogDriver has two parameters {<read>,<write>}, these are set as true or false depending on what the pin should be able to do.