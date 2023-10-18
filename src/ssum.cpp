#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

// Constant for infinity; used to initialize table entries
// which may not be feasible for min cardinality
const unsigned long long int INF = 99999999999999;

struct ssum_elem
{
  unsigned int x;   // value of element
  std::string name; // label for element
};

// Data corresponding to an entry in the ssum_table
// Each entry encodes this data for a target sum of i, x
// where i is the index of the last element considered and
// x is the target sum
struct ssum_data
{
  bool feasible;                              // feasibility of entry achieving target sum
  bool include;                               // flag indicating inclusion of entry in lexi first sset
  unsigned long long int no_v_ssets;          // number of valid distinct subsets for entry
  unsigned long long int min_card;            // min card of valid distinct subsets for entry
  unsigned long long int no_v_ssets_min_card; // number of valid distinct subsets of min card for entry
};

class ssum_instance
{
private:
  unsigned int target = 0; // Overall target sum for this instance
  int done = false;        // Flag indicating whether table has already been populated

  // dynamic programming table which stores interesting ssum data
  std::vector<std::vector<ssum_data>> ssum_table;

public:
  std::vector<ssum_elem> elems;

  // Function:  read_elems
  // Description:  reads elements from stdin, returns num els;
  // Format:  sequence of <number> <name> pairs where
  //      <number> is non-negative int and
  //      <name> is a string associated with element
  void read_elems(std::istream &stream)
  {
    ssum_elem e;

    elems.clear();
    while (stream >> e.x && stream >> e.name)
    {
      elems.push_back(e);
    }
    done = false;
  }

  // Function:  solve
  // Desc:  populates dynamic programming table of
  //        calling object for specified target sum.
  //        Returns true/false depending on whether the
  //        target sum is achievable or not.
  //        Table remains intact after call.
  //        Object can be reused for alternative target sums.
  ssum_data solve(unsigned int tgt)
  {
    unsigned int n = elems.size();
    unsigned int i, x;

    // Reverse the elements to extract the lexicographically first subset
    std::reverse(elems.begin(), elems.end());

    // If target sum is previously requested one and table has already been populated, ret result
    if (target == tgt && done)
      return ssum_table[n - 1][tgt];

    // Otherwise, reassign class members to prepare for new table population
    target = tgt;
    ssum_table = std::vector<std::vector<ssum_data>>(n, std::vector<ssum_data>(target + 1, {false, false, 0, INF, 0}));

    // Column base cases
    for (i = 0; i < n; i++)
    { // N runtime

      // All TRUE because a target sum of zero is
      // always acheivable (via the empty set).
      ssum_table[i][0].feasible = true;

      // All FALSE because no element (in Z+) can be
      // included to compose a target some of 0.
      ssum_table[i][0].include = false;

      // The empty set is a set, so there is 1 valid
      // subset which compose a target sum of 0.
      ssum_table[i][0].no_v_ssets = 1;

      // The empty set has a cardinality of 0.
      ssum_table[i][0].min_card = 0;

      // The empty set is unique, so it is the only
      // valid subset of cardinality 0.
      ssum_table[i][0].no_v_ssets_min_card = 1;
    } // otherwise, ssum_table[i][0] init vals remain

    // Row base cases
    for (x = 1; x <= target; x++)
    { // T runtime

      // We only care about the case where the first
      // element is exactly the target sum.
      if (elems[0].x == x)
      {
        // The first element IS the target sum, so it is feasible.
        ssum_table[0][x].feasible = true;

        // The first element IS the target sum, so has the potential
        // to be included in the lexicographically first min card subset.
        ssum_table[0][x].include = true;

        // The set containing the first element is a set, so
        // there is 1 valid subset which composes the target sum.
        ssum_table[0][x].no_v_ssets = 1;

        // The set containing the first element has a cardinality of 1.
        ssum_table[0][x].min_card = 1;

        // The set containing the first element is unique, so it is the
        // only valid subset of cardinality 1.
        ssum_table[0][x].no_v_ssets_min_card = 1;
      }
    } // otherwise, ssum_table[0][x] init vals remain

    // Populate the rest of the table using the recursive cases
    for (i = 1; i < n; i++) // NT runtime
    {
      for (x = 1; x <= tgt; x++)
      {
        // Exclude case
        if (ssum_table[i - 1][x].feasible)
        {
          // If the target sum can be composed without the new element, values carry over
          ssum_table[i][x].feasible = true;
          ssum_table[i][x].no_v_ssets = ssum_table[i - 1][x].no_v_ssets;
          ssum_table[i][x].min_card = ssum_table[i - 1][x].min_card;
          ssum_table[i][x].no_v_ssets_min_card = ssum_table[i - 1][x].no_v_ssets_min_card;
        }
        // Include case
        if (x >= elems[i].x && ssum_table[i - 1][x - elems[i].x].feasible)
        {
          ssum_table[i][x].feasible = true;

          // # of valid ssets is the sum of the # of valid ssets in the exclude and include cases
          // Note that # valid ssets in exclude case is already stored in exclude case, so we accumulate
          ssum_table[i][x].no_v_ssets = ssum_table[i - 1][x].no_v_ssets
                                      + ssum_table[i - 1][x - elems[i].x].no_v_ssets;

          // min card is min btween exclude and include cases -- if one is infeasible, other is min (INF const)
          ssum_table[i][x].min_card = std::min(ssum_table[i - 1][x].min_card,
                                               ssum_table[i - 1][x - elems[i].x].min_card + 1);

          // Smallest valid size has either not changed, decreased, or increased bc of new el
          if (ssum_table[i - 1][x].min_card == ssum_table[i - 1][x - elems[i].x].min_card + 1)
          {
            // # of valid ssets of min card is the sum of the # of valid ssets in the exclude and include cases
            // since they are both of min card
            ssum_table[i][x].no_v_ssets_min_card = ssum_table[i - 1][x].no_v_ssets_min_card
                                                 + ssum_table[i - 1][x - elems[i].x].no_v_ssets_min_card;

            // new el may be included since min card would not change as a result of its inclusion
            ssum_table[i][x].include = true;
          }
          else if (ssum_table[i - 1][x].min_card > ssum_table[i - 1][x - elems[i].x].min_card + 1) 
          {
            // # of valid ssets of min card is the # of valid ssets in the include case since it is the
            // only one of min card
            ssum_table[i][x].no_v_ssets_min_card = ssum_table[i - 1][x - elems[i].x].no_v_ssets_min_card;

            // new el may be included since min card would decrease as a result of its inclusion
            ssum_table[i][x].include = true;
          }
          else if (ssum_table[i - 1][x].min_card < ssum_table[i - 1][x - elems[i].x].min_card + 1)
          {
            // # of valid ssets of min card is the # of valid ssets in the exclude case since it is the
            // only one of min card
            ssum_table[i][x].no_v_ssets_min_card = ssum_table[i - 1][x].no_v_ssets_min_card;

            // NOTE: new el may not be included since min card would increase as a result of its inclusion
          }
        }
      }
    }
    done = true;
    return ssum_table[n - 1][target];
  }

