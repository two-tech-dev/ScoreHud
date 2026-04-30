#include "listener.h"

#include "plugin.h"
#include "scorehud/score_tag.h"

void ScoreHudListener::onActorDamage(endstone::ActorDamageEvent &event)
{
    auto *player = dynamic_cast<endstone::Player *>(&event.getActor());
    if (player == nullptr) {
        return;
    }

    plugin_.setPlayerTag(*player, {"scorehud.health", std::to_string(player->getHealth())});
    plugin_.setPlayerTag(*player, {"scorehud.max_health", std::to_string(player->getMaxHealth())});
}

void ScoreHudListener::onPlayerDimensionChange(endstone::PlayerDimensionChangeEvent &event)
{
    plugin_.refreshPlayer(event.getPlayer());
}

void ScoreHudListener::onPlayerItemHeld(endstone::PlayerItemHeldEvent &event)
{
    auto &player = event.getPlayer();
    plugin_.tagResolver().refreshPlayerTags(player);
    plugin_.refreshPlayer(player);
}

void ScoreHudListener::onPlayerJoin(endstone::PlayerJoinEvent &event)
{
    plugin_.tagResolver().refreshServerTags(plugin_.settings().runtime());
    plugin_.createSession(event.getPlayer());
    plugin_.refreshAll();
}

void ScoreHudListener::onPlayerMove(endstone::PlayerMoveEvent &event)
{
    const auto &from = event.getFrom();
    const auto &to = event.getTo();
    if (from.getBlockX() == to.getBlockX() && from.getBlockY() == to.getBlockY() && from.getBlockZ() == to.getBlockZ()) {
        return;
    }

    auto &player = event.getPlayer();
    plugin_.setPlayerTag(player, {"scorehud.x", std::to_string(to.getBlockX())});
    plugin_.setPlayerTag(player, {"scorehud.y", std::to_string(to.getBlockY())});
    plugin_.setPlayerTag(player, {"scorehud.z", std::to_string(to.getBlockZ())});
}

void ScoreHudListener::onPlayerQuit(endstone::PlayerQuitEvent &event)
{
    plugin_.destroySession(event.getPlayer());
    plugin_.tagResolver().refreshServerTags(plugin_.settings().runtime());
    plugin_.refreshAll();
}

void ScoreHudListener::onPlayerTeleport(endstone::PlayerTeleportEvent &event)
{
    const auto &from = event.getFrom();
    const auto &to = event.getTo();
    auto &player = event.getPlayer();

    if (&from.getDimension() != &to.getDimension()) {
        plugin_.refreshPlayer(player);
        plugin_.refreshAll();
        return;
    }

    plugin_.setPlayerTag(player, {"scorehud.x", std::to_string(to.getBlockX())});
    plugin_.setPlayerTag(player, {"scorehud.y", std::to_string(to.getBlockY())});
    plugin_.setPlayerTag(player, {"scorehud.z", std::to_string(to.getBlockZ())});
}
