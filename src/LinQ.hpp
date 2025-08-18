#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace LinQ {
template <typename Iterator, typename Predicate> class WhereIterator {
public:
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using iterator_category = std::forward_iterator_tag;

    WhereIterator(Iterator it, Iterator end, Predicate pred) : current(it), end(end), predicate(pred) {
        while (current != end && !predicate(*current)) {
            ++current;
        }
    }

    WhereIterator& operator++() {
        ++current;
        while (current != end && !predicate(*current)) {
            ++current;
        }
        return *this;
    }

    bool operator==(const WhereIterator& other) const {
        return current == other.current;
    }

    bool operator!=(const WhereIterator& other) const {
        return !(*this == other);
    }

    reference operator*() const {
        return *current;
    }

private:
    Iterator current;
    Iterator end;
    Predicate predicate;
};

template <typename Iterator, typename Transformer> class SelectIterator {
public:
    using value_type = decltype(std::declval<Transformer>()(std::declval<typename std::iterator_traits<Iterator>::value_type>()));
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using pointer = void;
    using reference = value_type;
    using iterator_category = std::forward_iterator_tag;

    SelectIterator(Iterator it, Transformer trans) : current(it), transformer(trans) {}

    SelectIterator& operator++() {
        ++current;
        return *this;
    }

    bool operator==(const SelectIterator& other) const {
        return current == other.current;
    }

    bool operator!=(const SelectIterator& other) const {
        return !(*this == other);
    }

    value_type operator*() const {
        return transformer(*current);
    }

private:
    Iterator current;
    Transformer transformer;
};

template <typename Iterator> class DistinctIterator {
public:
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using iterator_category = std::forward_iterator_tag;

    DistinctIterator(Iterator it, Iterator end) : current(it), end(end), seen(std::make_shared<std::unordered_set<value_type>>()) {
        advanceToNextUnique();
    }

    DistinctIterator& operator++() {
        ++current;
        advanceToNextUnique();
        return *this;
    }

    bool operator==(const DistinctIterator& other) const {
        return current == other.current;
    }

    bool operator!=(const DistinctIterator& other) const {
        return !(*this == other);
    }

    reference operator*() const {
        return *current;
    }

private:
    void advanceToNextUnique() {
        while (current != end && seen->count(*current)) {
            ++current;
        }
        if (current != end) {
            seen->insert(*current);
        }
    }

    Iterator current;
    Iterator end;
    std::shared_ptr<std::unordered_set<value_type>> seen;
};

template <typename T, typename Iterator> class Range;

template <typename T, typename Iterator> class OrderedRange;

template <typename TKey, typename TElement> class Grouping : public Range<TElement, typename std::vector<TElement>::iterator> {
public:
    Grouping(const TKey& key, std::shared_ptr<std::vector<TElement>> elements) : Range<TElement, typename std::vector<TElement>::iterator>(elements), key(key) {}

    TKey Key() const {
        return key;
    }

private:
    TKey key;
};

