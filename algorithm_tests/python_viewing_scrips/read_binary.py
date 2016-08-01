from struct import unpack
import numpy as np

class binary_input(object):
    def __init__(self, fname):
        self.fname=fname
        self.fin=open(fname, 'rb')
        
    def in_short(self):
        return unpack('b', self.fin.read(1))[0]
        
    def in_int(self):
        return unpack('i', self.fin.read(4))[0]
        
    def in_float(self):
        return unpack('f', self.fin.read(4))[0]
        
    def in_double(self):
        return unpack('d', self.fin.read(8))[0]
        
    def at_end(self):
        L=self.fin.read(1)
        if L=='':
            return True
        else:
            self.fin.seek(-1,1)
            return False
        
class array_input(object):
    def __init__(self, binary_in):
        self.fin=binary_in
        self.type=self.fin.in_short()
        self.size=self.fin.in_int()
        self.num_left=self.size
        
    def read_ints(self):
        if self.type!=1:
            print( "cannot read integers from file with type: "+str(self.type))
        if self.num_left==0:
            print( "no data left in this array")
        out=np.zeros(self.size, dtype=int)
        for i in range(self.size):
            out[i]=self.fin.in_int()
        self.num_left=0
        return out
        
    def read_floats(self):
        if self.type!=2:
            print( "cannot read floats from file with type: "+str(self.type))
        if self.num_left==0:
            print( "no data left in this array")
        out=np.zeros(self.size, dtype=float)
        for i in range(self.size):
            out[i]=self.fin.in_float()
        self.num_left=0
        return out
        
    def read_doubles(self):
        if self.type!=3:
            print( "cannot read doubles from file with type: "+str(self.type))
        if self.num_left==0:
            print( "no data left in this array")
        out=np.zeros(self.size, dtype=np.double)
        for i in range(self.size):
            out[i]=self.fin.in_double()
        self.num_left=0
        return out

    def get_array(self):
        if self.type!=0:
            print( "cannot read arrays from file with type: "+str(self.type))
        if self.num_left==0:
            print( "no data left in this array")
        self.num_left-=1
        return array_input(self.fin)

        

