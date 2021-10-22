#pragma once

#include <cstddef>

namespace std {

template <typename T, size_t N>
struct array {
	using value_type = T;
	T data[N];
	size_t size() {
		return N;
	}
	operator T*() {
		return data;
	}
	T& operator[](size_t const idx) {
		return data[idx];
	}
	T const& operator[](size_t const idx) const {
		return data[idx];
	}
	T& operator*() {
		return *data;
	}
	T const& operator*() const {
		return *data;
	}
	array& operator=(array const& other) {
		for (size_t i = 0; i < N; i++) {
			data[i] = other.data[i];
		}
		return *this;
	}
};

}  // namespace std
