#!/bin/env python3

text = None
with open("grammar.lark", "r") as fp:
    text = fp.read()

from lark import Lark, Transformer
parser = Lark(text, start='program')
tree = parser.parse('5 + abs (!1 * 2); gcd 5 6, 10')
print(tree.pretty())

class HdTransformer(Transformer):
    pass

HdTransformer().transform(tree)
