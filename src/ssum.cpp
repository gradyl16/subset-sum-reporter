#include <iostream>
#include <vector>
#include <string>

struct ssum_elem
{
  unsigned int x;   // value of element
  std::string name; // label for element
};

struct ssum_data
{
  bool feasible;
  unsigned long long int num_valid_subsets;
  unsigned long long int smallest_valid_size;
  unsigned long long int num_valid_subsets_min_size;
  std::vector<int> lexi_first_valid_min_size_subset;
};

class ssum_instance
{
  unsigned int target = 0;
  std::vector<std::vector<ssum_data>> feasible;
  bool done = false; // flag indicating if dp has been run or not on this instance

public:
  std::vector<ssum_elem> elems;

  int read_elems(std::istream &stream)
  {
    ssum_elem e;
    elems.clear();
    int num_elems = 0;

    while (stream >> e.x && stream >> e.name)
    {
      elems.push_back(e);
      num_elems++;
    }
    done = false;
    return num_elems;
  }

  ssum_data solve(unsigned int tgt)
  {
    unsigned int n = elems.size();
    target = tgt;
    feasible = std::vector<std::vector<ssum_data>>(n, std::vector<ssum_data>(target + 1, {false, 0, 0, 0, {}}));

    for (unsigned int i = 0; i < n; i++)
    {
      feasible[i][0].feasible = true;
      feasible[i][0].num_valid_subsets = 1;
      feasible[i][0].smallest_valid_size = 0;
      feasible[i][0].num_valid_subsets_min_size = 1;
      feasible[i][0].lexi_first_valid_min_size_subset = {};
    }

    for (unsigned int x = 1; x <= target; x++)
    {
      if (elems[0].x == x)
      {
        feasible[0][x].feasible = true;
        feasible[0][x].num_valid_subsets = 1;
        feasible[0][x].smallest_valid_size = 1;
        feasible[0][x].num_valid_subsets_min_size = 1;
        feasible[0][x].lexi_first_valid_min_size_subset = {0};
      }
    }

    for (unsigned int i = 1; i < n; i++)
    {
      for (unsigned int x = 1; x <= tgt; x++)
      {
        if (feasible[i - 1][x].feasible)
        {
          feasible[i][x].feasible = true;
          feasible[i][x].num_valid_subsets = feasible[i - 1][x].num_valid_subsets;
          feasible[i][x].smallest_valid_size = feasible[i - 1][x].smallest_valid_size;
          feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x].num_valid_subsets_min_size;
          feasible[i][x].lexi_first_valid_min_size_subset = feasible[i - 1][x].lexi_first_valid_min_size_subset;
        }
        else if (x >= elems[i].x && feasible[i - 1][x - elems[i].x].feasible)
        {
          feasible[i][x].feasible = true;
          feasible[i][x].num_valid_subsets = feasible[i - 1][x].num_valid_subsets + feasible[i - 1][x - elems[i].x].num_valid_subsets;
          feasible[i][x].smallest_valid_size = std::min(feasible[i - 1][x].smallest_valid_size, feasible[i - 1][x - elems[i].x].smallest_valid_size + 1);

          if (feasible[i][x].smallest_valid_size == feasible[i - 1][x].smallest_valid_size)
          {
            feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x].num_valid_subsets_min_size;
            feasible[i][x].lexi_first_valid_min_size_subset = feasible[i - 1][x].lexi_first_valid_min_size_subset;
          }
          else if (feasible[i][x].smallest_valid_size == feasible[i - 1][x - elems[i].x].smallest_valid_size + 1)
          {
            feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x - elems[i].x].num_valid_subsets_min_size;
            feasible[i][x].lexi_first_valid_min_size_subset = feasible[i - 1][x - elems[i].x].lexi_first_valid_min_size_subset;
            feasible[i][x].lexi_first_valid_min_size_subset.push_back(i);
          }
        }
      }
    }

    done = true;
    return feasible[n - 1][target];
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

  ssi.read_elems(std::cin);

  // if (ssi.solve(target).feasible)
  // {
  //   std::cout << "HOORAY!  Apparently, the target sum of " << target << " is achievable\n";
  //   std::cout << "  How you ask?  Sorry, we just know it is possible...\n";
  // }
  // else
  // {
  //   std::cout << "SORRY!  Apparently, the target sum of " << target << " is NOT achievable\n";
  // }

  ssum_data result = ssi.solve(target);

  std::cout << "### REPORT ###" << std::endl;
  std::cout << "  NUM ELEMS            :     " << ssi.elems.size() << std::endl;
  std::cout << "  TARGET               :     " << target << std::endl;
  std::cout << "  NUM-FEASIBLE         :     " << result.num_valid_subsets << std::endl;
  std::cout << "  MIN-CARD-FEASIBLE    :     " << result.smallest_valid_size << std::endl;
  std::cout << "  NUM-MIN-CARD-FEASIBLE:     " << result.num_valid_subsets_min_size << std::endl;
  std::cout << "Lex-First Min-Card Subset Totaling " << target << ": " << std::endl;
  std::cout << " {" << std::endl;

  std::string name;
  int id;
  int val;
  for (int i = 0; i < result.lexi_first_valid_min_size_subset.size(); i++)
  {
    name = ssi.elems[i].name;
    id = result.lexi_first_valid_min_size_subset[i];
    val = ssi.elems[result.lexi_first_valid_min_size_subset[i]].x;
    std::cout << "  " << name << "   (  id: " << id << "; val: " << val << ")\n"
              << std::endl;
  }

  std::cout << " }" << std::endl;
}
