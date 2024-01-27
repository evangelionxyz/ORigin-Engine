// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

using System;

namespace ORiginEngine
{
  public class Entity
  {
    protected Entity() { ID = 0; }

    internal Entity(ulong id)
    {
      ID = id;
    }

    public readonly ulong ID;

    public Vector3 Translation
    {
      get
      {
        InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 translation);
        return translation;
      }
      set
      {
        InternalCalls.TransformComponent_SetTranslation(ID, ref value);
      }
    }

    public Vector3 Rotation
    {
      get
      {
        InternalCalls.TransformComponent_GetRotation(ID, out Vector3 rotation);
        return rotation;
      }
      set
      {
        InternalCalls.TransformComponent_SetRotation(ID, ref value);
      }
    }

    public Vector3 Scale
    {
      get
      {
        InternalCalls.TransformComponent_GetScale(ID, out Vector3 scale);
        return scale;
      }
      set
      {
        InternalCalls.TransformComponent_SetScale(ID, ref value);
      }
    }

    public T AddComponent<T>() where T : Component, new()
    {
      if (HasComponent<T>())
        return GetComponent<T>();

      T component = new T() { Entity = this };
      Type componentType = typeof(T);
      InternalCalls.Entity_AddComponent(ID, componentType);
      return component;
    }

    public bool HasComponent<T>() where T : Component, new()
    {
      Type componentType = typeof(T);
      return InternalCalls.Entity_HasComponent(ID, componentType);
    }

    public T GetComponent<T>() where T : Component, new()
    {
      if (!HasComponent<T>())
        return null;

      T component = new T() { Entity = this };
      return component;
    }

    public Entity FindEntityByName(string name)
    {
      ulong entityID = InternalCalls.Entity_FindEntityByName(name);
      if (entityID == 0)
        return null;

      return new Entity(entityID);
    }

    public T As<T>() where T : Entity, new()
    {
      object instance = InternalCalls.GetScriptInstance(ID);
      return instance as T;
    }
  }
}