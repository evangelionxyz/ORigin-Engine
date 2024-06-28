// Copyright (c) Evangelion Manuhutu | ORigin Engine

namespace ORiginEngine
{
  public static class Input
  {
    public static bool IsKeyPressed(KeyCode code)
    {
      return InternalCalls.Input_IsKeyPressed(code);
    }

    public static bool IsKeyReleased(KeyCode code)
    {
      return InternalCalls.Input_IsKeyReleased(code);
    }

    public static bool IsMouseDown(MouseCode code)
    {
      return InternalCalls.Input_IsMouseDown(code);
    }

    public static bool IsMouseUp(MouseCode code)
    {
      return InternalCalls.Input_IsMouseUp(code);
    }
  }
}
