#include <config.hpp>
#include <unordered_map>
#include <fstream>

const std::string config::DEFAULT_PATH = "config.txt";

std::unordered_map<std::string, std::string> conf;

void config::load(const std::string &file, char comment) {
    std::ifstream config_file(file);
    if (!config_file) {
        throw std::runtime_error("config file not found");
    }

    std::string line;
    while (std::getline(config_file, line)) {
        if (line.empty() && line[0] == comment)
            continue;
        std::string key, value;
        auto pos = line.find('=');
        key = line.substr(0, pos);
        value = line.substr(pos + 1, line.size());
        conf[key] = value;
    }

    config_file.close();
}

bool config::contains(const std::string &key) {
    return conf.find(key) != conf.end();
}

void config::set(const std::string &key, const std::string &value) {
    conf[key] = value;
}

std::string config::get(const std::string &key) {
    if (contains(key)) {
        return conf[key];
    } else {
        throw std::runtime_error("cannot find key '" + key + "\' in config");
    }
}

void config::remove(const std::string &key) {
    if (contains(key)) {
        conf.erase(key);
    }
}
