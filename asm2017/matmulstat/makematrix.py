#! /usr/bin/env python3

m1 = [[i+4*j+1 for i in range(4)] for j in range(4)]
m2 = [[13+i+2*j for i in range(2)] for j in range(4)]

def matrixconst(M,arch):
    sizere = 0
    result = bytes()
    for ligne in M:
        for elt in ligne:
            result += elt.to_bytes(arch//8,"big")
    return result


f = open("mat4x4.bytes","wb")
f.write(matrixconst(m1,64))
f.close()

