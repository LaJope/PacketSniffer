name: pasn 
arguments:
1. No arguments
    Listens on default device for a minute.
    Writes to "./packageSniffer.csv"
2. -i "file.pcap" / --infile "flie.pcap"
    Parses "file.pcap" and writes to "./packageSniffer.csv"
3. -o "filename" / --outfile "filename"
    Writes to "./filename.csv"
4. -t "num" / --time "num"
    If listens to device, specifies time in seconds.
5. -d "device name" / --device "device name"
    Listens to specific device.
6. -h / --help
    List help.

Cannot parse file and listen to device at the same time.
