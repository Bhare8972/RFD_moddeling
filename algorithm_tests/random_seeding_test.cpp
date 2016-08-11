
#include <cmath>
#include <list>

#include "vector.hpp"
#include "vector_long.hpp"

#include "GSL_utils.hpp"
#include "rand.hpp"

using namespace std;

//prove that the randome number generators that are initiated very closely in time will have different sequences

int main()
{
    //set_next_seed(0);
    
    rand_gen randA;
    rand_gen randB;
    rand_gen randC;
    
    print(randA.uniform());
    print(randB.uniform());
    print(randC.uniform());
    print();
    
    
    print(randA.uniform());
    print(randB.uniform());
    print(randC.uniform());
    print();
    
    
    print(randA.uniform());
    print(randB.uniform());
    print(randC.uniform());
    print();
    
    
    print(randA.uniform());
    print(randB.uniform());
    print(randC.uniform());
    print();
    
    
    print(randA.uniform());
    print(randB.uniform());
    print(randC.uniform());
    print();
    
}
