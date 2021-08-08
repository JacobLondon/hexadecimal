# HD - HexaDecimal
Run `sudo make install` or `sudo make uninstall`. Run `hd --help` to see options. Note most operators have a symbol version which is also usable, but not shown below to avoid needing to escape `;` for example, in the terminal. Calculations are currently in RPN (Reverse Polish Notation). Numbers are represented in 64 bits unless explicitly told to use 32 bits.

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

$ hd "10, 50, 100"
0xA 0x32 0x64

$ hd 10 sep 50 end 100
0xA 0x32
0x64

$ hd --ord a
97

$ hd --chr 97
a

# RPN-only features calling ord on ASCII:
$ hd --rpn a ord A ord sub
32

# RPN-only first argument can be negative
$ hd -r -1
0xFFFFFFFFFFFFFFFF

$ ./hd -r -1.0 fexp
1023

$ ./hd --32 -r -1.0 fexp
127

$ ./hd -r nan info
0, 2047, 2251799813685248
0, 0x7FF, 0x8000000000000
nan
```

Constants `pi e nan inf` are supported
```bash
hd nan
nan

hd nan hex as
0x7FF8000000000000
```

## TODO
* Allow underscores to make it easier to enter and display
* Make 'ord' and 'chr' an operation
* Eliminate undefined or unexpected behavior in rpn program
* Pivot point can't find negative numbers. Only works with `--rpn`
* `./hd -v 0 -1 abs` should work, but it also thinks this is a binary op error. Only works with `--rpn`
* `$ ./hd -r nan fexp hex as` gives `0x409FFC0000000000` for some reason?
