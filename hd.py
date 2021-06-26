#!/bin/env python3
import argparse
import math
import operator
import sys
import numpy
import lark

def sep(top):
    print(top, ' ', sep='', end='')

def end(top):
    print(top, sep='')

# parser
text = None
with open("grammar.lark", "r") as fp:
    text = fp.read()

parser = lark.Lark(text, start='program')

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

if __name__ == '__main__':
    flag32 = False
    converter = float

    pivot = 1
    for i, arg in enumerate(sys.argv):
        if i == 0: continue

        if arg in ('-h', '--help'):
            print('help')
            exit(0)
        elif arg == '--32':
            flag32 = True
            converter = numpy.float32
            if i >= pivot: pivot = i + 1
        else:
            break

    rpn = Rpn(converter)
    if len(sys.argv) == pivot:
        while True:
            text = input('> ')
            hd = Hexadecimal(text, rpn.record)
            hd.parse()
            rpn.execute()
    else:
        text = ' '.join(sys.argv[pivot:])
        hd = Hexadecimal(text, rpn.record)
        hd.parse()
        rpn.execute()

    exit(0)
