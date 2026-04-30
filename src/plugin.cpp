#include "plugin.h"
#include "version.h"

#include <algorithm>

namespace {

constexpr auto kPrefix = "§8[§l§6S§eH§r§8]§r ";

std::string lower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) { return std::tolower(ch); });
    return text;
}

}  // namespace

void ScoreHudPlugin::onEnable()
{
    settings_.load(*this);
    tagResolver_ = std::make_unique<scorehud::TagResolver>(getServer());
    sessions_ = std::make_unique<scorehud::SessionManager>(*this, settings_, *tagResolver_);

    registerListeners();
    scheduleTasks();

    tagResolver_->refreshServerTags(settings_.runtime());
    for (auto *player : getServer().getOnlinePlayers()) {
        if (player != nullptr) {
            createSession(*player);
        }
    }

    getLogger().info("ScoreHud enabled.");
}

void ScoreHudPlugin::onDisable()
{
    getServer().getScheduler().cancelTasks(*this);
    if (sessions_ != nullptr) {
        sessions_->destroyAll();
    }

    titleUpdateTask_.reset();
    tagUpdateTask_.reset();
    hudUpdateTask_.reset();
    sessions_.reset();
    tagResolver_.reset();
    listener_.reset();

    getLogger().info("ScoreHud disabled.");
}

bool ScoreHudPlugin::onCommand(endstone::CommandSender &sender, const endstone::Command &command,
                               const std::vector<std::string> &args)
{
    if (command.getName() != "scorehud") {
        return false;
    }

    const auto action = args.empty() ? "help" : lower(args.front());
    if (action == "about") {
        sender.sendMessage("{}§6Score§eHud §av{}§a. Plugin by §dTwoTech§a. Endstone C++ port.", kPrefix,
                           getDescription().getVersion());
        return true;
    }

    if (action == "help") {
        sender.sendMessage("{}§cUsage: /scorehud <on|off|about|help>", kPrefix);
        return true;
    }

    auto *player = sender.asPlayer();
    if (player == nullptr) {
        sender.sendErrorMessage("{}§cYou can only use this command in-game.", kPrefix);
        return true;
    }

    handleScoreHudCommand(*player, action);
    return true;
}

void ScoreHudPlugin::createSession(endstone::Player &player)
{
    sessions_->create(player);
}

void ScoreHudPlugin::destroySession(endstone::Player &player)
{
    sessions_->destroy(player);
}

void ScoreHudPlugin::refreshPlayer(endstone::Player &player)
{
    sessions_->handle(player);
}

void ScoreHudPlugin::refreshAll(bool calledFromTask)
{
    sessions_->handleAll(calledFromTask);
}

void ScoreHudPlugin::setServerTag(const scorehud::ScoreTag &tag)
{
    tagResolver_->setServerTag(tag);
    refreshAll();
}

void ScoreHudPlugin::setPlayerTag(endstone::Player &player, const scorehud::ScoreTag &tag)
{
    tagResolver_->setPlayerTag(player, tag);
    refreshPlayer(player);
}

scorehud::TagResolver &ScoreHudPlugin::tagResolver()
{
    return *tagResolver_;
}

scorehud::SessionManager &ScoreHudPlugin::sessions()
{
    return *sessions_;
}

const scorehud::ScoreHudSettings &ScoreHudPlugin::settings() const
{
    return settings_;
}

void ScoreHudPlugin::scheduleTasks()
{
    hudUpdateTask_ = getServer().getScheduler().runTaskTimer(*this, [this]() { refreshAll(); }, 20, 20);

    if (settings_.runtime().tagFactoryEnabled) {
        const auto period = static_cast<std::uint64_t>(settings_.runtime().tagFactoryUpdateSeconds * 20);
        tagUpdateTask_ = getServer().getScheduler().runTaskTimer(
            *this,
            [this]() {
                tagResolver_->refreshServerTags(settings_.runtime());
                for (auto *player : getServer().getOnlinePlayers()) {
                    if (player != nullptr) {
                        tagResolver_->refreshPlayerTags(*player);
                    }
                }
                refreshAll();
            },
            period, period);
    }

    if (settings_.board().flickerTitles && settings_.board().titles.size() > 1) {
        const auto period = static_cast<std::uint64_t>(settings_.board().titlePeriodSeconds * 20);
        titleUpdateTask_ =
            getServer().getScheduler().runTaskTimer(*this, [this]() { refreshAll(true); }, period, period);
    }
}

void ScoreHudPlugin::registerListeners()
{
    listener_ = std::make_unique<ScoreHudListener>(*this);
    registerEvent(&ScoreHudListener::onActorDamage, *listener_);
    registerEvent(&ScoreHudListener::onPlayerDimensionChange, *listener_);
    registerEvent(&ScoreHudListener::onPlayerItemHeld, *listener_);
    registerEvent(&ScoreHudListener::onPlayerJoin, *listener_, endstone::EventPriority::Low);
    registerEvent(&ScoreHudListener::onPlayerMove, *listener_);
    registerEvent(&ScoreHudListener::onPlayerQuit, *listener_, endstone::EventPriority::Monitor);
    registerEvent(&ScoreHudListener::onPlayerTeleport, *listener_);
}

void ScoreHudPlugin::handleScoreHudCommand(endstone::Player &player, const std::string &action)
{
    if (action == "on") {
        if (!sessions_->isDisabled(player)) {
            player.sendMessage("{}§cScoreHud is already enabled for you.", kPrefix);
            return;
        }

        sessions_->enable(player);
        player.sendMessage("{}§aSuccessfully enabled ScoreHud.", kPrefix);
        return;
    }

    if (action == "off") {
        if (sessions_->isDisabled(player)) {
            player.sendMessage("{}§cScoreHud is already disabled for you.", kPrefix);
            return;
        }

        sessions_->disable(player);
        player.sendMessage("{}§aSuccessfully disabled ScoreHud.", kPrefix);
        return;
    }

    player.sendMessage("{}§cUsage: /scorehud <on|off|about|help>", kPrefix);
}

ENDSTONE_PLUGIN("scorehud", SCOREHUD_VERSION, ScoreHudPlugin)
{
    prefix = "ScoreHud";
    description = "Add fully customizable scoreboards to your server.";
    website = "https://github.com/two-tech-dev/ScoreHud/";
    authors = {"TwoTech"};

    command("scorehud")
        .description("Shows ScoreHud commands")
        .usages("/scorehud [action: str]")
        .aliases("sh")
        .permissions("sh.command.sh");

    permission("sh.command.sh")
        .description("Allow players to use /scorehud command.")
        .default_(endstone::PermissionDefault::True);
}
