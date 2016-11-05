#!/usr/bin/env python3

def read_word(file_in):
    whitespace=[' ', '\n', '\t']
    output=[]
    state=0
    
    r=file_in.read(1)
    while r in whitespace:
        r=file_in.read(1)
    
    output.append(r)
    r=file_in.read(1)
    while not r in whitespace:
        output.append(r)
        r=file_in.read(1)
        
    return ''.join(output)

if __name__=="__main__":
    fname='pdebr18.p08'
    output_fname="./out_"+fname
    location='./SnBfiles/'
    
    N_columns=32
    N_lines=57
    lines=[]
    ##input!
    with open(location+fname, 'r') as fin:
        element=read_word(fin)
        for line_i in range(N_lines):
            E=read_word(fin)
            columns=[]
            for column_i in range(N_columns):
                columns.append(read_word(fin))
            lines.append(columns)
            energy_lost=read_word(fin)
            
    ## output!
    with open(output_fname, 'w') as fout:
        fout.write('{\n')
        for line_i in range(N_lines):
            line=lines[line_i]
            fout.write('{')
            for column_i in range(N_columns):
                fout.write(line[column_i])
                if column_i!=N_columns-1:
                    fout.write(',')
            fout.write('}')
            if line_i != N_lines-1:
                fout.write(',')
            fout.write('\n')
        fout.write('}\n')
