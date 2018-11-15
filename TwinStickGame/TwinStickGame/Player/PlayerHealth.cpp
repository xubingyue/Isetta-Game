/*
 * Copyright (c) 2018 Isetta
 */
#include "PlayerHealth.h"

// ENGINE
#include "Core/Color.h"
#include "Graphics/GUI.h"
#include "Graphics/RectTransform.h"
#include "Scene/Entity.h"

#include "Input/Input.h"  // TODO(Jacob) remove

// GAME
#include "Gameplay/Damageable.h"

using namespace Isetta;

void PlayerHealth::Start() {
  health = entity->GetComponent<Damageable>();
  if (!health) {
    SetActive(false);
    return;
  }
  // TODO(Jacob) remove
  Input::RegisterKeyPressCallback(KeyCode::H, [&]() { health->DealDamage(1); });
}

void PlayerHealth::GuiUpdate() {
  const float scale = 3.f;
  RectTransform rect{{30, 45, 0, 0}};
  rect.pivot = GUI::Pivot::Left;
  GUI::Text(rect, "Health:", GUI::TextStyle{50.f, "Neon"});
  rect.pivot = GUI::Pivot::Left;
  rect.rect.x += 150.f;
  rect.rect.width = scale * health->GetMaxHealth();
  rect.rect.height = 40.f;
  GUI::Draw::RectFilled(rect, Color{187.f / 255, 187.f / 255, 187.f / 255});
  rect.rect.width = scale * health->GetHealth();
  GUI::Draw::RectFilled(rect, Color{109.f / 255, 205.f / 255, 116.f / 255});
}