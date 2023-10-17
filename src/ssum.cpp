#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

const unsigned long long int INF = 99999999999999;

struct ssum_elem
{
  unsigned int x;   // value of element
  std::string name; // label for element
};

struct ssum_data
{
  bool feasible;
  bool include;
  unsigned long long int num_valid_subsets;
  unsigned long long int smallest_valid_size;
  unsigned long long int num_valid_subsets_min_size;
};

class ssum_instance
{
  unsigned int target = 0;
  std::vector<std::vector<ssum_data>> feasible;
  std::vector<std::vector<int>> subsets_count;

  // first el in this set will be lexicographically first; but logn insertion time... use vector?
  // std::set<std::set<int>> valid_min_size_subsets;

  // feasible[i][x] = TRUE if there is a subset
  //  of a[0..i] adding up to x; FALSE otherwise
  //  (once instance has been solved)
  //
  int done = false; // flag indicating if dp has been run or
                    //   not on this instance

public:
  std::vector<ssum_elem> elems;

  // Function:  read_elems
  // Description:  reads elements from standard-input, returns num els;
  //   Format:  sequence of <number> <name> pairs where
  //      <number> is non-negative int and
  //      <name> is a string associated with element
  int read_elems(std::istream &stream)
  {
    int count = 0;
    ssum_elem e;

    elems.clear();
    // while(std::cin >> e.x && std::cin >> e.name) {
    while (stream >> e.x && stream >> e.name)
    {
      elems.push_back(e);
      count++;
    }
    done = false;

    return count;
  }

