# libSMCE command line frontend

Like SMCE, this sample program requires a C++20 toolchain (for `<span>` mostly).

**IMPORTANT**  
As is, SMCE_Client is not guaranteed to work on Windows machines!
Due to a bug that causes the running sketch to crash when sending or reciving messages on uart.


## Build instructions
To be able to build SMCE_client, the enviroment variable SMCE_ROOT needs to point to a current installed release of libSMCE.
These can be found under releases on git. Or it can be build directly from the source code with the commands below.
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

You can now find the executable "SMCE_client" in the `./build` directory.

## Run instructions
```
SMCE_client -f <fqbn> -p <path_to_sketch>
```
where
- FQBN: [Fully Qualified Board Name](https://arduino.github.io/arduino-cli/latest/FAQ/#whats-the-fqbn-string)
    Testing has been done with fqbn = arduino:sam:arduino_due_x
- Sketch path: Relative or absolute path to the sketch to run

### Start arguments
-f,--fqbn <fqbn>                    -> <fqbn> = Fully qualified board name 
-p,--path <path_to_sketch>          -> <path_to_sketch> = Relative or absolute path to the sketch to run
-d,--dir <arduino_root_dir>         -> <arduino_root_dir> = Relative or absolute path to desired location of arduino root folder
-s,--SMCE <smce_resource_dir>       -> <smce_resource_dir> = Relative or absolute path to SMCE\_RESOURCE folder
-u,--file <write_to_file>           -> <write_to_file> = Set to true if uart should write to file (created in the set arduino root folder)

(-s or -- SMCE, can be used if binary is not compiled and already linked to the SMCE_RESOURCE folder for the current computer.) 

## Configuration of board

As is, configuring of GPIO pins on the board is done in the source file SMCE_Client.cpp, as seen here:

```
smce::BoardConfig board_conf{                                                                                                
        .pins = {0,1},                                                                          
        .gpio_drivers = {                                                                                                        
            smce::BoardConfig::GpioDrivers{0,                                                                                    
            smce::BoardConfig::GpioDrivers::DigitalDriver{true,true},                                                           
            smce::BoardConfig::GpioDrivers::AnalogDriver{false,false}                                                             
            },                                                                                                                   
            ...
            ...                                                                                                             
        }
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