
# This script is used to generate triangles for E2E tests.

import random

TRS_NUM = 10000
TRS_SIZE = 20
DOMAIN_SIZE = 1000

def genBigFP() :
    return random.random() * DOMAIN_SIZE

def genSmallFP() :
    return random.random() * TRS_SIZE

class Point :
    def __init__(self, x, y, z) :
        self.x = x
        self.y = y
        self.z = z

    def getNear(self) :
        return Point (self.x + genSmallFP (),\
                      self.y + genSmallFP (),\
                      self.z + genSmallFP ())
    
    def print(self) :
        print (self.x, self.y, self.z)

class Triangle :
    def __init__(self) :
        self.A = Point (genBigFP (), genBigFP (), genBigFP ())
        self.B = self.A.getNear()
        self.C = self.A.getNear()

    def print(self) :
        self.A.print ()
        self.B.print ()
        self.C.print ()
        print ()

print (TRS_NUM)

for i in range(0, TRS_NUM) :
    t = Triangle ()
    t.print ()
