/* Copyright 2022 Jorge Bellon-Castro
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef EYTZINGER_ARRAY_HPP
#define EYTZINGER_ARRAY_HPP

#include <array>
#include <cassert>
#include <climits>
#include <cstddef>
#include <functional>

// Based on the paper
// ARRAY LAYOUTS FOR COMPARISON-BASED SEARCHING, by Paul-Virak Khuong and Pat Morin

template <class T, size_t N, class Compare = std::less<T>>
class eytzinger_array : protected Compare, private std::array<T, N> {
    using array = std::array<T, N>;

   public:
    constexpr eytzinger_array(array arr, const Compare &comp = {})
        : Compare(comp), array{make_eytzinger(arr, *this)} {}

    using iterator = array::const_iterator;
    using array::cbegin;
    using array::cend;
    using array::size;

    auto begin() const { return cbegin(); }
    auto end() const { return cend(); }
    const T *data() const { return array::data(); }

    constexpr const T &operator[](size_t i) const {
        return array::operator[](i);
    }

    constexpr iterator find(const T &value) const {
        unsigned long k = 1;
        while (k <= N) {
            k = 2 * k + Compare::operator()(operator[](k - 1), value);
        }
        k >>= __builtin_ffsl(~k);
        return cbegin() + k - 1;
    }

   private:
    constexpr static size_t breadth_first_copy(const array &in, array &out,
                                               size_t i = 0, size_t k = 1) {
        if (k <= N) {
            i = breadth_first_copy(in, out, i, k * 2);
            out[k - 1] = in[i++];
            i = breadth_first_copy(in, out, i, k * 2 + 1);
        }
        return i;
    }

    constexpr static std::array<T, N> make_eytzinger(std::array<T, N> &arr,
                                                     const Compare &comp) {
        std::sort(arr.begin(), arr.end(), comp);
        std::array<T, N> result;
        breadth_first_copy(arr, result);
        return result;
    }
};

// Deduction guides for eyztinger_array<T, N>
template <class T, size_t N>
eytzinger_array(std::array<T, N>) -> eytzinger_array<T, N>;

template <class T, size_t N>
eytzinger_array(const T (&)[N]) -> eytzinger_array<T, N>;

/* Example of use:
 * constexpr eytzinger_array<int, 5> a({1, 2, 3, 4, 5});
 * auto it = a.find(5);
 * assert( std::distance(a.begin(), it) == 4 );
 */

#endif // EYTZINGER_ARRAY_HPP
