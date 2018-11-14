/*
 * Copyright (c) 2018 Isetta
 */
#pragma once
#include <bitset>
#include <execution>
#include <typeindex>
#include <typeinfo>
#include "Component.h"
#include "Core/DataStructures/Array.h"
#include "Core/Memory/MemoryManager.h"
#include "Scene/Transform.h"
#include "Util.h"

namespace Isetta {
class ISETTA_API_DECLARE Entity {
 private:
  enum class EntityAttributes { IS_ACTIVE, NEED_DESTROY, IS_TRANSFORM_DIRTY };

  friend class RenderModule;
  friend class Level;

  std::vector<std::type_index> componentTypes;
  Array<class Component*> components;
  Transform transform;

  void OnEnable();
  void GuiUpdate();
  void Update();
  void FixedUpdate();
  void LateUpdate();
  void CheckDestroy();
  void OnDisable();

  std::bitset<3> attributes;

  StringId entityID;
  std::string entityName;
  int layer{0};

  void SetAttribute(EntityAttributes attr, bool value);
  bool GetAttribute(EntityAttributes attr) const;

 public:
  Entity(const std::string& name);
  ~Entity();

  std::string GetName() const { return entityName; }
  static void Destroy(Entity* entity);
  static void DestroyHelper(Entity* entity);
  static void DestroyImmediately(Entity* entity);
  static Entity* GetEntityByName(const std::string& name);
  static std::list<Entity*> GetEntitiesByName(const std::string& name);

  void SetActive(bool inActive);
  bool GetActive() const;

  template <typename T, typename... Args>
  T* AddComponent(Args&&... args);
  template <typename T, bool IsActive, typename... Args>
  T* AddComponent(Args&&... args);
  template <typename T>
  T* GetComponent();
  template <typename T>
  Array<T*> GetComponents();
  template <typename T>
  T* GetComponentInParent();
  template <typename T>
  Array<T*> GetComponentsInParent();
  template <typename T>
  T* GetComponentInChildren();
  template <typename T>
  Array<T*> GetComponentsInChildren();
  template <typename T>
  T* GetComponentInDescendant();
  template <typename T>
  Array<T*> GetComponentsInDescendant();

  void SetTransform(const Math::Vector3& worldPos = Math::Vector3::zero,
                    const Math::Vector3& worldEulerAngles = Math::Vector3::zero,
                    const Math::Vector3& localScale = Math::Vector3::one);
  Transform* const GetTransform() { return &transform; }
  //#if _DEBUG
  // TODO(YIDI): Delete this! This is used for in game editor
  // TODO(Jacob) no don't this is good
  Array<class Component*> GetComponents() const { return components; }
  // TODO(Chaojie): You can use GetComponents<Component> now
  //#endif

  void SetLayer(int layer);
  void SetLayer(std::string layer);
  int GetLayerIndex() const;
  std::string GetLayerName() const;
};

template <typename T, typename... Args>
T* Entity::AddComponent(Args&&... args) {
  T* component = AddComponent<T, true>(std::forward<Args>(args)...);
  return component;
}

template <typename T, bool IsActive, typename... Args>
T* Entity::AddComponent(Args&&... args) {
  if constexpr (!std::is_base_of<class Component, T>::value) {
    throw std::logic_error(
        Util::StrFormat("Entity::AddComponent => %s is not a derived class "
                        "from Component class",
                        typeid(T).name));
  } else {
    std::type_index typeIndex{typeid(T)};
    if (std::any_of(
            std::execution::par, Component::excludeComponents().begin(),
            Component::excludeComponents().end(),
            [typeIndex](std::type_index type) { return type == typeIndex; }) &&
        std::any_of(
            std::execution::par, componentTypes.begin(), componentTypes.end(),
            [typeIndex](std::type_index type) { return type == typeIndex; })) {
      throw std::logic_error(Util::StrFormat(
          "Entity::AddComponent => Adding multiple excluded components %s",
          typeIndex.name()));
    }
    T* component = MemoryManager::NewOnFreeList<T>(std::forward<Args>(args)...);
    component->SetActive(IsActive);
    component->entity = this;
    if (IsActive) {
      component->Awake();
      component->SetAttribute(Component::ComponentAttributes::HAS_AWAKEN, true);
      component->OnEnable();
    }
    componentTypes.emplace_back(typeIndex);
    components.EmplaceBack(component);

    LevelManager::Instance().currentLevel->AddComponentToStart(component);
    return component;
  }
}

template <typename T>
T* Entity::GetComponent() {
  auto types = Component::childrenTypes();
  std::list<std::type_index> availableTypes =
      types.at(std::type_index(typeid(T)));
  for (int i = 0; i < componentTypes.size(); i++) {
    std::type_index componentType = componentTypes[i];
    if (std::any_of(std::execution::par, availableTypes.begin(),
                    availableTypes.end(), [componentType](std::type_index x) {
                      return x == componentType;
                    })) {
      return static_cast<T*>(components[i]);
    }
  }
  return nullptr;
}

template <typename T>
Array<T*> Entity::GetComponents() {
  std::list<std::type_index> availableTypes =
      Component::childrenTypes().at(std::type_index(typeid(T)));
  Array<T*> returnValue;
  returnValue.Reserve(componentTypes.size());
  for (int i = 0; i < componentTypes.size(); i++) {
    std::type_index componentType = componentTypes[i];
    if (std::any_of(std::execution::par, availableTypes.begin(),
                    availableTypes.end(), [componentType](std::type_index x) {
                      return x == componentType;
                    })) {
      returnValue.EmplaceBack(static_cast<T*>(components[i]));
    }
  }
  return returnValue;
}
template <typename T>
inline T* Entity::GetComponentInParent() {
  return transform.parent->entity->GetComponent<T>();
}
template <typename T>
inline Array<T*> Entity::GetComponentsInParent() {
  return transform.parent->entity->GetComponents<T>();
}
template <typename T>
inline T* Entity::GetComponentInChildren() {
  T* component = nullptr;
  for (auto it = transform.begin(); it != transform.end() && !component; it++) {
    // TODO Calling getcomponent on iterator could break
    component = it->GetComponent<T>();
  }
  return component;
}
template <typename T>
inline Array<T*> Entity::GetComponentsInChildren() {
  Array<T*> components;
  for (auto it = transform.begin(); it != transform.end(); it++) {
    // TODO Calling getcomponent on iterator could break
    Array<T*> c;
    c = it->GetComponents<T>();
    components.insert(components.end(), c.begin(), c.end());
  }
  return components;
}
template <typename T>
inline T* Entity::GetComponentInDescendant() {
  T* component = nullptr;
  for (auto it = transform.begin(); it != transform.end() && !component; it++) {
    // TODO Calling getcomponent on iterator could break
    component = it->GetComponent<T>();
    if (!component) component = it->GetComponentInDescendant<T>();
  }
  return component;
}
template <typename T>
inline Array<T*> Entity::GetComponentsInDescendant() {
  Array<T*> components;
  for (auto it = transform.begin(); it != transform.end(); it++) {
    // TODO Calling getcomponent on iterator could break
    Array<T*> c;
    c = it->GetComponents<T>();
    components.insert(components.end(), c.begin(), c.end());
    c = it->GetComponentsInDescendant<T>();
    components.insert(components.end(), c.begin(), c.end());
  }
  return components;
}
}  // namespace Isetta