// Copyright (c) Evangelion Manuhutu | ORigin Engine

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

        public static bool IsMouseDragging()
        {
            InternalCalls.Input_IsMouseDragging(out bool result);
            return result;
        }

        public static void SetMouseHide(bool hide)
        {
            InternalCalls.Input_SetMouseHide(hide);
        }

        public static bool HideMouse
        {
            get
            {
                InternalCalls.Input_IsMouseHidden(out bool result);
                return result;
            }
            set => InternalCalls.Input_SetMouseHide(value);
        }

        public static void ToggleMouseLock()
        {
            InternalCalls.Input_ToggleMouseLock();
        }

        public static bool IsMouseDown(MouseCode code)
        {
            InternalCalls.Input_IsMouseButtonDown(code, out bool result);
            return result;
        }

        public static bool IsKeyPressed(KeyCode code)
        {
            InternalCalls.Input_IsKeyPressed(code, out bool result);
            return result;
        }

        public static bool IsKeyReleased(KeyCode code)
        {
            InternalCalls.Input_IsKeyReleased(code, out bool result);
            return result;
        }
    }
}
