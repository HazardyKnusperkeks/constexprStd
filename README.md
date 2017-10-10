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

## Noteable differences to `std::`
### General
I've added conditional noexcept statements to nearly all (member) functions.
So what can be noexcept should also be marked as such.
I've only omited the statements when a throw is actually used in its body.

### Allocator
Since we can neither use placement new, nor reinterpret_cast at compile time, the allocator does not fulfill the [allocator concept](http://en.cppreference.com/w/cpp/concept/Allocator) and can probably not be used for `std::` containers.

It lacks the copy/move construction and assignment.
The equality operator tests if it is the same object.
And rebinding is not possible.

When allocating multiple elements you can not easyly construct them in constexpr context.
```C++
Allocator<T, N> a;
T *singleElement = a.allocate(1);
T *multipleElements = a.allocate(3);

a.construct(singleElement, ...); //This is fine
a.construct(multipleElements, ...); //This also, initializes the first element
a.constrcut(&multipleElements[0], ...); //Works also
a.constrcut(&multipleElements[1], ...); //Does not work in a constexpr context, does work at runtime

//What you have to do is:
T *multiSecondElement = a.getPointer(multipleElements, 1); //This is a operation in O(N), so use sparely
//                                   ^^                ^
//                            base pointer of array    offset to the pointer, here multipleElements[1]
a.construct(multiSecondElement, ...);

//But you can provide a hint, so it is faster
T *multiThirdElement = a.getPointer(multipleElements, 2, static_cast<std::size_t>(constexprStd::distance(singleElement, multipleElements)));
//                                  ^^^^^^^^^^^^^^^^^^^  ^^                              ^^^^^^^^^^^^^^^^
//                                  both as before       cast if you use -Wconversion    the actual hint, the difference from the first element to the base pointer
a.construct(multiThirdElement, ...);
```

Even if you provide a hint which is after the actual base pointer the algorithm is correct and complete, but the runtime is as bad as it can get.
If you do not have access to the very first element, you can use what ever you have, that is before our base pointer, everything helps.

### `constexprStd::variant`
#### Incompatibilites to `std::variant`
Using types with a non trivial destructor is not possible in `constexpr` context, so they can't be used in `constexrStd::variant` either.
But all other types can be used, even `emplace` and the assignment operator, which are not `constexpr` in their `std::` incarnation.
This comes at the cost, that the implementation does not meet all requirements.
When emplacing a new type, the value is not direct initialized, but move assigned to the storage where another type was before.
This means that some types are not save to be used in `constexprStd::variant`.
If you want to use such a type and need direct initialization, add a non trivial destructor to it and the variant behaves like `std::variant`.

#### Additions
- It is possible to (un-)equal compare different types of variant. They only compare as equal, iff both hold the same type and the equal comparison of this type returns true.
- It is possible to assign the content of diffrent types of variant. But only if the currently hold alternative of the right hand side is also an alternative of the left hand side, or the right hand side is valueless. In any other case a `constexprStd::BadVariantAssignment` exception is thrown.

### `constexprStd::pair`
The default constructor is never explicit.

### `constexprStd::set`
Because it is based on our [Allocator](#Allocator) it's not fully compatible with `std::set`, we do not provide extract() and the corresponding insert() functions.
And because we can not call a non trivial destructor in constexpr `constexprStd::set` does *NOT* delete the content of the nodes in its destructor.
In constexpr mode this is not necessary, because only types with a non trivial destructor could leak memory, but if you intend to use the set in run time (which is advised against) you have to manually call clear or use the `constexprStd::setDestroy` wrapper.

Note that emplace() and emplace_hint() are not constexpr, because without copying or moving we could not provide exception safety.

It the provided `constexprStd::allocator` runs out of memory it falls back to `std::allocator`.
This results in a compile time error, when happening in constexpr mode, in this case you have to increase the preallocated memory.

## Implementation
### [Algorihms library](http://en.cppreference.com/w/cpp/algorithm)
The execution policies and the overloads for the algorithms are not implemented, because as far as I know, there is no chance of performing paralell computations in `constexpr`.
But we offer some convenience overloads, e.g. instead of `copy(c.begin(), c.end(), dest)` you can call `copy(c, dest)`.

#### Non-modifying sequence operations
- [X] all_of
- [X] any_of
- [X] none_of
- [X] for_each
- [X] for_each_n
- [X] count
- [X] count_if
- [X] mismatch
- [X] equal
- [X] find
- [X] find_if
- [X] find_if_not
- [X] find_end
- [X] find_first_of
- [X] adjacent_find
- [X] search
- [ ] search_n

#### Modifying sequence operations
- [X] copy
- [X] copy_if
- [X] copy_n
- [X] copy_backward
- [X] move
- [X] move_backward
- [X] fill
- [X] fill_n
- [X] transform
- [X] generate
- [X] generate_n
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
- [X] lexicographical_compare

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

#### Searchers
- [X] default_searcher
- [ ] boyer_moore_searcher
- [ ] boyer_moore_horspool_searcher

### [Iterator library](http://en.cppreference.com/w/cpp/iterator)
#### Iterator primitives
These can be used directly from the `std::` namespace.

#### Iterator adaptors
- [X] reverse_iterator: This can be used directly from the `std::` namespace.
- [X] make_reverse_iterator: This can be used directly from the `std::` namespace.
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
- [X] next
- [X] prev

#### Range access & Container access
These can be used directly from the `std::` namespace.

### ["Dynamic" memory management](http://en.cppreference.com/w/cpp/memory)
#### Allocators
- [X] allocator

### [Utility library](http://en.cppreference.com/w/cpp/utility)
- [X] bitset

#### optional, variant and any
- [ ] optional
- [X] variant
- [ ] any
- [X] in_place*: These can be used directly from the `std::` namespace.

#### Pairs and tuples
- [X] pair
- [ ] tuple
- [X] apply: This can be used directly from the `std::` namespace.
- [X] make_from_tuple: This can be used directly from the `std::` namespace.
- [X] piecewise_construct_t: This can be used directly from the `std::` namespace.
- [X] piecewise_construct: This can be used directly from the `std::` namespace.
- [X] integer_sequence: This can be used directly from the `std::` namespace.

#### Swap, forward and move
- [X] swap
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

### [Containers Library](http://en.cppreference.com/w/cpp/container)
- [X] set
