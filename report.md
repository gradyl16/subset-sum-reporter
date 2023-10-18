# Subset Sum Algorithms Report

## Source Code

![](ssum_codesnap.png)


---

## Explanations

I would like to preface all of these explanations by stating that my overall approach to the entire problem was to simply extend the table to accommodate more data. As such, for the raw statistics (the first 2/3 below), there are both base cases and recursive cases to consider in their computation.

Additionally, note that each of the table's entries are initialized with values that can only be overwritten; in other words, they will never go "in the other direction" for whatever metric is currently being examined.

### Distinct Subset Quantity Computation

Base cases:

- The first **column** of the dynamic programming table (as we established with the simple T/F feasibility) corresponds to a target sum of 0 for some prefix of input elements up to i for i <= n (with 1 based indexing). In all of these cases, the number of distinct subsets which can compose the target sum is 1. This is because the only set (the empty set) which can compose a target sum of 0 (from an input of positive integers) is independent of the values of the elements themselves. See `line 98` from section 1 and the comments above.

- The first **row** of the dynamic programming table corresponds to a varying target and an array consisting of the first element provided by the user. The target sum can only be composed by a singleton element if it is itself equal to the target. If this is the case, then there is exactly one valid subset which composes that target sum: the set containing the index of that element. See `line 125` from section 1 and the comments above.

Recursive cases:

- If it is feasible to compose a given target sum without the newest element, then it must be the case that the number of distinct subsets totaling the target is at least as large as it was without that element. In other words, if the newest element doesn't contribute in any way to the feasibility of the target sum (the "exclude" case), the number of distinct subsets that total the target sum in consideration the new element will remain equal to the number before consideration.

- It follows straightforwardly from this notion that if the newest element *does* contribute in some way to the feasibility of the target sum (the "include" case)

### Minimum Sized Subset Computation

### Lexicographically First Minimum Size Subset Extraction

---

## Data

### Run 1

![](run1.png)

### Run 2

![](run2.png)

### Run 3

![](run3.png)
