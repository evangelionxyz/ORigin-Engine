// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using System.Runtime.CompilerServices;

namespace ORigin
{
    public struct Vector3
    {
        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public float X, Y, Z;
    }

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int number);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector(ref Vector3 parameter, out Vector3 result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float NativeLog_VectorDot(ref Vector3 parameter);
    }

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