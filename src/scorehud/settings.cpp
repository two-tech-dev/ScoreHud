#include "scorehud/settings.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace scorehud {
namespace {

const char *kDefaultRuntimeConfig = R"(config-version: 11

tag-factory:
  enable: true
  update-period: 5
  enable-memory-tags: false

time:
  zone: false
  format:
    time: "H:i:s"
    date: "d-m-Y"
)";

const char *kDefaultBoardConfig = R"(scorehud-version: 3

titles:
  flicker: false
  period: 5
  title: "§l§aServer §dName"
  lines:
  - "§l§aServer §dName"
  - "§l§bServer §cName"
  - "§l§cServer §bName"
  - "§l§dServer §aName"

default-board:
- ""
- "§dName: §5{scorehud.name}"
- ""
- "§bOnline: §a{scorehud.online} §c/ §d{scorehud.max_online}"
- ""
- "§6Please Vote for us!"
)";

std::string trim(std::string text)
{
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const auto last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string removeInlineComment(std::string line)
{
    bool quoted = false;
    char quote = '\0';
    for (std::size_t i = 0; i < line.size(); ++i) {
        const auto ch = line[i];
        if ((ch == '"' || ch == '\'') && (i == 0 || line[i - 1] != '\\')) {
            if (!quoted) {
                quoted = true;
                quote = ch;
            }
            else if (quote == ch) {
                quoted = false;
            }
        }
        if (ch == '#' && !quoted) {
            return line.substr(0, i);
        }
    }
    return line;
}

std::string unquote(std::string text)
{
    text = trim(std::move(text));
    if (text.size() >= 2 && ((text.front() == '"' && text.back() == '"') ||
                             (text.front() == '\'' && text.back() == '\''))) {
        return text.substr(1, text.size() - 2);
    }
    return text;
}

std::string valueAfterColon(const std::string &line)
{
    const auto colon = line.find(':');
    if (colon == std::string::npos) {
        return "";
    }
    return unquote(line.substr(colon + 1));
}

bool parseBool(std::string text, bool fallback)
{
    text = trim(std::move(text));
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) { return std::tolower(ch); });
    if (text == "true" || text == "yes" || text == "1") {
        return true;
    }
    if (text == "false" || text == "no" || text == "0") {
        return false;
    }
    return fallback;
}

int parsePositiveInt(const std::string &text, int fallback)
{
    try {
        return std::max(1, std::stoi(trim(text)));
    }
    catch (const std::exception &) {
        return fallback;
    }
}

std::vector<std::string> readLines(const std::filesystem::path &path)
{
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Unable to read " + path.string());
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(removeInlineComment(line));
    }
    return lines;
}

void writeDefaultFile(const std::filesystem::path &path, const char *content, endstone::Logger &logger)
{
    if (std::filesystem::exists(path)) {
        return;
    }

    std::ofstream file(path);
    if (!file) {
        logger.error("Unable to create {}", path.string());
        return;
    }
    file << content;
}

}  // namespace

void ScoreHudSettings::load(endstone::Plugin &plugin)
{
    runtime_ = RuntimeConfig{};
    board_ = BoardConfig{};
    ensureDefaultFiles(plugin);

    loadRuntimeConfig(plugin.getDataFolder() / "config.yml", plugin.getLogger());
    loadBoardConfig(plugin.getDataFolder() / "scorehud.yml", plugin.getLogger());
    validate(plugin.getLogger());

    if (runtime_.timezone.has_value()) {
#ifdef _WIN32
        _putenv_s("TZ", runtime_.timezone->c_str());
        _tzset();
#else
        setenv("TZ", runtime_.timezone->c_str(), 1);
        tzset();
#endif
    }
}

const RuntimeConfig &ScoreHudSettings::runtime() const
{
    return runtime_;
}

const BoardConfig &ScoreHudSettings::board() const
{
    return board_;
}

const std::vector<std::string> &ScoreHudSettings::defaultLines() const
{
    return board_.defaultLines;
}

void ScoreHudSettings::ensureDefaultFiles(endstone::Plugin &plugin)
{
    std::error_code error;
    std::filesystem::create_directories(plugin.getDataFolder(), error);
    if (error) {
        plugin.getLogger().error("Unable to create data folder: {}", error.message());
        return;
    }

    writeDefaultFile(plugin.getDataFolder() / "config.yml", kDefaultRuntimeConfig, plugin.getLogger());
    writeDefaultFile(plugin.getDataFolder() / "scorehud.yml", kDefaultBoardConfig, plugin.getLogger());
}

