#!/bin/env python3
import lark
import sys

text = None
with open("grammar.lark", "r") as fp:
    text = fp.read()

parser = lark.Lark(text, start='program')

class Hexadecimal:
    def __init__(self, stream, func):
        '''
        stream is an open file stream
        func is a 'def func(str)' where str is the token value
        '''
        self.stream = stream
        self.func = func

    def parse(self):
        for text in self.stream:
            tree = parser.parse(text)
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

hd = Hexadecimal(sys.stdin, print)
hd.parse()
