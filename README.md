# ft_ping

`ft_ping` is a command line utility that imitates the behavior of the `ping` command, with the added ability to include a payload in the ICMP message.

## Installation

To install `ft_ping`, clone the repository and compile the source code using a C compiler.

```
git clone https://github.com/jguyet/ft_ping.git
cd ft_ping
make
```

## Usage

To use `ft_ping`, open a terminal and type the command followed by the hostname or IP address of the target host:

`ft_ping google.com`


By default, `ft_ping` will send four ICMP Echo Request messages to the target host, with a 1-second interval between each message. You can customize the number of messages and the interval using the `-c` and `-i` options, respectively:

You can also include a payload in the ICMP message using the `-g` option: 

`ft_ping -g 100 google.com`

For a full list of options, use the `--help` flag:

`ft_ping --help`

## License

`ft_ping` is released under the [MIT License](LICENSE).
