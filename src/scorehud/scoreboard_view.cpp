#include "scorehud/scoreboard_view.h"

#include <algorithm>

namespace scorehud {
namespace {

constexpr auto kObjectiveName = "scorehud";
constexpr int kMaxScoreboardLines = 15;

std::string uniqueLine(std::string line, const std::vector<std::string> &usedLines)
{
    if (line.empty()) {
        line = " ";
    }

    while (std::find(usedLines.begin(), usedLines.end(), line) != usedLines.end()) {
        line += " ";
    }
    return line;
}

}  // namespace

ScoreboardView::ScoreboardView(std::shared_ptr<endstone::Scoreboard> scoreboard) : scoreboard_(std::move(scoreboard))
{
}

void ScoreboardView::display(endstone::Player &player, const std::string &title, const std::vector<std::string> &lines,
                             TagResolver &tagResolver, const RuntimeConfig &config)
{
    auto objective = scoreboard_->getObjective(kObjectiveName);
    if (objective == nullptr) {
        objective = scoreboard_->addObjective(kObjectiveName, endstone::Criteria::Type::Dummy, title);
        objective->setDisplay(endstone::DisplaySlot::SideBar, endstone::ObjectiveSortOrder::Descending);
    }
    else {
        objective->setDisplayName(title);
    }

    for (const auto &entry : scoreboard_->getEntries()) {
        scoreboard_->resetScores(entry);
    }

    std::vector<std::string> renderedLines;
    renderedLines.reserve(std::min<int>(lines.size(), kMaxScoreboardLines));
    for (const auto &line : lines) {
        if (renderedLines.size() >= kMaxScoreboardLines) {
            break;
        }

        auto renderedLine = tagResolver.resolve(line, player, config);
        renderedLines.push_back(uniqueLine(std::move(renderedLine), renderedLines));
    }

    auto score = static_cast<int>(renderedLines.size());
    for (const auto &line : renderedLines) {
        objective->getScore(std::string{" "} + line + " ")->setValue(score--);
    }
}

void ScoreboardView::clear()
{
    scoreboard_->clearSlot(endstone::DisplaySlot::SideBar);
}

}  // namespace scorehud
