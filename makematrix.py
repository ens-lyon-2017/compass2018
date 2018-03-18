#! /usr/bin/env python3

m1 = [[i+4*j+1 for i in range(4)] for j in range(4)]
m2 = [[13+i+2*j for i in range(2)] for j in range(4)]

def matrixconst(M,arch):
    sizere = 0
    result = ""
    for ligne in M:
        for elt in ligne:
            result += bin(elt)[2:].rjust(arch,"0")
            sizere += arch
    return f".const {sizere} #{result}"


print(matrixconst(m1,64))
print(matrixconst(m2,64))
