/*
 * Copyright (c) 2018 Isetta
 */
#include "Gameplay/GameManager.h"
#include <Core/IsettaCore.h>

using namespace Isetta;

GameManager& GameManager::Instance() {
  return *instance;
}

void GameManager::Awake() {
  instance = this;
}

void GameManager::Update() {
}