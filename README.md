# HD - HexaDecimal
Run `sudo make install` or `sudo make uninstall`. Run `hd help` to see options. Note most operators have a symbol version which is also usable, but not shown below to avoid needing to escape `;` for example, in the terminal. Calculations are current in RPN (Reverse Polish Notation). Numbers are represented in 64 bits unless explicitly told to use 32 bits.

## Simple Usage
```bash
$ hd 10
0xA

$ hd 0x10
16

$ hd 0b1010
10

$ hd -5 10 add
5

$ hd 3.14159 hex as
0x400921F9F01B866E

$ hd --32 3.14159 hex as
0x40490FD0

$ hd 12.3456 int cast
12

$ hd 10.2 bin as
0b100000000100100011001100110011001100110011001100110011001100110

$ hd 10 sep 50 sep 100
0xA 0x32 0x64

$ hd 10 sep 50 end 100
0xA 0x32
0x64

$ hd --ord a
97

$ hd --chr 97
a
```

## TODO
* Allow underscores to make it easier to enter and display