void ScoreHudSettings::loadRuntimeConfig(const std::filesystem::path &path, endstone::Logger &logger)
{
    std::vector<std::string> lines;
    try {
        lines = readLines(path);
    }
    catch (const std::exception &error) {
        logger.warning("{}; using runtime defaults.", error.what());
        return;
    }

    enum class Section {
        Root,
        TagFactory,
        Time,
        TimeFormat,
    };

    auto section = Section::Root;
    for (const auto &rawLine : lines) {
        const auto line = trim(rawLine);
        if (line.empty()) {
            continue;
        }

        if (line == "tag-factory:") {
            section = Section::TagFactory;
            continue;
        }
        if (line == "time:") {
            section = Section::Time;
            continue;
        }
        if (section == Section::Time && line == "format:") {
            section = Section::TimeFormat;
            continue;
        }

        if (section == Section::TagFactory) {
            if (line.starts_with("enable:")) {
                runtime_.tagFactoryEnabled = parseBool(valueAfterColon(line), runtime_.tagFactoryEnabled);
            }
            else if (line.starts_with("update-period:")) {
                runtime_.tagFactoryUpdateSeconds =
                    parsePositiveInt(valueAfterColon(line), runtime_.tagFactoryUpdateSeconds);
            }
            else if (line.starts_with("enable-memory-tags:")) {
                runtime_.memoryTagsEnabled = parseBool(valueAfterColon(line), runtime_.memoryTagsEnabled);
            }
            continue;
        }

        if (section == Section::Time && line.starts_with("zone:")) {
            auto zone = valueAfterColon(line);
            if (!zone.empty() && zone != "false") {
                runtime_.timezone = zone;
            }
            continue;
        }

        if (section == Section::TimeFormat) {
            if (line.starts_with("time:")) {
                runtime_.timeFormat = valueAfterColon(line);
            }
            else if (line.starts_with("date:")) {
                runtime_.dateFormat = valueAfterColon(line);
            }
        }
    }
}

void ScoreHudSettings::loadBoardConfig(const std::filesystem::path &path, endstone::Logger &logger)
{
    std::vector<std::string> lines;
    try {
        lines = readLines(path);
    }
    catch (const std::exception &error) {
        logger.warning("{}; using board defaults.", error.what());
        return;
    }

    enum class Section {
        Root,
        Titles,
        TitleLines,
        DefaultBoard,
    };

    auto section = Section::Root;
    board_.titles.clear();
    board_.defaultLines.clear();

    for (const auto &rawLine : lines) {
        const auto line = trim(rawLine);
        if (line.empty()) {
            continue;
        }

        if (line == "titles:") {
            section = Section::Titles;
            continue;
        }
        if (line == "default-board:") {
            section = Section::DefaultBoard;
            continue;
        }
        if (section == Section::Titles && line == "lines:") {
            section = Section::TitleLines;
            continue;
        }

        if (section == Section::Titles) {
            if (line.starts_with("flicker:")) {
                board_.flickerTitles = parseBool(valueAfterColon(line), board_.flickerTitles);
            }
            else if (line.starts_with("period:")) {
                board_.titlePeriodSeconds = parsePositiveInt(valueAfterColon(line), board_.titlePeriodSeconds);
            }
            else if (line.starts_with("title:")) {
                board_.title = valueAfterColon(line);
            }
            continue;
        }

        if (section == Section::TitleLines && line.starts_with("-")) {
            board_.titles.push_back(unquote(line.substr(1)));
            continue;
        }

        if (section == Section::DefaultBoard && line.starts_with("-")) {
            board_.defaultLines.push_back(unquote(line.substr(1)));
            continue;
        }
    }
}

void ScoreHudSettings::validate(endstone::Logger &logger)
{
    if (board_.titles.empty()) {
        board_.titles.push_back(board_.title);
    }

    if (board_.defaultLines.empty()) {
        logger.warning("scorehud.yml default-board is empty; using built-in default lines.");
        board_.defaultLines = {
            "",
            "§dName: §5{scorehud.name}",
            "",
            "§bOnline: §a{scorehud.online} §c/ §d{scorehud.max_online}",
            "",
            "§dTPS: §5{scorehud.tps}",
            "§bLoad: §a{scorehud.load}",
        };
    }
}

}  // namespace scorehud
