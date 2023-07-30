// MathLibrary.h
#pragma once

namespace MathLibraryaaa
{
    class Arithmetic
    {
    public:
        Arithmetic();

        // Returns a + b
        static double Add(double a, double b);

        // Returns a - b
        static double Subtract(double a, double b);

        // Returns a * b
        static double Multiply(double a, double b);

        // Returns a / b
        static double Divide(double a, double b);
    };
}