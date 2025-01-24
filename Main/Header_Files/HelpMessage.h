const char *helpMessage = R""""(
usage: pasn [-h | --help] [-i | --infile <filename>] [-o | --outfile <filename>]
            [-d | --device <deviceName>] [-t | --time <time>]

Requires superuser rights to listen to device.

Default:
  Listening to system default interface for 60 seconds.
  Writes csv file to "pasn.csv".

--infile - Set input filename (.pcap) to parse (ignore everything after the first ".")
--outfile - Set output filename (.csv)
--device - Set network interface name
--time - Set time for listening in seconds (if infile is specified, time is ignored)
)"""";
