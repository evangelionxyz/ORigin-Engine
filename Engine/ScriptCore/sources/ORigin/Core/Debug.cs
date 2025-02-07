namespace ORiginEngine
{
    public class Debug
    {
        public static void LogInfo(string message)
        {
            InternalCalls.Debug_LogInfo(message);
        }

        public static void LogError(string message)
        {
            InternalCalls.Debug_LogError(message);
        }

        public static void LogWarning(string message)
        {
            InternalCalls.Debug_LogWarning(message);
        }
    }
}