template <typename T, typename Iterator> class Range {
public:
    Range(Iterator begin, Iterator end) : beginIndex(begin), endIndex(end), dataPointer(nullptr) {}

    Range(std::shared_ptr<std::vector<T>> data) : dataPointer(data), beginIndex(dataPointer->begin()), endIndex(dataPointer->end()) {}

    Iterator begin() const {
        return beginIndex;
    }
    Iterator end() const {
        return endIndex;
    }

    template <typename Predicate> auto Where(Predicate p) const {
        using NewIterator = WhereIterator<Iterator, Predicate>;
        return Range<T, NewIterator>(NewIterator(beginIndex, endIndex, p), NewIterator(endIndex, endIndex, p));
    }

    template <typename Transformer> auto Select(Transformer t) const {
        using NewIterator = SelectIterator<Iterator, Transformer>;
        using NewValueType = typename NewIterator::value_type;
        return Range<NewValueType, NewIterator>(NewIterator(beginIndex, t), NewIterator(endIndex, t));
    }

    std::vector<T> ToVector() const {
        return std::vector<T>(begin(), end());
    }

    auto ToArray() const {
        return ToVector();
    }

    auto ToCArray() const {
        const size_t count = std::distance(begin(), end());
        if (count == 0) {
            return std::make_pair(std::unique_ptr<T[]>(nullptr), size_t{0});
        }
        auto arrayPointer = std::make_unique<T[]>(count);
        std::copy(begin(), end(), arrayPointer.get());
        return std::make_pair(std::move(arrayPointer), count);
    }

    template <typename KeySelector>
    auto ToMap(KeySelector keySelector) const {
        using TKey = decltype(keySelector(std::declval<T>()));
        std::map<TKey, T> map;
        for (const auto& item : *this) {
            map.emplace(keySelector(item), item);
        }
        return map;
    }

    template <typename KeySelector, typename ValueSelector>
    auto ToMap(KeySelector keySelector, ValueSelector valueSelector) const {
        using TKey = decltype(keySelector(std::declval<T>()));
        using TValue = decltype(valueSelector(std::declval<T>()));
        std::map<TKey, TValue> map;
        for (const auto& item : *this) {
            map.emplace(keySelector(item), valueSelector(item));
        }
        return map;
    }

    template <typename KeySelector>
    auto ToUnorderedMap(KeySelector keySelector) const {
        using TKey = decltype(keySelector(std::declval<T>()));
        std::unordered_map<TKey, T> map;
        for (const auto& item : *this) {
            map.emplace(keySelector(item), item);
        }
        return map;
    }

    template <typename KeySelector, typename ValueSelector>
    auto ToUnorderedMap(KeySelector keySelector, ValueSelector valueSelector) const {
        using TKey = decltype(keySelector(std::declval<T>()));
        using TValue = decltype(valueSelector(std::declval<T>()));
        std::unordered_map<TKey, TValue> map;
        for (const auto& item : *this) {
            map.emplace(keySelector(item), valueSelector(item));
        }
        return map;
    }

    bool Any() const {
        return begin() != end();
    }

    template <typename Predicate> bool Any(Predicate p) const {
        return std::any_of(begin(), end(), p);
    }

    template <typename Predicate> bool All(Predicate p) const {
        return std::all_of(begin(), end(), p);
    }

    T First() const {
        if (begin() == end()) {
            throw std::runtime_error("Sequence contains no elements");
        }
        return *begin();
    }

    template <typename Predicate> T First(Predicate p) const {
        return Where(p).First();
    }

    std::optional<T> FirstOrDefault() const {
        if (begin() == end()) {
            return std::nullopt;
        }
        return *begin();
    }

    template <typename Predicate> std::optional<T> FirstOrDefault(Predicate p) const {
        return Where(p).FirstOrDefault();
    }

    long long Count() const {
        return std::distance(begin(), end());
    }

    template <typename Predicate> long long Count(Predicate p) const {
        return Where(p).Count();
    }

    auto Skip(size_t count) const {
        auto it = begin();
        std::advance(it, std::min(count, (size_t)std::distance(it, end())));
        return Range<T, Iterator>(it, end());
    }

    auto Take(size_t count) const {
        auto it = begin();
        size_t dist = std::distance(it, end());
        auto newEnd = begin();
        std::advance(newEnd, std::min(count, dist));
        return Range<T, Iterator>(begin(), newEnd);
    }

    T Sum() const {
        return std::accumulate(begin(), end(), T{});
    }

    T Min() const {
        if (begin() == end()) {
            throw std::runtime_error("Sequence contains no elements");
        }
        return *std::min_element(begin(), end());
    }

    T Max() const {
        if (begin() == end()) {
            throw std::runtime_error("Sequence contains no elements");
        }
        return *std::max_element(begin(), end());
    }

    double Average() const {
        auto s = Sum();
        auto c = Count();
        if (c == 0) {
            throw std::runtime_error("Sequence contains no elements");
        }
        return static_cast<double>(s) / c;
    }

    auto Distinct() const {
        using NewIterator = DistinctIterator<Iterator>;
        return Range<T, NewIterator>(NewIterator(beginIndex, endIndex), NewIterator(endIndex, endIndex));
    }

    template <typename KeySelector> auto OrderBy(KeySelector ks) const {
        auto data = std::make_shared<std::vector<T>>(begin(), end());
        std::sort(data->begin(), data->end(), [&](const T& a, const T& b) { return ks(a) < ks(b); });
        return OrderedRange<T, typename std::vector<T>::iterator>(data);
    }

    template <typename KeySelector> auto OrderByDescending(KeySelector ks) const {
        auto data = std::make_shared<std::vector<T>>(begin(), end());
        std::sort(data->begin(), data->end(), [&](const T& a, const T& b) { return ks(a) > ks(b); });
        return OrderedRange<T, typename std::vector<T>::iterator>(data);
    }

    auto Reverse() const {
        auto data = std::make_shared<std::vector<T>>(begin(), end());
        std::reverse(data->begin(), data->end());
        return Range<T, typename std::vector<T>::iterator>(data);
    }

    template <typename KeySelector> auto GroupBy(KeySelector ks) const {
        using TKey = decltype(ks(std::declval<T>()));
        auto map = std::map<TKey, std::shared_ptr<std::vector<T>>>();

        for (const auto& item : *this) {
            TKey key = ks(item);
            if (map.find(key) == map.end()) {
                map[key] = std::make_shared<std::vector<T>>();
            }
            map[key]->push_back(item);
        }

        using GroupingType = Grouping<TKey, T>;
        auto groups = std::make_shared<std::vector<GroupingType>>();
        for (auto& pair : map) {
            groups->emplace_back(pair.first, pair.second);
        }

        return Range<GroupingType, typename std::vector<GroupingType>::iterator>(groups);
    }

    template <typename OtherIterator> auto Union(const Range<T, OtherIterator>& other) const {
        auto data = std::make_shared<std::vector<T>>();
        auto seen = std::unordered_set<T>();

        for (const auto& item : *this) {
            if (seen.find(item) == seen.end()) {
                seen.insert(item);
                data->push_back(item);
            }
        }

        for (const auto& item : other) {
            if (seen.find(item) == seen.end()) {
                seen.insert(item);
                data->push_back(item);
            }
        }

        return Range<T, typename std::vector<T>::iterator>(data);
    }

    template <typename OtherIterator> auto Intersect(const Range<T, OtherIterator>& other) const {
        auto data = std::make_shared<std::vector<T>>();
        auto otherSet = std::unordered_set<T>(other.begin(), other.end());
        auto seen = std::unordered_set<T>();

        for (const auto& item : *this) {
            if (otherSet.count(item) && seen.find(item) == seen.end()) {
                data->push_back(item);
                seen.insert(item);
            }
        }

        return Range<T, typename std::vector<T>::iterator>(data);
    }

    template <typename OtherIterator> auto Except(const Range<T, OtherIterator>& other) const {
        auto data = std::make_shared<std::vector<T>>();
        auto otherSet = std::unordered_set<T>(other.begin(), other.end());
        auto seen = std::unordered_set<T>();

        for (const auto& item : *this) {
            if (otherSet.find(item) == otherSet.end() && seen.find(item) == seen.end()) {
                data->push_back(item);
                seen.insert(item);
            }
        }

        return Range<T, typename std::vector<T>::iterator>(data);
    }

    template <typename OtherIterator> auto Concat(const Range<T, OtherIterator>& other) const {
        auto data = std::make_shared<std::vector<T>>(begin(), end());
        data->insert(data->end(), other.begin(), other.end());
        return Range<T, typename std::vector<T>::iterator>(data);
    }

    template <typename TAccumulate, typename Func> TAccumulate Aggregate(TAccumulate seed, Func func) const {
        TAccumulate result = seed;
        for (const auto& item : *this) {
            result = func(result, item);
        }
        return result;
    }

    T ElementAt(size_t index) const {
        auto it = begin();
        for (size_t i = 0; i < index; ++i) {
            if (it == end()) {
                throw std::out_of_range("Index out of range");
            }
            ++it;
        }
        if (it == end()) {
            throw std::out_of_range("Index out of range");
        }
        return *it;
    }

    std::optional<T> ElementAtOrDefault(size_t index) const {
        auto it = begin();
        for (size_t i = 0; i < index; ++i) {
            if (it == end()) {
                return std::nullopt;
            }
            ++it;
        }
        if (it == end()) {
            return std::nullopt;
        }
        return *it;
    }

    T Last() const {
        if (begin() == end()) {
            throw std::runtime_error("Sequence contains no elements");
        }
        Iterator it = begin();
        Iterator next = it;
        ++next;
        while (next != end()) {
            ++it;
            ++next;
        }
        return *it;
    }

    template <typename Predicate> T Last(Predicate p) const {
        return Where(p).Last();
    }

    std::optional<T> LastOrDefault() const {
        if (begin() == end()) {
            return std::nullopt;
        }
        Iterator it = begin();
        Iterator next = it;
        ++next;
        while (next != end()) {
            ++it;
            ++next;
        }
        return *it;
    }

    template <typename Predicate> std::optional<T> LastOrDefault(Predicate p) const {
        return Where(p).LastOrDefault();
    }

    T Single() const {
        auto it = begin();
        if (it == end()) {
            throw std::runtime_error("Sequence contains no elements");
        }
        auto next = it;
        ++next;
        if (next != end()) {
            throw std::runtime_error("Sequence contains more than one element");
        }
        return *it;
    }

    template <typename Predicate> T Single(Predicate p) const {
        return Where(p).Single();
    }

    std::optional<T> SingleOrDefault() const {
        auto it = begin();
        if (it == end()) {
            return std::nullopt;
        }
        auto next = it;
        ++next;
        if (next != end()) {
            throw std::runtime_error("Sequence contains more than one element");
        }
        return *it;
    }

    template <typename Predicate> std::optional<T> SingleOrDefault(Predicate p) const {
        return Where(p).SingleOrDefault();
    }

protected:
    std::shared_ptr<std::vector<T>> dataPointer;
    Iterator beginIndex;
    Iterator endIndex;
};

template <typename T, typename Iterator> class OrderedRange : public Range<T, Iterator> {
public:
    using Range<T, Iterator>::Range;

    template <typename KeySelector> auto ThenBy(KeySelector ks) -> OrderedRange<T, Iterator>& {
        if (!this->dataPointer)
            return *this;
        std::stable_sort(this->beginIndex, this->endIndex, [&](const T& a, const T& b) { return ks(a) < ks(b); });
        return *this;
    }

    template <typename KeySelector> auto ThenByDescending(KeySelector ks) -> OrderedRange<T, Iterator>& {
        if (!this->dataPointer)
            return *this;
        std::stable_sort(this->beginIndex, this->endIndex, [&](const T& a, const T& b) { return ks(a) > ks(b); });
        return *this;
    }
};

template <typename T, std::size_t N> auto From(const T (&collection)[N]) {
    return Range<T, const T*>(collection, collection + N);
}

template <typename C> auto From(const C& collection) {
    return Range<typename C::value_type, typename C::const_iterator>(collection.begin(), collection.end());
}

} // namespace LinQ