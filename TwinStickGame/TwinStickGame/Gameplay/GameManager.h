/*
 * Copyright (c) 2018 Isetta
 */
#pragma once
#include <IsettaEngine.h>
#include "Player/PlayerController.h"

using namespace Isetta;

class GameManager {
 public:
  struct GameConfig {
    CVarString playerName{"player_name", "Unknown"};
  };

  static GameConfig gameConfig;

  static GameManager& Instance();
  GameManager();

  // Player stuff
  void SendSpawnPlayerMessage();        // from client
  void RegisterSpawnPlayerCallbacks();  // on both client and server
  Math::Vector3 GetPlayerStartPos();
  Array<PlayerController*> players;

  // Level loading stuff
  void LoadLevel(std::string_view levelName);
  void SendLevelLoadedMessage();             // from client
  void RegisterClientLevelLoadedCallback();  // on server
  void RegisterAllPlayerReadyCallback();     // on client
  void RegisterClientConnectionCallbacks();  // on server
  U8 playerCount = 0; // server only
  U8 levelLoadCompletePlayerCount = 0; // server only
};