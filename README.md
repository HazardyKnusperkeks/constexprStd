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
- [ ] advance
- [ ] distance
- [ ] next
- [ ] prev

#### Range access & Container access
These can be used directly from the `std::` namespace.
