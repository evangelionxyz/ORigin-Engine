// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

namespace ORiginEngine
{
    public static class Input
    {
        public static bool IsKeyPressed(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyPressed(keycode);
        }
    }
}
