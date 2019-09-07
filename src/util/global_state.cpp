#include <global_state.hpp>

std::unordered_map<std::string, std::any> global_state::_state;

bool global_state::contains(const std::string &name) {
    return _state.find(name) != _state.end();
}

void global_state::set(const std::string &name, const std::any &obj) {
    _state.emplace(std::pair<std::string, std::any>(name, obj));
}

std::any global_state::get(const std::string &name) {
    auto it = _state.find(name);
    if (it != _state.end()) {
        return std::any();
    } else {
        return (*it).second;
    }
}

void global_state::clear() {
    _state.clear();
}
