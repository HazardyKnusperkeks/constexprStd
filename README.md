# constexprStd
This library provides some part of the STL as `constexpr` version.
The implementation may not be as efficient as your standard STL, but can be used at compile time.

As it is an implementation it contains no API documentation, for that I recommend [cppreference.com](http://en.cppreference.com/w/).
In addition to supply `constexpr` versions of functions which are not declared `constexpr` by the standard, it also contains some fixes.
For example `std::advance` should be `constexpr` as of C++17, but the GCC implementation of it has a bug in 7.1 and 7.2, see [Bug 81912](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81912).

## Usage
As this is a `constexpr` library just clone it into your project directory and add `include` to your includepath.
Projects based on Qt can `include(constexprStd/constexprStd.pri)` in their project file.

And then you use the library just like the STL.  
Instead of `#include <algorithm>` you write `#include <constexprStd/algorithm>` and the content is in the namespace `constexprStd::` instead of `std::`.

## Implementation
### [Algorihms library](http://en.cppreference.com/w/cpp/algorithm)
The execution policies and the overloads for the algorithms are not implemented, because as far as I know, there is no chance of performing paralell computations in `constexpr`.
But we offer some convenience overloads, e.g. instead of `copy(c.begin(), c.end(), dest)` you can call `copy(c, dest)`.

#### Non-modifying sequence operations
- [ ] all_of
- [ ] any_of
- [ ] none_of
- [ ] for_each
- [ ] for_each_n
- [X] count
- [X] count_if
- [ ] mismatch
- [X] equal
- [X] find
- [X] find_if
- [X] find_if_not
- [ ] find_end
- [ ] find_first_of
- [ ] adjacent_find
- [ ] search
- [ ] search_n

#### Modifying sequence operations
- [X] copy
- [ ] copy_if
- [ ] copy_n
- [ ] copy_backward
- [ ] move
- [ ] move_backward
- [ ] fill
- [ ] fill_n
- [ ] transform
- [ ] generate
- [ ] generate_n
- [ ] remover
- [ ] emove_if
- [ ] remove_copy
- [ ] remove_copy_if
- [ ] replace
- [ ] replace_if
- [ ] replace_copy
- [ ] replace_copy_if
- [ ] swap
- [ ] swap_ranges
- [ ] iter_swap
- [ ] reverse
- [ ] reverse_copy
- [ ] rotate
- [ ] rotate_copy
- [ ] shuffle
- [ ] sample
- [ ] unique
- [ ] unique_copy

#### Partitioning operations
- [ ] is_partitioned
- [ ] partition
- [ ] partition_copy
- [ ] stable_partition
- [ ] partition_point

#### Sorting operations
- [ ] is_sorted
- [ ] is_sorted_until
- [ ] sort
- [ ] partial_sort
- [ ] partial_sort_copy
- [ ] stable_sort
- [ ] nth_element

#### Binary search operations (on sorted ranges)
- [ ] lower_bound
- [ ] upper_bound
- [ ] binary_search
- [ ] equal_range

#### Set operations (on sorted ranges)
- [ ] merge
- [ ] inplace_merge
- [ ] includes
- [ ] set_difference
- [ ] set_intersection
- [ ] set_symmetric_difference
- [ ] set_union

#### Heap operations
- [ ] is_heap
- [ ] is_heap_until
- [ ] make_heap
- [ ] push_heap
- [ ] pop_heap
- [ ] sort_heap

#### Minimum/maximum operations
- [ ] max
- [ ] max_element
- [ ] min
- [ ] min_element
- [ ] minmax
- [ ] minmax_element
- [ ] clamp
- [ ] lexicographical_compare

#### Permutations
- [X] is_permutation
- [ ] next_permutation
- [ ] prev_permutation

#### Numeric operations
- [ ] iota
- [ ] accumulate
- [ ] inner_product
- [ ] adjacent_difference
- [ ] partial_sum
- [ ] reduce
- [ ] exclusive_scan
- [ ] inclusive_scan
- [ ] transform_reduce
- [ ] transform_exclusive_scan
- [ ] transform_inclusive_scan

#### Operations on uninitialized memory
- [ ] uninitialized_copy
- [ ] uninitialized_copy_n
- [ ] uninitialized_fill
- [ ] uninitialized_fill_n
- [ ] uninitialized_move
- [ ] uninitialized_move_n
- [ ] uninitialized_default_construct
- [ ] uninitialized_default_construct_n
- [ ] uninitialized_value_construct
- [ ] uninitialized_value_construct_n
- [ ] destroy_at
- [ ] destroy
- [ ] destroy_n

### [Function objects](http://en.cppreference.com/w/cpp/utility/functional)
#### Polymorphic function wrappers
- [ ] function
- [ ] mem_fn
- [X] bad_function_call: If used, use `std::`.
- [X] invoke

#### Negators
- [X] not_fn

### [Iterator library](http://en.cppreference.com/w/cpp/iterator)
#### Iterator primitives
These can be used directly from the `std::` namespace.

#### Iterator adaptors
- [ ] reverse_iterator
- [ ] make_reverse_iterator
- [ ] move_iterator
- [ ] make_move_iterator
- [ ] back_insert_iterator
- [ ] back_inserter
- [ ] front_insert_iterator
- [ ] front_inserter
- [X] insert_iterator
- [X] inserter

#### Stream iterators
Has to be checked if there are actually `constexpr` streams.

#### Iterator operations
Are actually defined as `constexpr`, but as stated earlier there are bugs in the GCC implementation, so we provide them too.
- [X] advance
- [X] distance
- [ ] next
- [ ] prev

#### Range access & Container access
These can be used directly from the `std::` namespace.

### [Utility library](http://en.cppreference.com/w/cpp/utility)
#### optional, variant and any
Will be handled seperately

#### Pairs and tuples
Is mostly `constexpr`, so nothing to do here

#### Swap, forward and move
- [ ] swap
- [X] exchange

The remainder is `constexpr`.

#### Type operations
These can be used directly from the `std::` namespace.

#### Relational operators
- [ ] operator!=
- [ ] operator>
- [ ] operator<=
- [ ] operator>=

#### Elementary string conversions
- [ ] to_chars
- [ ] from_chars
- [X] chars_format: This can be used directly from the `std::` namespace.

#### Hash support
- [ ] hash
