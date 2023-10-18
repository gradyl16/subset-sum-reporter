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
  int read_elems(std::istream &stream)
  {
    int count = 0;
    ssum_elem e;

    elems.clear();
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
          ssum_table[i][x].no_v_ssets += ssum_table[i - 1][x - elems[i].x].no_v_ssets;

          // min card is min btween exclude and include cases -- if one is infeasible, other is min (INF const)
          ssum_table[i][x].min_card = std::min(ssum_table[i - 1][x].min_card,
                                               ssum_table[i - 1][x - elems[i].x].min_card + 1);

          // Smallest valid size has either not changed, decreased, or increased bc of new el
          if (ssum_table[i - 1][x].min_card == ssum_table[i - 1][x - elems[i].x].min_card + 1) // The smallest cardinality for a distinct subset has not changed
          {
            // # of valid ssets of min card is the sum of the # of valid ssets in the exclude and include cases
            // since they are both of min card
            ssum_table[i][x].no_v_ssets_min_card = ssum_table[i - 1][x].no_v_ssets_min_card
                                                 + ssum_table[i - 1][x - elems[i].x].no_v_ssets_min_card;

            // new el may be included since min card would not change as a result of its inclusion
            ssum_table[i][x].include = true;
          }
          else if (ssum_table[i - 1][x].min_card > ssum_table[i - 1][x - elems[i].x].min_card + 1) // The smallest cardinality for a distinct subset has incremented
          {
            // # of valid ssets of min card is the # of valid ssets in the include case since it is the
            // only one of min card
            ssum_table[i][x].no_v_ssets_min_card = ssum_table[i - 1][x - elems[i].x].no_v_ssets_min_card;

            // new el may be included since min card would decrease as a result of its inclusion
            ssum_table[i][x].include = true;
          }
          else if (ssum_table[i - 1][x].min_card < ssum_table[i - 1][x - elems[i].x].min_card + 1) // The smallest cardinality for a distinct subset has decreased
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

  std::vector<int> extract(ssum_data result)
  {
    std::vector<int> lexi_first;
    int i = elems.size() - 1;
    int x = target;

    // std::cout << "size of a: " << elems.size() << std::endl;
    // std::cout << "elements of a: " << std::endl
    //           << "|";
    // for (int i = 0; i < elems.size(); i++)
    // {
    //   std::cout << "  " << elems[i].x;
    // }
    // std::cout << std::endl
    //           << std::endl;

    while (x > 0)
    {
      // std::cout << "-----START LOOP-----" << std::endl;
      // std::cout << "x is not zero" << std::endl;
      // std::cout << "i b4: " << i << std::endl;
      // std::cout << "x b4: " << x << std::endl
      //           << std::endl;

      if (ssum_table[i][x].include)
      {
        lexi_first.push_back(i);

        // std::cout << "adding " << i << " to lexi_first" << std::endl;
        // std::cout << "lexi_first: ";
        // for (int i = 0; i < lexi_first.size(); i++)
        // {
        //   std::cout << lexi_first[i] << " ";
        // }

        x -= elems[i].x;

        // std::cout << std::endl;
        // std::cout << "subtracting " << elems[i].x << " (a[" << i << "]) from " << x << std::endl;
      }
      i--;

      // std::cout << "decrementing i" << std::endl
      //           << std::endl;

      // std::cout << "i: " << i << std::endl;
      // std::cout << "x: " << x << std::endl;
      // std::cout << "-----END LOOP-----" << std::endl
      //           << std::endl;
    }

    // Manually reverse elements according to mapping offset
    for (int &ind : lexi_first)
    {
      ind = elems.size() - 1 - ind;
      // std::cout << "ind: " << ind << std::endl;
    }
    // std::cout << std::endl;

    // std::cout << "lexi_first: ";
    // for (int i = 0; i < lexi_first.size(); i++)
    // {
    //   std::cout << lexi_first[i] << " ";
    // }
    // std::cout << std::endl;

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
  std::cout << "  NUM-FEASIBLE         : " << result.no_v_ssets << std::endl;
  std::cout << "  MIN-CARD-FEASIBLE    : " << result.min_card << std::endl;
  std::cout << "  NUM-MIN-CARD-FEASIBLE: " << result.no_v_ssets_min_card << std::endl;
  std::cout << "Lex-First Min-Card Subset Totaling " << target << ": " << std::endl;
  std::cout << " {" << std::endl;

  std::vector<int> lexi_first_min_card = ssi.extract(result);
  std::reverse(ssi.elems.begin(), ssi.elems.end());

  int val_sum = 0;
  for (int i = 0; i < lexi_first_min_card.size(); i++)
  {
    int id = lexi_first_min_card[i];
    int val = ssi.elems[id].x;
    val_sum += val;
    std::string name = ssi.elems[id].name;
    std::cout << "  " << name << "  ( id: " << id << "; val: " << val << ")"
              << std::endl;
  }

  std::cout << " }" << std::endl;

  // std::cout << "sum of elements of extracted subset: " << val_sum << std::endl;
}
