#pragma once

#include <endstone/endstone.hpp>

class ScoreHudPlugin;

class ScoreHudListener {
public:
    explicit ScoreHudListener(ScoreHudPlugin &plugin) : plugin_(plugin) {}

    void onActorDamage(endstone::ActorDamageEvent &event);
    void onPlayerDimensionChange(endstone::PlayerDimensionChangeEvent &event);
    void onPlayerItemHeld(endstone::PlayerItemHeldEvent &event);
    void onPlayerJoin(endstone::PlayerJoinEvent &event);
    void onPlayerMove(endstone::PlayerMoveEvent &event);
    void onPlayerQuit(endstone::PlayerQuitEvent &event);
    void onPlayerTeleport(endstone::PlayerTeleportEvent &event);

private:
    ScoreHudPlugin &plugin_;
};
