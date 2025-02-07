namespace ORiginEngine
{
    public class Scene
    {
        public static void LockMouse()
        {
            InternalCalls.Scene_LockMouse();
        }

        public static void UnlockMouse()
        {
            InternalCalls.Scene_UnlockMouse();
        }

        public static bool IsFocusing()
        {
            InternalCalls.Scene_IsFocusing(out bool result);
            return result;
        }
    }
}
