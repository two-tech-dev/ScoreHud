#include "scorehud/tag_resolver.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>
#include <unordered_set>

namespace scorehud {
namespace {

std::string formatFloat(float number, int precision = 2)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << number;
    return stream.str();
}

void replaceAll(std::string &text, const std::string &from, const std::string &to)
{
    std::size_t position = 0;
    while ((position = text.find(from, position)) != std::string::npos) {
        text.replace(position, from.size(), to);
        position += to.size();
    }
}

std::string formatDateTime(const char *format)
{
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, format);
    return stream.str();
}

std::string phpDateFormatToStrftime(std::string format)
{
    replaceAll(format, "H", "%H");
    replaceAll(format, "i", "%M");
    replaceAll(format, "s", "%S");
    replaceAll(format, "d", "%d");
    replaceAll(format, "m", "%m");
    replaceAll(format, "Y", "%Y");
    return format;
}

std::string gameModeName(endstone::GameMode mode)
{
    switch (mode) {
    case endstone::GameMode::Survival:
        return "Survival";
    case endstone::GameMode::Creative:
        return "Creative";
    case endstone::GameMode::Adventure:
        return "Adventure";
    case endstone::GameMode::Spectator:
        return "Spectator";
    }
    return "Unknown";
}

std::string heldItemName(endstone::Player &player)
{
    const auto item = player.getInventory().getItemInMainHand();
    if (!item.has_value()) {
        return "Air";
    }
    return fmt::format("{}", item->getType().getId());
}

std::string heldItemCount(endstone::Player &player)
{
    const auto item = player.getInventory().getItemInMainHand();
    return item.has_value() ? std::to_string(item->getAmount()) : "0";
}

int playersInWorld(endstone::Server &server, const std::string &worldName)
{
    auto playerCount = 0;
    for (auto *player : server.getOnlinePlayers()) {
        if (player != nullptr && player->getLevel().getName() == worldName) {
            ++playerCount;
        }
    }
    return playerCount;
}

std::string readProcStatusValue(const std::string &field)
{
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.starts_with(field + ":")) {
            return line.substr(field.size() + 1);
        }
    }
    return "N/A";
}

}  // namespace

TagResolver::TagResolver(endstone::Server &server) : server_(server) {}

void TagResolver::setServerTag(const ScoreTag &tag)
{
    serverTags_[tag.name()] = tag.content();
}

void TagResolver::setPlayerTag(endstone::Player &player, const ScoreTag &tag)
{
    playerTags_[playerKey(player)][tag.name()] = tag.content();
}

void TagResolver::removePlayer(endstone::Player &player)
{
    playerTags_.erase(playerKey(player));
}

std::string TagResolver::resolve(const std::string &line, endstone::Player &player, const RuntimeConfig &config)
{
    auto resolvedLine = line;
    for (const auto &tagName : extractTags({line})) {
        auto content = std::string{};

        if (const auto playerTags = playerTags_.find(playerKey(player));
            playerTags != playerTags_.end() && playerTags->second.contains(tagName)) {
            content = playerTags->second.at(tagName);
        }
        else if (serverTags_.contains(tagName)) {
            content = serverTags_.at(tagName);
        }
        else {
            content = builtInTag(tagName, player, config);
        }

        replaceAll(resolvedLine, "{" + tagName + "}", content);
    }
    return resolvedLine;
}

std::vector<std::string> TagResolver::extractTags(const std::vector<std::string> &lines)
{
    static const std::regex tagPattern(R"(\{(([A-Za-z0-9_\-]{2,})(\.[A-Za-z0-9_\-]+)+)\})");

    std::vector<std::string> tagNames;
    std::unordered_set<std::string> seen;
    for (const auto &line : lines) {
        for (auto match = std::sregex_iterator(line.begin(), line.end(), tagPattern); match != std::sregex_iterator();
             ++match) {
            const auto tagName = (*match)[1].str();
            if (seen.insert(tagName).second) {
                tagNames.push_back(tagName);
            }
        }
    }
    return tagNames;
}

