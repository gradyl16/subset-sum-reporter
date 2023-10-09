

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>


struct ssum_elem {
  unsigned int x;
  std::string name;
};

class ssum_instance {
    unsigned int target=0;
    std::vector<ssum_elem> elems;
    std::vector<std::vector<bool>> feasible;  
    // feasible[i][x] = TRUE if there is a subset
    //  of a[0..i] adding up to x; FALSE otherwise
    //  (once instance has been solved)
    //
    int done=false;    // flag indicating if dp has been run or 
                       //   not on this instance

    public:

    // Function:  read_elems
    // Description:  reads elements from standard-input; 
    //   Format:  sequence of <number> <name> pairs where
    //      <number> is non-negative int and
    //      <name> is a string associated with element
    void read_elems( std::istream &stream) {
        ssum_elem e;

        elems.clear();
        // while(std::cin >> e.x && std::cin >> e.name) {
        while(stream >> e.x && stream >> e.name) {
            elems.push_back(e);
        }
        done = false;
    }

    // Function:  solve
    // Desc:  populates dynamic programming table of
    //    calling object for specified target sum.
    //    Returns true/false depending on whether the
    //    target sum is achievalble or not.
    //    Table remains intact after call.
    //    Object can be reused for alternative target sums.
    bool solve(unsigned int tgt) {
        unsigned int n = elems.size();
        unsigned int i, x;

        if(target == tgt && done) 
            return feasible[n-1][tgt];

        target = tgt;
        feasible = 
            std::vector<std::vector<bool>>(n, std::vector<bool>(target+1, false));

        // leftmost column (column zero) is all TRUE because 
        //    a target sum of zero is always acheivable (via the
        //    empty set).
        for(i=0; i<n; i++) 
            feasible[i][0] = true;

        for(x=1; x<=target; x++) {
            if(elems[0].x == x) 
                feasible[0][x] = true;
            // otherwise, feasible[0][x] remains false
        }
        for(i=1; i<n; i++) {
            for(x=1; x<=tgt; x++) {
                if(feasible[i-1][x]) 
                    feasible[i][x] = true;
                else if(x >= elems[i].x && feasible[i-1][x-elems[i].x])
                    feasible[i][x] = true;
            }
            // otherwise, feasible[0][x] remains false
        }
        done = true;
        return feasible[n-1][target];
    }
};  // end class





/**
* usage:  ssum  <target> < <input-file>
*
*
* input file format:
*
*     sequence of non-negative-int, string pairs
*
*     example:

    12 alice
    9  bob
    22 cathy
    12 doug

* such a file specifies a collection of 4 integers: 12, 9, 22, 12
* "named" alice, bob, cathy and doug.
*/
int main(int argc, char *argv[]) {
  unsigned int target;
  ssum_instance ssi;

  if(argc != 2) {
    fprintf(stderr, "one cmd-line arg expected: target sum\n");
    return 0;
  }
  if(sscanf(argv[1], "%u", &target) != 1) {
    fprintf(stderr, "bad argument '%s'\n", argv[1]);
    fprintf(stderr, "   Expected unsigned integer\n");
    return 0;
  }

  ssi.read_elems(std::cin);

  if(ssi.solve(target) ) {
    std::cout << "HOORAY!  Apparently, the target sum of " <<
      target << " is achievable\n";
    std::cout << "  How you ask?  Sorry, we just know it is possible...\n";
  }
  else {
    std::cout << "SORRY!  Apparently, the target sum of " <<
      target << " is NOT achievable\n";
  }

}
