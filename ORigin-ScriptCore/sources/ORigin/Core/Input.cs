// Copyright (c) Evangelion Manuhutu | ORigin Engine

using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;

namespace ORiginEngine
{
    public static class Input
    {
        public static Vector2 MousePosition
        {
            get
            {
                InternalCalls.Input_GetMousePosition(out Vector2 result);
                return result;
            }
            set
            {
                InternalCalls.Input_SetMousePosition(value);
            }
        }

        public static Vector2 MouseDelta
        {
            get
            {
                InternalCalls.Input_GetMouseDelta(out Vector2 result);
                return result;
            }
        }

        public static bool IsMouseDown(MouseCode code)
        {
            return InternalCalls.Input_IsMouseButtonDown(code);
        }

        public static bool IsKeyPressed(KeyCode code)
        {
            return InternalCalls.Input_IsKeyPressed(code);
        }

        public static bool IsKeyReleased(KeyCode code)
        {
            return InternalCalls.Input_IsKeyReleased(code);
        }
    }
}
