# PacketSniffer
This is a program to either listen to a network device or parse a .pcap file. The output of the program is a csv file with source/distanation IPs/Ports grouped together and size of packets within those groups counted.

## Help
usage: pasn [-h | --help] [-i | --infile <filename>] [-o | --outfile <filename>]
            [-d | --device <deviceName>] [-t | --time <time>]

***Requires superuser rights to listen to device.***

Default:
  Listening to system default interface for 60 seconds.
  Writes csv file to "pasn.csv".

--infile - Set input filename (.pcap) to parse (ignore everything after the first ".")  
--outfile - Set output filename (.csv)  
--device - Set network interface name  
--time - Set time for listening in seconds (if infile is specified, time is ignored)  

## Dependencies
For building the project you need to install gcc, cmake, flex, bison, libpcap.
You can install them using your prefered package manager. For example:
#### Ubuntu
```bash
sudo apt install gcc cmake flex bison libpcap-dev
```
#### Arch linux
```bash
sudo pacman -S gcc cmake flex bison libpcap
```

## Build
To build the program in project root directory type:
```bash
cmake -S . -B build ;
cmake --build build
```
The executable will be placed in ./build/bin/

# PcapPlusPlus library used in this project
https://github.com/seladb/PcapPlusPlus.git
