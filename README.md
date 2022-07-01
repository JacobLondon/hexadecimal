# HD - HexaDecimal
Run `hd --help` to see options. Note most operators have a symbol version which is also usable to avoid needing to escape `;` for example, in the terminal.

## Building
```bash
# installation
make release -j4
sudo make install

# uninstallation
sudo make uninstall
```

## Usage
```bash

$ hd 10
0xA

$ hd 0xA
10

$ hd 0b101
5

# math with varying formats and conversions!
$ hd 0b101 5 mul hex as
0x19
$ hd 0x19
25

# support of 8/16/32/64 bit operations, as well as some constants
# note 8/16 bits do not have floating point representations
$ hd inf hex as
0x7FF0000000000000
$ hd --32 inf hex as
0x7F800000
$ hd --16 -2 hex as
0xFFFE
$ hd --8 --long 10 bin as
0b00001010
$ hd --8 10 bin as
0b1010

# float to binary conversion
$ hd -10.2 bin as
0b1100000000100100011001100110011001100110011001100110011001100110

# seperate your operations
$ hd 10 sep 50 sep 100
0xA 0x32 0x64
$ hd 10 int as end 10 hex as
10
0xA

# signed-ness
$ hd -1
0xFFFFFFFFFFFFFFFF
$ hd -1 int as
-1

# integer to float casting then print as hex
$ hd -1 float cast hex as
0xBFF0000000000000

# bit operations on floats
# manually set highest bit (sign bit) to 1 on a float 64
$ hd 10.2 1 63 shl bitor
-10.2

# inspect floating point info
$ hd 10.0 info
0, 1026, 1125899906842624
0, 0x402, 0x4000000000000
10

# Constants 'pi e nan inf' are supported
$ hd pi sep %e sep nan sep inf sep -inf
3.14159 2.71828 nan inf -inf

# get the ASCII table
$ hd --table

# manual chr lookup
$ hd --chr 10
\n
# chr operation
$ hd 10 chr as
\n

# manual ord lookup
$ hd --ord a
97
# ord operation and math
$ hd a ord 1 add
98
$ hd a ord 1 add chr as
b

$ hd --endianness
little

# variables seperated by 'quiet' to not print anything before (similar to end/sep)
$ hd fexpmask fexpbit shl my_floating_mantissa save quiet  my_floating_mantissa hex as
0x7FF0000000000000

# saving returns the saved value
$ hd 10 my_ten save 1 add
11
```

## TODO
* Allow underscores to make it easier to enter and display
* Support different formats for numbers
* hyperbolic trig functions
