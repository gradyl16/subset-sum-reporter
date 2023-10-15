#include <iostream>
#include <vector>
#include <string>

// Structure representing an element with a value and a name.
struct ssum_elem
{
  unsigned int x;   // Value of the element
  std::string name; // Label for the element
};

// Structure representing data for the subset sum problem.
struct ssum_data
{
  bool feasible;                                     // Indicates if the target is feasible
  unsigned long long int num_valid_subsets;          // Number of valid subsets
  unsigned long long int smallest_valid_size;        // Size of the smallest valid subset
  unsigned long long int num_valid_subsets_min_size; // Number of valid subsets of the smallest size
  std::vector<int> lexi_first_valid_min_size_subset; // Lexicographically first subset of the smallest size
};

// Class representing an instance of the subset sum problem.
class ssum_instance
{
  unsigned int target = 0;
  std::vector<std::vector<ssum_data>> feasible;
  bool done = false; // Flag indicating if dynamic programming has been run or not on this instance

public:
  std::vector<ssum_elem> elems;

  // Function to read elements from the input stream and populate the elems vector.
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

  // Function to solve the subset sum problem for a specified target sum.
  ssum_data solve(unsigned int tgt)
  {
    unsigned int n = elems.size();
    target = tgt;
    feasible = std::vector<std::vector<ssum_data>>(n, std::vector<ssum_data>(target + 1, {false, 0, 0, 0, {}}));

    // Initialize the first column (x=0) as all elements are feasible for an empty subset.
    for (unsigned int i = 0; i < n; i++)
    {
      feasible[i][0].feasible = true;                       // The empty subset is a subset of all sets, and the empty set can create a target sum of 0.
      feasible[i][0].num_valid_subsets = 1;                 // The only feasible subset for a target sum of 0 is the empty set.
      feasible[i][0].smallest_valid_size = 0;               // The empty set has cardinality of 0.
      feasible[i][0].num_valid_subsets_min_size = 1;        // The only feasible subset for a target sum of 0 is the empty set (same as overall # of valid ssets).
      feasible[i][0].lexi_first_valid_min_size_subset = {}; // The empty set is the lexicographically first subset of cardinality 0.
    }

    // Initialize the first row (element 0) for specific cases.
    for (unsigned int x = 1; x <= target; x++)
    {
      // If the first element in the array is the target sum...
      if (elems[0].x == x)
      {
        feasible[0][x].feasible = true;                        // The first element can create a target sum of itself.
        feasible[0][x].num_valid_subsets = 1;                  // The only feasible subset for a target sum of the first element is the first element.
        feasible[0][x].smallest_valid_size = 1;                // The set containing the first element has cardinality of 1.
        feasible[0][x].num_valid_subsets_min_size = 1;         // The only feasible subset for a target sum of the first element is the first element (same as overall # of valid ssets).
        feasible[0][x].lexi_first_valid_min_size_subset = {0}; // The set containing the index of the first element is the lexicographically first subset of cardinality 1.
      }
    }

    for (unsigned int i = 1; i < n; i++)
    {
      for (unsigned int x = 1; x <= tgt; x++)
      {
        // Exclude case
        if (feasible[i - 1][x].feasible)
        {
          feasible[i][x].feasible = true;                                                                        // feasible to exclude newest element --> feasible to exclude newest element or include newest element
          feasible[i][x].num_valid_subsets = feasible[i - 1][x].num_valid_subsets;                               // # of valid subsets for target x is the same as the # of valid subsets for target x without newest element
          feasible[i][x].smallest_valid_size = feasible[i - 1][x].smallest_valid_size;                           // smallest valid size for target x is the same as the smallest valid size for target x without newest element
          feasible[i][x].num_valid_subsets_min_size = feasible[i - 1][x].num_valid_subsets_min_size;             // # of valid subsets of the smallest size for target x is the same as the # of valid subsets of the smallest size for target x without newest element
          feasible[i][x].lexi_first_valid_min_size_subset = feasible[i - 1][x].lexi_first_valid_min_size_subset; // lexicographically first subset of the smallest size for target x is the same as the lexicographically first subset of the smallest size for target x without newest element
        }
        // Include case (short circuit if the current target is less than the current element's value)
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
};

// The main function reads the target value and input data and reports the results.
int main(int argc, char *argv[])
{
  unsigned int target;
  ssum_instance ssi;

  if (argc != 2)
  {
    fprintf(stderr, "One command-line argument expected: target sum\n");
    return 0;
  }

  if (sscanf(argv[1], "%u", &target) != 1)
  {
    fprintf(stderr, "Bad argument '%s'\n", argv[1]);
    fprintf(stderr, "Expected an unsigned integer\n");
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

  for (int i = 0; i < result.lexi_first_valid_min_size_subset.size(); i++)
  {
    int id = result.lexi_first_valid_min_size_subset[i];
    int val = ssi.elems[result.lexi_first_valid_min_size_subset[i]].x;
    std::string name = ssi.elems[result.lexi_first_valid_min_size_subset[i]].name;

    std::cout << "  " << name << "   (  id: " << id << "; val: " << val << ")\n"
              << std::endl;
  }

  std::cout << " }" << std::endl;
}