#pragma once

#include "scorehud/scoreboard_view.h"
#include "scorehud/settings.h"
#include "scorehud/tag_resolver.h"

#include <endstone/endstone.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace scorehud {

class SessionManager {
public:
    SessionManager(endstone::Plugin &plugin, ScoreHudSettings &settings, TagResolver &tagResolver);

    void create(endstone::Player &player);
    void destroy(endstone::Player &player);
    void destroyAll();

    void handle(endstone::Player &player, bool calledFromTask = false);
    void handleAll(bool calledFromTask = false);
    void disable(endstone::Player &player);
    void enable(endstone::Player &player);

    [[nodiscard]] bool isDisabled(endstone::Player &player) const;
    [[nodiscard]] std::string currentTitle(bool calledFromTask);

private:
    struct PlayerSession {
        std::unique_ptr<ScoreboardView> view;
    };

    [[nodiscard]] std::string playerKey(endstone::Player &player) const;
    void restoreServerScoreboard(endstone::Player &player);

    endstone::Plugin &plugin_;
    ScoreHudSettings &settings_;
    TagResolver &tagResolver_;
    std::unordered_map<std::string, PlayerSession> sessions_;
    std::unordered_set<std::string> disabledPlayers_;
    std::size_t titleIndex_{0};
};

}  // namespace scorehud
