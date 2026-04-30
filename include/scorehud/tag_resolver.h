#pragma once

#include "scorehud/score_tag.h"
#include "scorehud/settings.h"

#include <endstone/endstone.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace scorehud {

class TagResolver {
public:
    explicit TagResolver(endstone::Server &server);

    void setServerTag(const ScoreTag &tag);
    void setPlayerTag(endstone::Player &player, const ScoreTag &tag);
    void removePlayer(endstone::Player &player);

    [[nodiscard]] std::string resolve(const std::string &line, endstone::Player &player, const RuntimeConfig &config);
    [[nodiscard]] static std::vector<std::string> extractTags(const std::vector<std::string> &lines);

    void refreshServerTags(const RuntimeConfig &config);
    void refreshPlayerTags(endstone::Player &player);

private:
    [[nodiscard]] std::string playerKey(endstone::Player &player) const;
    [[nodiscard]] std::string builtInTag(const std::string &tagName, endstone::Player &player,
                                         const RuntimeConfig &config);

    endstone::Server &server_;
    std::unordered_map<std::string, std::string> serverTags_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> playerTags_;
};

}  // namespace scorehud
