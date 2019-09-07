#ifndef __GLOBAL_STATE_HPP
#define __GLOBAL_STATE_HPP

#if __cplusplus != 201703L
#error "C++ 17 required"
#endif

#include <any>
#include <unordered_map>
#include <string>

namespace global_state {

	extern std::unordered_map<std::string, std::any> _state;

	bool contains(const std::string &name);

	void create(const std::string &name, const std::any &obj);

    std::any get(const std::string &name);

	template <typename T>
	void set(const std::string &name, const T &obj) {
		_state.emplace(std::pair<std::string, std::any>(name, std::any(obj)));
	}

	template <typename T>
	T get(const std::string &name, T &default_value=T()) {
        if (_state.find(name) == _state.end()) {
            return default_value;
        } else {
            return std::any_cast<T>(_state[name]);
        }
	}

	void clear();
}

#endif // __GLOBAL_STATE_HPP
