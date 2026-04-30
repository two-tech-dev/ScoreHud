#pragma once

#include <endstone/endstone.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace scorehud {

struct RuntimeConfig {
    bool tagFactoryEnabled{true};
    int tagFactoryUpdateSeconds{5};
    bool memoryTagsEnabled{false};
    std::optional<std::string> timezone;
    std::string timeFormat{"H:i:s"};
    std::string dateFormat{"d-m-Y"};
};

struct BoardConfig {
    bool flickerTitles{false};
    int titlePeriodSeconds{5};
    std::string title{"§l§aServer §dName"};
    std::vector<std::string> titles{"§l§aServer §dName"};
    std::vector<std::string> defaultLines;
};

class ScoreHudSettings {
public:
    void load(endstone::Plugin &plugin);

    [[nodiscard]] const RuntimeConfig &runtime() const;
    [[nodiscard]] const BoardConfig &board() const;
    [[nodiscard]] const std::vector<std::string> &defaultLines() const;

private:
    void ensureDefaultFiles(endstone::Plugin &plugin);
    void loadRuntimeConfig(const std::filesystem::path &path, endstone::Logger &logger);
    void loadBoardConfig(const std::filesystem::path &path, endstone::Logger &logger);
    void validate(endstone::Logger &logger);

    RuntimeConfig runtime_;
    BoardConfig board_;
};

}  // namespace scorehud