  // Function:  extract
  // Desc:  populates first satisfying subset of minimum
  //        cardinality which occurs lexicographically first
  //        according to the indices of the input elements.
  std::vector<int> extract()
  {
    std::vector<int> lexi_first;

    // start the extraction from the last element in the table
    int i = elems.size() - 1;
    int x = target;

    // until we reach a base case...
    while (x > 0)
    {
      if (ssum_table[i][x].include)
      {
        // add first occurring indices that have been marked
        // for min card inclusion
        lexi_first.push_back(i);

        // locate the next element to extract by following the
        // include case
        x -= elems[i].x;
      }
      i--;
    }

    // manually reverse elements according to mapping offset
    for (int &ind : lexi_first)
      ind = elems.size() - 1 - ind;

    // automatically reverse elements to restore original order
    // for class user
    std::reverse(elems.begin(), elems.end());

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

  // Ensure proper command line usage
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

  ssi.read_elems(std::cin);                             // Read in elements from stdin
  ssum_data result = ssi.solve(target);                 // Get reference to last entry in dp ssum table
  std::vector<int> lexi_first_min_card = ssi.extract(); // Extract lexi first min card subset

  // Report all discovered data
  std::cout << "### REPORT ###" << std::endl;
  std::cout << "  NUM ELEMS            : " << ssi.elems.size() << std::endl;
  std::cout << "  TARGET               : " << target << std::endl;
  std::cout << "  NUM-FEASIBLE         : " << result.no_v_ssets << std::endl;
  std::cout << "  MIN-CARD-FEASIBLE    : " << result.min_card << std::endl;
  std::cout << "  NUM-MIN-CARD-FEASIBLE: " << result.no_v_ssets_min_card << std::endl;

  std::cout << "Lex-First Min-Card Subset Totaling " << target << ": " << std::endl;
  std::cout << " {" << std::endl;
  for (int i = 0; i < lexi_first_min_card.size(); i++)
  {
    // Aliases for legibility
    int id = lexi_first_min_card[i];
    int val = ssi.elems[id].x;

    std::string name = ssi.elems[id].name;
    std::cout << "  " << name << "  ( id: " << id << "; val: " << val << ")"
              << std::endl;
  }
  std::cout << " }" << std::endl;
}