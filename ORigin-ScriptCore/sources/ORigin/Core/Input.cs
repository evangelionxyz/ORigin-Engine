// Copyright (c) Evangelion Manuhutu | ORigin Engine

namespace ORiginEngine
{
  public static class Input
  {
    public static bool IsKeyPressed(KeyCode keycode)
    {
      return InternalCalls.Input_IsKeyPressed(keycode);
    }

    public static bool IsKeyReleased(KeyCode keyCode)
    {
      return InternalCalls.Input_IsKeyReleased(keyCode);
    }
  }
}
