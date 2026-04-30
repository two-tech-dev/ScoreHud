#pragma once

#include "scorehud/settings.h"
#include "scorehud/tag_resolver.h"

#include <endstone/endstone.hpp>

#include <memory>
#include <string>
#include <vector>

namespace scorehud {

class ScoreboardView {
public:
    explicit ScoreboardView(std::shared_ptr<endstone::Scoreboard> scoreboard);

    void display(endstone::Player &player, const std::string &title, const std::vector<std::string> &lines,
                 TagResolver &tagResolver, const RuntimeConfig &config);
    void clear();

private:
    std::shared_ptr<endstone::Scoreboard> scoreboard_;
};

}  // namespace scorehud