void TagResolver::refreshServerTags(const RuntimeConfig &config)
{
    setServerTag({"scorehud.online", std::to_string(server_.getOnlinePlayers().size())});
    setServerTag({"scorehud.max_online", std::to_string(server_.getMaxPlayers())});
    setServerTag({"scorehud.load", formatFloat(server_.getCurrentTickUsage())});
    setServerTag({"scorehud.tps", formatFloat(server_.getCurrentTicksPerSecond())});
    setServerTag({"scorehud.mspt", formatFloat(server_.getCurrentMillisecondsPerTick())});
    setServerTag({"scorehud.time", formatDateTime(phpDateFormatToStrftime(config.timeFormat).c_str())});
    setServerTag({"scorehud.date", formatDateTime(phpDateFormatToStrftime(config.dateFormat).c_str())});

    if (config.memoryTagsEnabled) {
        setServerTag({"scorehud.memory_main_thread", readProcStatusValue("VmRSS")});
        setServerTag({"scorehud.memory_total", readProcStatusValue("VmHWM")});
        setServerTag({"scorehud.memory_virtual", readProcStatusValue("VmSize")});
        setServerTag({"scorehud.memory_heap", readProcStatusValue("VmData")});
        setServerTag({"scorehud.memory_global", "MAX"});
    }
}

void TagResolver::refreshPlayerTags(endstone::Player &player)
{
    const auto location = player.getLocation();
    const auto worldName = player.getLevel().getName();

    setPlayerTag(player, {"scorehud.name", player.getName()});
    setPlayerTag(player, {"scorehud.real_name", player.getName()});
    setPlayerTag(player, {"scorehud.display_name", player.getName()});
    setPlayerTag(player, {"scorehud.item_name", heldItemName(player)});
    setPlayerTag(player, {"scorehud.item_count", heldItemCount(player)});
    setPlayerTag(player, {"scorehud.x", std::to_string(location.getBlockX())});
    setPlayerTag(player, {"scorehud.y", std::to_string(location.getBlockY())});
    setPlayerTag(player, {"scorehud.z", std::to_string(location.getBlockZ())});
    setPlayerTag(player, {"scorehud.level_name", worldName});
    setPlayerTag(player, {"scorehud.world_name", worldName});
    setPlayerTag(player, {"scorehud.level_folder_name", worldName});
    setPlayerTag(player, {"scorehud.world_folder_name", worldName});
    setPlayerTag(player, {"scorehud.world_player_count", std::to_string(playersInWorld(server_, worldName))});
    setPlayerTag(player, {"scorehud.ip", player.getAddress().getHostname()});
    setPlayerTag(player, {"scorehud.ping", std::to_string(player.getPing().count())});
    setPlayerTag(player, {"scorehud.health", std::to_string(player.getHealth())});
    setPlayerTag(player, {"scorehud.max_health", std::to_string(player.getMaxHealth())});
    setPlayerTag(player, {"scorehud.gamemode", gameModeName(player.getGameMode())});
}

std::string TagResolver::playerKey(endstone::Player &player) const
{
    const auto xuid = player.getXuid();
    return xuid.empty() ? player.getName() : xuid;
}

std::string TagResolver::builtInTag(const std::string &tagName, endstone::Player &player, const RuntimeConfig &config)
{
    if (tagName == "scorehud.time") {
        return formatDateTime(phpDateFormatToStrftime(config.timeFormat).c_str());
    }
    if (tagName == "scorehud.date") {
        return formatDateTime(phpDateFormatToStrftime(config.dateFormat).c_str());
    }

    refreshPlayerTags(player);
    const auto playerTags = playerTags_.find(playerKey(player));
    if (playerTags != playerTags_.end() && playerTags->second.contains(tagName)) {
        return playerTags->second.at(tagName);
    }
    if (serverTags_.contains(tagName)) {
        return serverTags_.at(tagName);
    }
    return "";
}

}  // namespace scorehud
