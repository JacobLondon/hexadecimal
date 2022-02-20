# HD - HexaDecimal
Run `hexadecimal --help` to see options. Note most operators have a symbol version which is also usable to avoid needing to escape `;` for example, in the terminal.

Note `requirements.txt` must be installed to use the `hexadecimal` tool. The `hd` tool alone doesn't require these dependencies however:
```bash
cat requirements.txt | xargs python3 -m pip install
```

## Building
```bash
# installation
make
sudo make install

# uninstallation
sudo make uninstall
```

## Usage
```bash
#
# Hexadecimal Tool
#

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

#
# C++ Helpers:
#

$ hd --table # -- prints the ASCII table
# chr lookup
$ hd --chr 10
\n
# ord lookup
$ hd --ord 'a'
97
```

## TODO
* Allow underscores to make it easier to enter and display
* Make 'ord' and 'chr' an operation
* Eliminate undefined or unexpected behavior in rpn program
* Pivot point can't find negative numbers. Only works with `--rpn`
* `./hd -v 0 -1 abs` should work, but it also thinks this is a binary op error. Only works with `--rpn`
* `$ ./hd -r nan fexp hex as` gives `0x409FFC0000000000` for some reason?
