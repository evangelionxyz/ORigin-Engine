// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

using System;

namespace ORiginEngine
{
    public class Entity
    {
        public Entity()
        {
            Console.WriteLine("Hello World From C# on Main Constructor");

            Vector3 pos = new Vector3(1.3f, 2.32f, 3.4f);
            Vector3 result = Log(pos);
            Console.WriteLine($"C# Says: {result.X}, {result.Y}, {result.Z}");
            Console.WriteLine($"C# Says: {InternalCalls.NativeLog_VectorDot(ref pos)}");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World From C# on Method");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine(message);
        }

        public void PrintInt(int number)
        {
            Console.WriteLine($"From C# {number}");
        }

        public void PrintInts(int number0, int number1)
        {
            Console.WriteLine($"From C# {number0},{number1}");
        }

        private void Log(string text, int parameter)
        {
            InternalCalls.NativeLog(text, parameter);
        }

        private Vector3 Log(Vector3 parameter)
        {
            InternalCalls.NativeLog_Vector(ref parameter, out Vector3 result);
            return result;
        }
    }
}
