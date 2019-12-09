#include <config.hpp>
#include <unordered_map>
#include <fstream>

struct iequals {
    bool operator()(const std::string& a, const std::string& b) const {
        return string::iequals(a, b);
    }
};

static std::unordered_map<std::string, std::string, std::hash<std::string>, iequals> conf;

static std::string preprocess(const std::string &str, const std::string &whitespace=" \t") {
    if (str.empty())
        return str;
    auto s = string::trim(str, whitespace);
    size_t pos = 0;
    size_t end_pos = 0;
    while ((pos = s.find_first_of('[', 0)) != std::string::npos && (end_pos = s.find_first_of(']', pos + 1)) != std::string::npos) {
        const std::string macro = s.substr(pos + 1, end_pos - pos - 1);
        s.replace(pos, end_pos - pos + 1, config::get(macro));
    }
    return s;
}

void config::load(const std::string &fname, char comment) {
    std::ifstream config_file(fname);
    if (!config_file) {
        throw std::runtime_error("config file \'" + fname + "\' not found");
    }

    std::string line;
    while (std::getline(config_file, line)) {
        if (line.empty() || line[0] == comment)
            continue;
        auto tokens = string::split(line, "=");
        std::for_each(tokens.begin(), tokens.end(), [](std::string &str){ str = preprocess(str); });
        conf[tokens[0]] = tokens.size() > 1 ? tokens[1] : "";
    }

    config_file.close();
}

void config::parse(const std::vector <std::string> &args) {
    set("EXEC_NAME", args[0]);
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i].empty())
            continue;
        std::string key, value;
        const auto tokens = string::split(args[i], "=");
        if (tokens.size() > 1) {
            key = string::trim(tokens[0], " -\t");
            value = preprocess(tokens[1]);
        } else {
            key = string::trim(tokens[0], " -\t");
            value = preprocess(i < args.size() ? args[i + 1] : "");
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

std::string& config::get(const std::string &key) {
    if (!contains(key))
        throw std::runtime_error("config does not contain key \'" + key + '\'');
    return conf[key];
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
