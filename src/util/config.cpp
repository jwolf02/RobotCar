#include <config.hpp>
#include <unordered_map>
#include <fstream>

std::unordered_map<std::string, std::string> conf;

void config::load(const std::string &file) {
    std::ifstream config_file(file);
    if (!config_file) {
        throw std::runtime_error("config file not found");
    }

    std::string line;
    while (std::getline(config_file, line)) {
        if (line.empty() && line[0] == '#')
            continue;
        std::string key, value;
        auto pos = line.find('=');
        key = line.substr(0, pos);
        value = line.substr(pos + 1, line.size());
        conf[key] = value;
    }

    config_file.close();
}

void config::set(const std::string &key, const std::string &value) {
    conf[key] = value;
}

std::string config::get(const std::string &key) {
    if (conf.find(key) != conf.end()) {
        return conf[key];
    } else {
        throw std::runtime_error("key not found: " + key);
    }
}

void config::remove(const std::string &key) {
    if (conf.find(key) != conf.end()) {
        conf.erase(key);
    }
}
