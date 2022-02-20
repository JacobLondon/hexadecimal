#!/usr/bin/env python3
#import argparse
import math
import operator
import sys
import numpy
import lark

grammar = r"""\

BINARY: /0[bB][01]+/
OCTAL: /0[oO][01234567]+/
HEXADECIMAL: /0[xX][0123456789abcdefABCDEF]+/
FLOAT: /[+-]?((0?\.[0123456789]+)|((0\.)|([123456789][0123456789]*\.[0123456789]*)))/
UNSIGNED: /0|([123456789][0123456789]*)/
SIGNED: /[+-](0|([123456789][0123456789]*))/
CONSTANT: /pi|e|inf|nan/
TYPE: /int|uint|float|string/
FORMAT: /dec|hex|oct|bin|big|little/

number: BINARY
    | OCTAL
    | HEXADECIMAL
    | CONSTANT
    | FLOAT
    | UNSIGNED
    | SIGNED
    | CONSTANT
    | TYPE
    | FORMAT

// decreasing precedence
!binary_func: "gcd" | "lcm"
!binary_conv: "cast" | "as"
!binary_pow: "**"  -> pow
           | "pow" -> pow
!binary_mul: "*"   -> mul
           | "mul" -> mul
           | "/"   -> div
           | "div" -> div
           | "%"   -> mod
           | "mod" -> mod
           | "<<"  -> shl
           | "shl" -> shl
           | ">>"  -> shr
           | "shr" -> shr
           | "&~"  -> bitandinv
           | "bitandinv" -> bitandinv
           | "&"   -> bitand
           | "bitand" -> bitand
!binary_add: "+"   -> add
           | "add" -> add
           | "-"   -> sub
           | "sub" -> sub
           | "^"   -> xor
           | "xor" -> xor
           | "|"   -> bitor
           | "bitor" -> bitor
!binary_equ: "=="  -> eq
           | "eq"  -> eq
           | "!="  -> neq
           | "neq" -> neq
           | ">"   -> gt
           | "gt"  -> gt
           | "<"   -> lt
           | "lt"  -> lt
           | ">="  -> gte
           | "gte" -> gte
           | "<="  -> lte
           | "lte" -> lte
!binary_and: "&&"  -> and
           | "and" -> and
!binary_or:  "||"  -> or
           | "or"  -> or

// decreasing precedence
!unary_func: "sqrt" | "sin" | "cos" | "tan"
         | "abs" | "sgn" | "floor" | "round"
!unary_op: "~"   -> inv
         | "inv" -> inv
         | "!"   -> not
         | "not" -> not
!seperator: ","  -> sep
         | "sep" -> sep
         | ";"   -> end
         | "end" -> end

// increasing precedence
value: value binary_or value
    | value binary_and value
    | value binary_equ value
    | value binary_add value
    | value binary_mul value
    | value binary_pow value
    | value binary_conv value
    | value binary_func value
    | unary_op value
    | unary_func value
    | number
    | "(" value ")"

program: value
    | [value (seperator value)*]

%import common.ESCAPED_STRING
%import common.WS
%ignore WS

"""

def sep(top):
    print(top, ' ', sep='', end='')

def end(top):
    print(top, sep='')

# parser
#text = None
#with open("grammar.lark", "r") as fp:
#    text = fp.read()
parser = lark.Lark(grammar, start='program')

# definitions

class Hexadecimal:
    def __init__(self, text, func):
        '''
        text is what will be parsed
        func is a 'def func(str)' where str is the token value
        '''
        self.text = text
        self.func = func

    def parse(self):
        tree = parser.parse(self.text)
        for top in tree.children:
            self.postorder(top)

    def postorder(self, tree):
        # last
        if len(tree.children) == 1:
            self.conv_or_recurse(tree.children[0])
        # unop
        elif len(tree.children) == 2:
            self.conv_or_recurse(tree.children[1])
            self.conv_or_recurse(tree.children[0])
        # binop
        elif len(tree.children) == 3:
            self.conv_or_recurse(tree.children[0])
            self.conv_or_recurse(tree.children[2])
            self.conv_or_recurse(tree.children[1])

    def conv_or_recurse(self, child):
        if type(child) is lark.lexer.Token:
            self.func(child)
        else:
            self.postorder(child)