  // Function:  solve
  // Desc:  populates dynamic programming table of
  //    calling object for specified target sum.
  //    Returns true/false depending on whether the
  //    target sum is achievable or not.
  //    Table remains intact after call.
  //    Object can be reused for alternative target sums.
  ssum_data solve(unsigned int tgt)
  {
    unsigned int n = elems.size();
    unsigned int i, x;

    // Reverse the elements -- this will help us to extract the lexicographically first subset
    std::reverse(elems.begin(), elems.end());

    // If target sum is previously requested one and table has already been populated, ret result
    if (target == tgt && done)
      return feasible[n - 1][tgt];

    /*
      Note: rethink how subsolutions are defined and computed -- left to right may not be sufficient!
    */

    // Otherwise, reset class members to prepare for new table population
    target = tgt;
    feasible = std::vector<std::vector<ssum_data>>(n, std::vector<ssum_data>(target + 1, {false, false, 0, INF, 0}));

    // leftmost column (column zero) is all TRUE because
    //    a target sum of zero is always acheivable (via the
    //    empty set).
    for (i = 0; i < n; i++)
    { // N runtime
      feasible[i][0].feasible = true;
      feasible[i][0].include = false;
      feasible[i][0].num_valid_subsets = 1;
      feasible[i][0].smallest_valid_size = 0;
      feasible[i][0].num_valid_subsets_min_size = 1;
    }

    // topmost row (row zero) is TRUE only if the first element of the array is equal to the target sum
    for (x = 1; x <= target; x++) // T runtime
    {
      if (elems[0].x == x)
      {
        feasible[0][x].feasible = true;
        feasible[0][x].include = true;
        feasible[0][x].num_valid_subsets = 1;
        feasible[0][x].smallest_valid_size = 1;
        feasible[0][x].num_valid_subsets_min_size = 1;
      }
      // otherwise, feasible[0][x] remains false
    }
    for (i = 1; i < n; i++) // NT runtime
    {
      for (x = 1; x <= tgt; x++)
      {
        // Exclude case
        if (feasible[i - 1][x].feasible)
        {
          feasible[i][x].feasible = true;
          // include remains false
          feasible[i][x].num_valid_subsets = feasible[i - 1][x].num_valid_subsets;
          feasible[i][x].smallest_valid_size = feasible[i - 1][x].smallest_valid_size;
          feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x].num_valid_subsets_min_size;
        }
        // Include case
        if (x >= elems[i].x && feasible[i - 1][x - elems[i].x].feasible)
        {
          feasible[i][x].feasible = true;

          if (feasible[i - 1][x].smallest_valid_size + 1 == feasible[i - 1][x - elems[i].x].smallest_valid_size + 1)
          {
            feasible[i][x].include = feasible[i - 1][x].num_valid_subsets_min_size < feasible[i - 1][x - elems[i].x].num_valid_subsets_min_size;
          }
          else
          {
            feasible[i][x].include = feasible[i - 1][x].smallest_valid_size + 1 < feasible[i - 1][x - elems[i].x].smallest_valid_size + 1;
          }

          // The number of valid distinct subsets is the sum of the number of valid distinct subsets in the exclude and include cases
          feasible[i][x].num_valid_subsets = feasible[i - 1][x].num_valid_subsets + feasible[i - 1][x - elems[i].x].num_valid_subsets;

          // The smallest valid subset size is the minimum of the smallest valid subset size in the exclude and include cases
          feasible[i][x].smallest_valid_size = std::min(feasible[i - 1][x].smallest_valid_size,
                                                        feasible[i - 1][x - elems[i].x].smallest_valid_size + 1);

          // It must be the case that the min card is either the min card in the exclude case or 1 greater than the min card in the include case
          if (feasible[i][x].smallest_valid_size == feasible[i - 1][x].smallest_valid_size + 1) // The smallest cardinality for a distinct subset has not changed:
          {
            // Add the number of valid subsets of the smallest valid size in the exclude case to the number of valid subsets of the smallest valid size in the include case
            feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x].num_valid_subsets_min_size + feasible[i - 1][x - elems[i].x].num_valid_subsets_min_size;
          }
          else if (feasible[i][x].smallest_valid_size < feasible[i - 1][x - elems[i].x].smallest_valid_size + 1) // The smallest cardinality for a distinct subset has incremented
          {
            feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x].num_valid_subsets_min_size;
          }
          else
          {
            feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x - elems[i].x].num_valid_subsets_min_size;
          }
        }
        // otherwise, feasible[i][x].feasible remains false
      }
    }

    done = true;
    return feasible[n - 1][target];
  }

  std::vector<int> extract(ssum_data result) {
    std::vector<int> lexi_first;
    int i = elems.size() - 1;
    int x = target;
    while (i >= 0 && x >= 0) {
      if (feasible[i][x].include) {
        lexi_first.push_back(i);
        x -= elems[i].x;
      }
      i--;
    }
    std::reverse(lexi_first.begin(), lexi_first.end());
    return lexi_first;
  }
}; // end class

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
int main(int argc, char *argv[])
{
  unsigned int target;
  ssum_instance ssi;

  if (argc != 2)
  {
    fprintf(stderr, "one cmd-line arg expected: target sum\n");
    return 0;
  }
  if (sscanf(argv[1], "%u", &target) != 1)
  {
    fprintf(stderr, "bad argument '%s'\n", argv[1]);
    fprintf(stderr, "   Expected unsigned integer\n");
    return 0;
  }

  int num_elems = ssi.read_elems(std::cin);

  ssum_data result = ssi.solve(target);

  // Print the results and report.
  std::cout << "### REPORT ###" << std::endl;
  std::cout << "  NUM ELEMS            : " << num_elems << std::endl;
  std::cout << "  TARGET               : " << target << std::endl;
  std::cout << "  NUM-FEASIBLE         : " << result.num_valid_subsets << std::endl;
  std::cout << "  MIN-CARD-FEASIBLE    : " << result.smallest_valid_size << std::endl;
  std::cout << "  NUM-MIN-CARD-FEASIBLE: " << result.num_valid_subsets_min_size << std::endl;
  std::cout << "Lex-First Min-Card Subset Totaling " << target << ": " << std::endl;
  std::cout << " {" << std::endl;

  std::vector<int> lexi_first_min_card = ssi.extract(result);
  std::reverse(ssi.elems.begin(), ssi.elems.end());

  for (int i = 0; i < lexi_first_min_card.size(); i++)
  {
    int id = lexi_first_min_card[i];
    int val = ssi.elems[id].x;
    std::string name = ssi.elems[id].name;
    std::cout << "  " << name << "   (  id: " << id << "; val: " << val << ")"
              << std::endl;
  }

  std::cout << " }" << std::endl;
}
