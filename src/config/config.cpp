#include "config.hpp"
#include <unordered_map>
#include <fstream>

struct iequals {
    bool operator()(const std::string& a, const std::string& b) const {
        return string::iequals(a, b);
    }
};

std::unordered_map<std::string, std::string, std::hash<std::string>, iequals> conf;

void config::load(const std::string &fname, char comment) {
    std::ifstream config_file(fname);
    if (!config_file) {
        throw std::runtime_error("config file \'" + fname + "\' not found");
    }

    std::string line;
    while (std::getline(config_file, line)) {
        if (line.empty() || line[0] == comment)
            continue;
        const auto tokens = string::split(line, "=");
        conf[string::trim(tokens[0])] = tokens.size() > 1 ? string::trim(tokens[1]) : "";
    }

    config_file.close();
}

void config::parse(const std::vector <std::string> &args) {
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i].empty())
            continue;
        std::string key, value;
        const auto tokens = string::split(args[i], "=");
        if (tokens.size() > 1) {
            key = string::trim(tokens[0], " -\t");
            value = string::trim(tokens[1]);
        } else {
            key = string::trim(tokens[0], " -\t");
            value = string::trim(i < args.size() ? args[i + 1] : "");
            i += 1;
        }
        conf[key] = value;
    }
}

bool config::contains(const std::string &key) {
    return conf.find(key) != conf.end();
}

void config::set(const std::string &key, const std::string &value) {
    conf[key] = value;
}

std::string config::get(const std::string &key) {
    return contains(key) ? conf[key] : throw std::runtime_error("config does not contain key \'" + key + '\'');
}

std::vector<std::string> config::get_keys() {
    std::vector<std::string> keys(conf.size());
    for (const auto &p : conf) {
        keys.push_back(p.first);
    }
    return keys;
}

void config::remove(const std::string &key) {
    if (contains(key)) {
        conf.erase(key);
    }
}