##
# TODO: ror, rol, chr, ord
binops = {
    'gcd': math.gcd,
    'lcm': lambda a, b: abs(a * b) // math.gcd(a, b),
    'cast': lambda a, b: None,
    'as': lambda a, b: None,
    '**': math.pow,
    'pow': math.pow,
    '*': operator.mul,
    'mul': operator.mul,
    '/': operator.truediv,
    'div': operator.truediv,
    '%': operator.mod,
    'mod': operator.mod,
    '<<': operator.lshift,
    'shl': operator.lshift,
    '>>': operator.rshift,
    'shr': operator.rshift,
    '&~': lambda a, b: a & ~b,
    'bitandinv': lambda a, b: a & ~b,
    '&': lambda a, b: a & b,
    'bitand': lambda a, b: a & b,
    '+': operator.add,
    'add': operator.add,
    '-': operator.sub,
    'sub': operator.sub,
    '^': operator.xor,
    'xor': operator.xor,
    '|': lambda a, b: a | b,
    'bitor': lambda a, b: a | b,
    '==': operator.eq,
    'eq': operator.eq,
    '!=': lambda a, b: a != b,
    'neq': lambda a, b: a != b,
    '>': operator.gt,
    'gt': operator.gt,
    '<': operator.lt,
    'lt': operator.lt,
    '>=': lambda a, b: a >= b,
    'gte': lambda a, b: a >= b,
    '<=': lambda a, b: a <= b,
    'lte': lambda a, b: a <= b,
    '&&': lambda a, b: 1 if a and b else 0,
    'and': lambda a, b: 1 if a and b else 0,
    '||': lambda a, b: 1 if a or b else 0,
    'or': lambda a, b: 1 if a or b else 0,
}

unops = {
    'sqrt': math.sqrt,
    'sin': math.sin,
    'cos': math.cos,
    'tan': math.tan,
    'abs': abs,
    '~': operator.inv,
    'inv': operator.inv,
    '!': operator.not_,
    'not': operator.not_,
}

seperators = {
    ',': sep,
    'sep': sep,
    ';': end,
    'end': end,
}

class Rpn:
    def __init__(self, converter=float):
        self.program = []
        self.converter = converter

    def record(self, literal):
        '''Record a single command'''
        self.program.append(literal)

    def execute(self):
        stack = []
        for f in self.program:
            if f in binops:
                rhs = stack.pop()
                lhs = stack.pop()
                stack.append(binops[f](lhs, rhs))
            elif f in unops:
                operand = stack.pop()
                stack.append(unops[f](operand))
            elif f in seperators:
                seperators[f](stack[-1])
            else:
                stack.append(self.converter(f))
        end(stack[-1])

class Converter:
    def __init__(self, flag32=False):
        self.type_float = float if not flag32 else numpy.float32
        self.type_int = int if not flag32 else numpy.int32

    def conv(self, string):
        return self.type_float(string)

# TODO: Do operations and type conversion/correction for int only ops...

def main():
    # quote output
    qflag = False

    if len(sys.argv) <= 1:
        return 1

    pivot = 1
    for i, arg in enumerate(sys.argv[pivot:]):
        ndx = i + 1
        if arg in ("-q", "--quote"):
            qflag = True
            if ndx >= pivot:
                pivot = ndx + 1

    text = " ".join(sys.argv[pivot:])
    stack = []

    hd = Hexadecimal(text, stack.append)
    hd.parse()

    if qflag: print(end="\"")
    # lark.lexer.Token.value
    for token in stack:
        print(token.value, end=' ')
    if qflag:
        print(end="\"\n")
    else:
        print()
    return 0

if __name__ == '__main__':
    exit(main())
