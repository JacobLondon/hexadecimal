#!/bin/bash

global_result=0

_assert() {
    local program="$1"
    local operator="$2"
    local equals="$3"
    local output=''

    output=$($program 2>/dev/null)
    if [ "$2" == "==" ]; then
        if [ "$output" == "$equals" ]; then
            return 0
        else
            return 1
        fi
    elif [ "$2" == "!=" ]; then
        if [ "$output" != "$equals" ]; then
            return 0
        else
            return 1
        fi
    fi
}

assert() {
    local rv
    _assert "$1" "$2" "$3"
    rv=$?
    if [ $rv -eq 0 ]; then
        :
    else
        echo "FAIL: $1 $2 $3"
        global_result=1
    fi
}

assert './build/hd 0 1 add' == '1'
assert './build/hd 0xA' == '10'
assert './build/hd 10' == '0xA'
assert './build/hd 0b101' == '5'
assert './build/hd inf hex as' == '0x7FF0000000000000'
assert './build/hd --32 inf hex as' == '0x7F800000'
assert './build/hd --16 -2 hex as' == '0xFFFE'
assert './build/hd --8 --long 10 bin as' == '0b00001010'
assert './build/hd --8 10 bin as' == '0b1010'
assert './build/hd 10.2 bin as' == '0b100000000100100011001100110011001100110011001100110011001100110'
assert './build/hd 10 sep 50 sep 100' == '0xA 0x32 0x64'
assert './build/hd 10 int as' == '10'
assert './build/hd 10 oct as' == '0o12'
assert './build/hd -1' == '0xFFFFFFFFFFFFFFFF'
assert './build/hd -1 int as' == '-1'
assert './build/hd -1 float cast hex as' == '0xBFF0000000000000'
assert './build/hd 10.2 1 63 shl bitor' == '-10.200000'
assert './build/hd 4 fact' == '24'
assert './build/hd --8 0x1 1 rol hex as' == '0x2'
assert './build/hd --8 0x1 2 ror hex as' == '0x40'
assert './build/hd 10 chr as' == '\n'
assert './build/hd a ord 1 add' == '98'
assert './build/hd a ord 1 add chr as' == 'b'
assert './build/hd fexpmask fexpbit shl my_floating_mantissa save quiet  my_floating_mantissa hex as' == '0x7FF0000000000000'
assert './build/hd 10 my_ten save  my_ten mul' == '100'
assert './build/hd 10 string as' == ""

exit $global_result

