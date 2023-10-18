# Subset Sum Algorithms Report

**Author:** Dylen Greenenwald

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

- If it is feasible to compose a given target sum without the newest element, then it must be the case that the number of distinct subsets totaling the target is at least as large as it was without that element. In other words, if the newest element doesn't contribute in any way to the feasibility of the target sum (the "exclude" case), the number of distinct subsets that total the target sum in consideration the new element will remain equal to the number before consideration. See `line 146`; the number of distinct subsets (the value, in this case, as indicated by the above comment) will carry over from the prefix that excludes that element.

- It follows straightforwardly from this notion that if the newest element *does* contribute in some way to the feasibility of the target sum (the "include" case), that there will be more distinct subsets that result from its inclusion. In fact, there will be exactly as many more distinct subsets as can be computed by looking up previously populated values in the table that take into account the value at the newest index. Refer to `line 157` to see this computation. Notice that this computation only occurs if it's feasible to include the newest element in the input sequence, as defined by `line 151`. To explain this intuitively, since the newest element has a new index, any valid subset that includes its index will be distinct by our definition -- regardless of whether or not the values in these new sets match previously existing subsets that achieve the target sum.

### Minimum Sized Subset Computation

Base cases:

- The first **column** refers to prefixes that compose a sum of 0. Since this can only be done by the empty set and the empty set has a cardinality of 0, any such prefix must have a minimum cardinality of 0 for a target sum of 0. See `line 101`, its comment and its enclosing loop.

- The first **row** refers to varying target sums, and seeing if they can be composed by the first element in the sequence. If this is the case, then the set containing such a singleton has a cardinality of 1. Otherwise, the minimum cardinality remains at its initial value of "infinity" to indicate that the set cannot be composed by any number of elements. See `line 128`, its comment, and its enclosing branch and loop.

Recursive cases:

- Since we have initialized all values of the table to infinity (see `line 11` and `line 82` for my implementation of this concept), we do not have to clutter our code with additional branching based on feasibility of a particular case.

- If we are dealing with the exclude case, the new element does not contribute to any of the data, so the minimum cardinality remains the same as the input sequence right up to before the new element occurs. See `line 147`.

- If we are dealing with the include case (but also, generally), there are 3 possibilities:

  1. The "previous" (excluding the new element) minimum cardinality of any satisfying subset is exactly equal to the minimum cardinality of any satisfying subset which includes the element. See the conditional on `line 164`.

  2. The "previous" (excluding the new element) minimum cardinality of any satisfying subset is strictly greater than the minimum cardinality of any satisfying subset which includes the element. See the conditional on `line 174`.

  3. The "previous" (excluding the new element) minimum cardinality of any satisfying subset is strictly less than the minimum cardinality of any satisfying subset which includes the element. See the conditional on `line 174`.

    The new minimum cardinality is simply the minimum between the two cases. See `line 160`.

### Lexicographically First Minimum Size Subset Extraction

The approach to this problem was different than the raw statistics above, so I will preface it before diving in.

The first thing we might observe is that if we start at the "end" entry (the entry corresponding to the full input sequence and whole target sum) and trace back through the table while biasing ourselves toward only choosing elements whose indices exist in minimum cardinality subsets, then we will always have a set of indices which correspond to the lexicographically *last* minimum sized subset. It follows that if we reverse the order of the input sequence that we will reverse the lexicographic ordering of indices which compose minimum cardinality subsets as well, thus yielding the lexicographically *first* subset of the original input sequence. This reversal will be the first operation of the `solve` member function of the class; see `line 74`. This is the big picture idea, the rest is left to implementation details.

The data which this approach cruxes on is a flag which will indicate to our extraction algorithm whether or not a particular index should be included in the set it returns. We can add this as a property of the existing `ssum_data` structure -- call it `includes`. It is initialized to false and changed as necessary. See `line 82`.

Dynamic programming table flag population...

Base cases:

- Since no positive integer can be included in a set which composes a sum of 0, it must be the case that the entire left column of the table has this flag set to false. This is redundant in the code, but good for legibility. See `line 94` and related comment.

- If the first element in the reversed sequence is exactly the target sum, then that must be included in the lexicographically first subset since its minimum cardinality is 1. Else, the flag remains false. See `line 121`.

Recursive cases:

- The main question here is, "when do we want to include an element?" The answer is when it belongs to a set of overall minimum cardinality. Thus, we only want to set this flag in the include case of the nested loop. However, we don't want to set it *any* time we can include a particular element in a satisfying subset since it has to be of minimum cardinality. Thus, the solution is to only set this flag to be true when the new element's inclusion in the subset will result in either a new, smaller minimum cardinality compared to what has previously been discovered **or** when its inclusion will result in a maintained minimum cardinality. In other words, we only want to include the new element when its inclusion in a particular subset results in its cardinality being less than or equal to what it previously was before this element's consideration. See `line 164` to `line 190`, specifically taking note of `line 172`, `line 181`, `line 189` and related comments.

At this point, we have everything we need to extract the lexicographically first subset from the original sequence. However, because of my chosen implementation, there are some annoying kinks to work out.

Now that these flags are populated, we can perform our traceback. We start at the bottom right entry, referring to the result of the overall problem. We will "pop off" any elements marked with the include flag, starting at the original target and continuing until the target sum is exactly 0. Note that it would have been equivalent on `line 211` to put `while (x != 0)`, but the existing version is easier to follow based on the decrementation of the intermediate target. We will shorten the reversed input sequence element by element (see `line 223`), until we have pushed all of our indices into our vector that represents the lexicographically first subset.

After the while loop ends, we will be left with a vector of indices that encode the lexicographically first starting from the end of the reversed sequence. In order to normalize the indices for our report, we must take care of the offset imposed by the reversal. To do this, we subtract whatever index we got from the size and then also subtract 1 from that (to take care of the 0 based indexing implementation). Finally (although, in retrospect, might have been more appropriate elsewhere), we reverse the input sequence to its initial order before returning the lexicographically first subset.

---

## Data

### Run 1

![](run1.png)

### Run 2

![](run2.png)

### Run 3

![](run3.png)
