#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace bre {

class Config {
public:
    // 获取单例实例
    static Config& getInstance() {
        static Config* instance = new Config();
        return *instance;
    }

    // 获取配置项的值（返回空字符串表示未找到）
    std::string Get(const std::string& key) const {
        auto it = configMap.find(key);
        if (it != configMap.end()) {
            return it->second;
        }
        return "";  // 或返回特定标志字符串
    }

private:
    Config() {
        loadConfig();
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::map<std::string, std::string> configMap;

    void loadConfig() {
        std::ifstream configFile("config.txt");
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open config file!!");
        }

        std::string line;
        while (std::getline(configFile, line)) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, ':') && std::getline(iss, value)) {
                removeAllSpace(key);
                removeAllSpace(value);
                configMap[key] = value;
            }
        }
    }

    void removeAllSpace(std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    }
};

} // namespace bre

#endif // CONFIG_H
