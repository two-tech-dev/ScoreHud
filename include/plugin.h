#pragma once

#include "listener.h"
#include "scorehud/session_manager.h"
#include "scorehud/settings.h"
#include "scorehud/tag_resolver.h"

#include <endstone/endstone.hpp>

#include <memory>
#include <string>
#include <vector>

class ScoreHudPlugin : public endstone::Plugin {
public:
    void onEnable() override;
    void onDisable() override;

    bool onCommand(endstone::CommandSender &sender, const endstone::Command &command,
                   const std::vector<std::string> &args) override;

    void createSession(endstone::Player &player);
    void destroySession(endstone::Player &player);
    void refreshPlayer(endstone::Player &player);
    void refreshAll(bool calledFromTask = false);
    void setServerTag(const scorehud::ScoreTag &tag);
    void setPlayerTag(endstone::Player &player, const scorehud::ScoreTag &tag);

    [[nodiscard]] scorehud::TagResolver &tagResolver();
    [[nodiscard]] scorehud::SessionManager &sessions();
    [[nodiscard]] const scorehud::ScoreHudSettings &settings() const;

private:
    void scheduleTasks();
    void registerListeners();
    void handleScoreHudCommand(endstone::Player &player, const std::string &action);

    std::unique_ptr<ScoreHudListener> listener_;
    std::unique_ptr<scorehud::TagResolver> tagResolver_;
    std::unique_ptr<scorehud::SessionManager> sessions_;
    scorehud::ScoreHudSettings settings_;
    std::shared_ptr<endstone::Task> hudUpdateTask_;
    std::shared_ptr<endstone::Task> tagUpdateTask_;
    std::shared_ptr<endstone::Task> titleUpdateTask_;
};
