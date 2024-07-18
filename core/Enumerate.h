#pragma once

template <typename T>
class Enumerate {
public:
	class Iterator {
	public:
		using iterator_type = typename T::iterator;
		using value_type = typename std::iterator_traits<iterator_type>::value_type;

		Iterator(iterator_type it, std::size_t idx) : it(it), idx(idx) {}

		Iterator& operator++() {
			++it;
			++idx;
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return it != other.it;
		}

		std::pair<std::size_t, value_type&> operator*() const {
			return { idx, *it };
		}

	private:
		iterator_type it;
		std::size_t idx;
	};

	Enumerate(T& container) : container(container) {}

	Iterator begin() {
		return Iterator(container.begin(), 0);
	}

	Iterator end() {
		return Iterator(container.end(), 0);
	}

private:
	T& container;
};

template <typename T>
Enumerate<T> enumerate(T& container) {
	return Enumerate<T>(container);
}