#include "scorehud/session_manager.h"

#include <utility>

namespace scorehud {

SessionManager::SessionManager(endstone::Plugin &plugin, ScoreHudSettings &settings, TagResolver &tagResolver)
    : plugin_(plugin), settings_(settings), tagResolver_(tagResolver)
{
}

void SessionManager::create(endstone::Player &player)
{
    const auto key = playerKey(player);
    if (!sessions_.contains(key)) {
        sessions_.emplace(key, PlayerSession{});
    }
    handle(player);
}

void SessionManager::destroy(endstone::Player &player)
{
    const auto key = playerKey(player);
    if (const auto session = sessions_.find(key); session != sessions_.end()) {
        if (session->second.view != nullptr) {
            session->second.view->clear();
        }
        sessions_.erase(session);
    }
    tagResolver_.removePlayer(player);
}

void SessionManager::destroyAll()
{
    for (auto *player : plugin_.getServer().getOnlinePlayers()) {
        if (player != nullptr) {
            destroy(*player);
            restoreServerScoreboard(*player);
        }
    }
    sessions_.clear();
}

void SessionManager::handle(endstone::Player &player, bool calledFromTask)
{
    if (isDisabled(player)) {
        restoreServerScoreboard(player);
        return;
    }

    auto &session = sessions_[playerKey(player)];
    if (session.view == nullptr) {
        auto scoreboard = plugin_.getServer().createScoreboard();
        player.setScoreboard(*scoreboard);
        session.view = std::make_unique<ScoreboardView>(std::move(scoreboard));
    }

    tagResolver_.refreshPlayerTags(player);
    session.view->display(player, currentTitle(calledFromTask), settings_.defaultLines(), tagResolver_,
                          settings_.runtime());
}

void SessionManager::handleAll(bool calledFromTask)
{
    for (auto *player : plugin_.getServer().getOnlinePlayers()) {
        if (player != nullptr) {
            handle(*player, calledFromTask);
        }
    }
}

void SessionManager::disable(endstone::Player &player)
{
    disabledPlayers_.insert(playerKey(player));
    if (const auto session = sessions_.find(playerKey(player)); session != sessions_.end() &&
                                                      session->second.view != nullptr) {
        session->second.view->clear();
    }
    restoreServerScoreboard(player);
}

void SessionManager::enable(endstone::Player &player)
{
    disabledPlayers_.erase(playerKey(player));
    handle(player);
}

bool SessionManager::isDisabled(endstone::Player &player) const
{
    return disabledPlayers_.contains(playerKey(player));
}

std::string SessionManager::currentTitle(bool calledFromTask)
{
    const auto &board = settings_.board();
    if (!board.flickerTitles || board.titles.empty()) {
        return board.title;
    }

    if (titleIndex_ >= board.titles.size()) {
        titleIndex_ = 0;
    }

    const auto title = board.titles[titleIndex_];
    if (calledFromTask) {
        titleIndex_ = (titleIndex_ + 1) % board.titles.size();
    }
    return title;
}

std::string SessionManager::playerKey(endstone::Player &player) const
{
    const auto xuid = player.getXuid();
    return xuid.empty() ? player.getName() : xuid;
}

void SessionManager::restoreServerScoreboard(endstone::Player &player)
{
    if (auto *scoreboard = plugin_.getServer().getScoreboard(); scoreboard != nullptr) {
        player.setScoreboard(*scoreboard);
    }
}

}  // namespace scorehud
