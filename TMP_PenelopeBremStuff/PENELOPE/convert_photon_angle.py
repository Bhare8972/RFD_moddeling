#!/usr/bin/env python3

if __name__=="__main__":
    fname='pdbrang.p08'
    fout_name='./out_pdbrang.p08'
    
    A_dict={}
    B_dict={}
    
    E_list=[]
    K_list=[]
    Z_list=[]
    
    with open(fname, 'r') as fin:
        for line in fin:
            i,j,k,Z,E,K,A,B=line.split()
            
            if E not in E_list:
                E_list.append(E)
            if Z not in Z_list:
                Z_list.append(Z)
            if K not in K_list:
                K_list.append(K)
            
            if E not in A_dict:
                A_dict[E]={}
                A_dict[E][K]={}
                A_dict[E][K][Z]=A
            elif K not in A_dict[E]:
                A_dict[E][K]={}
                A_dict[E][K][Z]=A
            elif Z not in A_dict[E][K]:
                A_dict[E][K][Z]=A
            
            if E not in B_dict:
                B_dict[E]={}
                B_dict[E][K]={}
                B_dict[E][K][Z]=B
            elif K not in B_dict[E]:
                B_dict[E][K]={}
                B_dict[E][K][Z]=B
            elif Z not in B_dict[E][K]:
                B_dict[E][K][Z]=B
    E_list.sort(key=float)
    K_list.sort(key=float)
    Z_list.sort(key=float)
                
                
    with open(fout_name, 'w') as fout:
        fout.write('{')
        for E_i in range(len(E_list)):
            E=E_list[E_i]
            fout.write(E)
            if E_i != len(E_list)-1:
                fout.write(',')
        fout.write('}\n\n')
        
        fout.write('{')
        for K_i in range(len(K_list)):
            K=K_list[K_i]
            fout.write(K)
            if K_i != len(K_list)-1:
                fout.write(',')
        fout.write('}\n\n')
        
        fout.write('{')
        for Z_i in range(len(Z_list)):
            Z=Z_list[Z_i]
            fout.write(Z)
            if Z_i != len(Z_list)-1:
                fout.write(',')
        fout.write('}\n\n')
        
        fout.write('{\n')
        for E_i in range(len(E_list)):
            E=E_list[E_i]
            fout.write('{')
            
            for K_i in range(len(K_list)):
                K=K_list[K_i]
                fout.write('{')
                
                for Z_i in range(len(Z_list)):
                    Z=Z_list[Z_i]
                    A=A_dict[E][K][Z]
                    fout.write(A)
                    if Z_i!=len(Z_list)-1:
                        fout.write(',')
                        
                fout.write('}')
                if K_i!=len(K_list)-1:
                    fout.write(',')
            
            fout.write('}')
            if E_i!=len(E_list)-1:
                fout.write(',')
            fout.write('\n')
        fout.write('}\n\n')
        
        fout.write('{\n')
        for E_i in range(len(E_list)):
            E=E_list[E_i]
            fout.write('{')
            
            for K_i in range(len(K_list)):
                K=K_list[K_i]
                fout.write('{')
                
                for Z_i in range(len(Z_list)):
                    Z=Z_list[Z_i]
                    B=B_dict[E][K][Z]
                    fout.write(B)
                    if Z_i!=len(Z_list)-1:
                        fout.write(',')
                        
                fout.write('}')
                if K_i!=len(K_list)-1:
                    fout.write(',')
            
            fout.write('}')
            if E_i!=len(E_list)-1:
                fout.write(',')
            fout.write('\n')
        fout.write('}\n\n')
                
                    
        
            
                
                
