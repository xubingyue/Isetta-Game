/*
 * Copyright (c) 2018 Isetta
 */
#pragma once
#include "Core/Math/Vector2.h"
#include "ISETTA_API.h"
#include "Scene/Component.h"

namespace Isetta {
DEFINE_COMPONENT(GridComponent, Component, false)
public:
#ifdef _EDITOR
void Update() override;
#endif
DEFINE_COMPONENT_END(GridComponent, Component)
}  // namespace Isetta
