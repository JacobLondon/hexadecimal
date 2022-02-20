# HD - HexaDecimal
Run `sudo make install` or `sudo make uninstall`. Run `hd --help` to see options. Note most operators have a symbol version which is also usable, but not shown below to avoid needing to escape `;` for example, in the terminal. Calculations are currently in RPN (Reverse Polish Notation). Numbers are represented in 64 bits unless explicitly told to use 32 bits.

## Building
```bash
# installation
make
sudo make install

# uninstallation
sudo make uninstall
```

## New Usage
```bash
$ hexadecimal 10
0xA

$ hexadecimal 0xA
10

$ hexadecimal 0b101
5

# math with varying formats and conversions!
$ hexadecimal '(0b101 * 5) as hex'
0x19
$ hexadecimal 0x19
25

# support of 32 and 64 bit operations, as well as some constants
$ hexadecimal 'inf as hex'
0x7FF0000000000000
$ hexadecimal --32 'inf as hex'
0x7F800000

# float to binary conversion
$ hexadecimal '-10.2 as bin'
0b1100000000100100011001100110011001100110011001100110011001100110

# seperate your operations
$ hexadecimal '10, 50, 100'
0xA 0x32 0x64

# signed-ness
$ hexadecimal '-1'
0xFFFFFFFFFFFFFFFF
$ hexadecimal '-1 as int'
-1

# integer to float casting then print as hex
$ hexadecimal '(-1 cast float) as hex'
0xBFF0000000000000

# type punning and multiple operations
$ hexadecimal '10.0 as int; 10.0 as hex'
4621819117588971520
0x4024000000000000

# inspect floating point info
$ hexadecimal 'info 10.0'
0, 1026, 1125899906842624
0, 0x402, 0x4000000000000
10

# Constants 'pi e nan inf' are supported
$ hexadecimal 'pi, e, nan, inf'
3.14159 2.71828 nan inf

```

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

$ hd a ord
97

$ hd --chr 97
a

# RPN-only features calling ord on ASCII:
$ hd a ord A ord sub
32

# RPN-only first argument can be negative
$ hd -1
0xFFFFFFFFFFFFFFFF

$ ./hd -1.0 fexp
1023

$ ./hd --32 -1.0 fexp
127

$ ./hd nan info
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
