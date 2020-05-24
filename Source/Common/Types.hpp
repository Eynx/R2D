/*
-------------------------------------------------------------------------------
    Filename: Common/Types.hpp
-------------------------------------------------------------------------------
*/

// Header guard
#pragma once

// --------------------------------------------------------------------------------------------
namespace R2D
{
    // Types

    // Unsigned 8-bit integer type.
    typedef unsigned char uByte;
    // Signed 8-bit integer type.
    typedef char Byte;

    // Unsigned 16-bit integer type.
    typedef unsigned short uShort;
    // Signed 16-bit integer type.
    typedef short Short;

    // Unsigned 32-bit integer type.
    typedef unsigned int uInt;
    // Signed 32-bit integer type.
    typedef int Int;

    // Unsigned 64-bit integer type.
    typedef unsigned long long uLong;
    // Signed 64-bit integer type.
    typedef long long Long;

    // 32-bit floating-point type.
    typedef float Float;
    // 64-bit floating-point type.
    typedef double Double;

    // 1-bit helper type.
    typedef bool Bool;
    // Special typeless identifier.
    typedef void Void;

    // ----------------------------------------------------------------------------------------
    // 2D Vector; Templated components.
    template <typename Type> struct Vector2
    {
        // Members
        // --------------------------------------------------------------------
        // Swizzle union (magic happens here).
        union
        {
            struct
            {
                // X component of the 2D vector.
                Type x;
                // Y component of the 2D vector.
                Type y;
            };
            // The vector components as an array.
            Type m[2];
        };

        // Constructors
        // --------------------------------------------------------------------
        // Constructor; Default initalization
        constexpr Vector2() : x(), y() {};
        // Constructor; Initialize all components to one value
        constexpr Vector2(Type value) : x(value), y(value) {};
        // Constructor; Directly initialize each value
        constexpr  Vector2(Type x, Type y) : x(x), y(y) {};
        // Constructor; Initialize components from an array of values
        constexpr  Vector2(const Type(&array)[2]) : x(array[0]), y(array[1]) {};
        // Copy Constructor; Copy over components from another vector
        constexpr Vector2(const Vector2<Type>& other) : x(other.x), y(other.y) {};

        // Constructor; Conversion from one vector type to another
        template <typename Other> inline constexpr  Vector2(const Vector2<Other>& other) : x(other.x), y(other.y) {};

        // Conversion operators
        // --------------------------------------------------------------------
        template <typename Other> inline constexpr Vector2<Type>& operator = (const Vector2<Other>& other) { x = other.x; y = other.y; return *this; };
        // Conversion casting and initialization
        template <typename Other> inline constexpr operator Vector2<Other>() { return Vector2<Other>(*this); };

        // Assignment operators
        // --------------------------------------------------------------------
        inline constexpr Vector2<Type>& operator = (const Vector2<Type>& other) { x = other.x; y = other.y; return *this; };
        // Arithmetic operators
        inline constexpr Vector2<Type>& operator += (const Vector2<Type>& other) { x += other.x; y += other.y; return *this; };
        inline constexpr Vector2<Type>& operator -= (const Vector2<Type>& other) { x -= other.x; y -= other.y; return *this; };
        inline constexpr Vector2<Type>& operator *= (const Vector2<Type>& other) { x *= other.x; y *= other.y; return *this; };
        inline constexpr Vector2<Type>& operator /= (const Vector2<Type>& other) { x /= other.x; y /= other.y; return *this; };
        inline constexpr Vector2<Type>& operator *= (const Type& value) { x *= value; y *= value; return *this; };
        inline constexpr Vector2<Type>& operator /= (const Type& value) { x /= value; y /= value; return *this; };
        // Bitwise operators
        inline constexpr Vector2<Type>& operator &= (const Vector2<Type>& other) { x &= other.x; y &= other.y; return *this; };
        inline constexpr Vector2<Type>& operator |= (const Vector2<Type>& other) { x |= other.x; y |= other.y; return *this; };
        inline constexpr Vector2<Type>& operator ^= (const Vector2<Type>& other) { x ^= other.x; y ^= other.y; return *this; };

        // Unary operators
        // --------------------------------------------------------------------
        // Arithmetic operators
        inline constexpr Vector2<Type> operator + () const { return Vector2<Type>(*this); };
        inline constexpr Vector2<Type> operator - () const { return Vector2<Type>(-x, -y); };
        // Bitwise operators
        inline constexpr Vector2<Type> operator ~ () const { return Vector2<Type>(~x, ~y); };

        // Binary operators
        // --------------------------------------------------------------------
        // Arithmetic operators
        inline constexpr Vector2<Type> operator + (const Vector2<Type>& other) const { return Vector2<Type>(x + other.x, y + other.y); };
        inline constexpr Vector2<Type> operator - (const Vector2<Type>& other) const { return Vector2<Type>(x - other.x, y - other.y); };
        inline constexpr Vector2<Type> operator * (const Vector2<Type>& other) const { return Vector2<Type>(x * other.x, y * other.y); };
        inline constexpr Vector2<Type> operator / (const Vector2<Type>& other) const { return Vector2<Type>(x / other.x, y / other.y); };
        inline constexpr Vector2<Type> operator * (const Type& value) const { return Vector2<Type>(x * value, y * value); };
        inline constexpr Vector2<Type> operator / (const Type& value) const { return Vector2<Type>(x / value, y / value); };
        // Bitwise operators
        inline constexpr Vector2<Type> operator & (const Vector2<Type>& other) const { return Vector2<Type>(x & other.x, y & other.y); };
        inline constexpr Vector2<Type> operator | (const Vector2<Type>& other) const { return Vector2<Type>(x | other.x, y | other.y); };
        inline constexpr Vector2<Type> operator ^ (const Vector2<Type>& other) const { return Vector2<Type>(x ^ other.x, y ^ other.y); };
        // Other operators
        inline constexpr Vector2<Type> operator % (const Vector2<Type>& other) const { return Vector2<Type>(x % other.x, y % other.y); };

        // --------------------------------------------------------------------
        // Comparison operators
        inline constexpr Bool operator == (const Vector2<Type>& other) const { return (x == other.x) && (y == other.y); };
        inline constexpr Bool operator != (const Vector2<Type>& other) const { return (x != other.x) && (y != other.y); };

        // --------------------------------------------------------------------
        // Array operators
        inline constexpr Type& operator [] (Int x) { return m[x]; };
        inline constexpr const Type& operator [] (Int x) const { return m[x]; };
    };

    // ----------------------------------------------------------------------------------------
    // 3D Vector; Templated components.
    template <typename Type> struct Vector3
    {
        // Members
        // --------------------------------------------------------------------
        // Swizzle union (even more magic happens here).
        union
        {
            struct
            {
                // X component of the 3D vector.
                Type x;
                union
                {
                    struct
                    {
                        // Y component of the 3D vector.
                        Type y;
                        // Z component of the 3D vector.
                        Type z;
                    };
                    // YZ components of the 3D vector.
                    Vector2<Type> yz;
                };
            };
            // XY components of the 3D vector.
            Vector2<Type> xy;
            // The vector components as an array.
            Type m[3];
        };

        // Constructors
        // --------------------------------------------------------------------
        // Constructor; Default initialization.
        constexpr Vector3() : x(), y(), z() {};
        // Constructor; Initialize all components to one value.
        constexpr Vector3(Type value) : x(value), y(value), z(value) {};
        // Constructor; Directly initialize each value.
        constexpr Vector3(Type x, Type y, Type z) : x(x), y(y), z(z) {};
        // Constructor; 2D Vector promotion with explicit z.
        constexpr Vector3(const Vector2<Type>& vector, Type z) : x(vector.x), y(vector.y), z(z) {};
        // Constructor; 2D Vector promotion with explicit x.
        constexpr Vector3(Type x, const Vector2<Type>& vector) : x(vector.x), y(vector.y), z(z) {};
        // Constructor; Initialize components from an array of values.
        constexpr Vector3(const Type(&array)[3]) : x(array[0]), y(array[1]), z(array[2]) {};
        // Copy Constructor; Copy over components from another vector.
        constexpr Vector3(const Vector3<Type>& other) : x(other.x), y(other.y), z(other.z) {};

        // Constructor; Conversion from one vector type to another.
        template <typename Other> inline constexpr Vector3(const Vector3<Other>& other) : x(other.x), y(other.y), z(other.z) {};

        // Conversion operators
        // --------------------------------------------------------------------
        template <typename Other> inline constexpr Vector3<Type>& operator = (const Vector3<Other>& other) { x = other.x; y = other.y; z = other.z; return *this; };
        // Conversion casting and initialization
        template <typename Other> inline constexpr operator Vector3<Other>() { return Vector3<Other>(*this); };

        // Assignment operators
        // --------------------------------------------------------------------
        inline constexpr Vector3<Type>& operator = (const Vector3<Type>& other) { x = other.x; y = other.y; z = other.z; return *this; };
        // Arithmetic operators
        inline constexpr Vector3<Type>& operator += (const Vector3<Type>& other) { x += other.x; y += other.y; z += other.z; return *this; };
        inline constexpr Vector3<Type>& operator -= (const Vector3<Type>& other) { x -= other.x; y -= other.y; z -= other.z; return *this; };
        inline constexpr Vector3<Type>& operator *= (const Vector3<Type>& other) { x *= other.x; y *= other.y; z *= other.z; return *this; };
        inline constexpr Vector3<Type>& operator /= (const Vector3<Type>& other) { x /= other.x; y /= other.y; z /= other.z; return *this; };
        inline constexpr Vector3<Type>& operator *= (const Type& value) { x *= value; y *= value; z *= value; return *this; };
        inline constexpr Vector3<Type>& operator /= (const Type& value) { x /= value; y /= value; z /= value; return *this; };
        // Bitwise operators
        inline constexpr Vector3<Type>& operator &= (const Vector3<Type>& other) { x &= other.x; y &= other.y; z &= other.z; return *this; };
        inline constexpr Vector3<Type>& operator |= (const Vector3<Type>& other) { x |= other.x; y |= other.y; z |= other.z; return *this; };
        inline constexpr Vector3<Type>& operator ^= (const Vector3<Type>& other) { x ^= other.x; y ^= other.y; z ^= other.z; return *this; };

        // Unary operators
        // --------------------------------------------------------------------
        // Arithmetic operators
        inline constexpr Vector3<Type> operator + () const { return *this; };
        inline constexpr Vector3<Type> operator - () const { return Vector3<Type>(-x, -y, -z); };
        // Bitwise operators
        inline constexpr Vector3<Type> operator ~ () const { return Vector3<Type>(~x, ~y, ~z); };

        // Binary operators
        // --------------------------------------------------------------------
        // Arithmetic operators
        inline constexpr Vector3<Type> operator + (const Vector3<Type>& other) const { return Vector3<Type>(x + other.x, y + other.y, z + other.z); };
        inline constexpr Vector3<Type> operator - (const Vector3<Type>& other) const { return Vector3<Type>(x - other.x, y - other.y, z - other.z); };
        inline constexpr Vector3<Type> operator * (const Vector3<Type>& other) const { return Vector3<Type>(x * other.x, y * other.y, z * other.z); };
        inline constexpr Vector3<Type> operator / (const Vector3<Type>& other) const { return Vector3<Type>(x / other.x, y / other.y, z / other.z); };
        inline constexpr Vector3<Type> operator * (const Type& value) const { return Vector3<Type>(x * value, y * value, z * value); };
        inline constexpr Vector3<Type> operator / (const Type& value) const { return Vector3<Type>(x / value, y / value, z / value); };
        // Bitwise operators
        inline constexpr Vector3<Type> operator & (const Vector3<Type>& other) const { return Vector3<Type>(x & other.x, y & other.y, z & other.z); };
        inline constexpr Vector3<Type> operator | (const Vector3<Type>& other) const { return Vector3<Type>(x | other.x, y | other.y, z | other.z); };
        inline constexpr Vector3<Type> operator ^ (const Vector3<Type>& other) const { return Vector3<Type>(x ^ other.x, y ^ other.y, z ^ other.z); };
        // Other operators
        inline constexpr Vector3<Type> operator % (const Vector3<Type>& other) const { return Vector3<Type>(x % other.x, y % other.y, z % other.z); };

        // --------------------------------------------------------------------
        // Comparison operators
        inline constexpr Bool operator == (const Vector3<Type>& other) const { return x == other.x && y == other.y && z == other.z; };
        inline constexpr Bool operator != (const Vector3<Type>& other) const { return x != other.x && y != other.y && z != other.z; };

        // --------------------------------------------------------------------
        // Array access
        inline constexpr Type& operator [] (Int index) { return m[index]; };
        inline constexpr const Type& operator [] (Int index) const { return m[index]; };
    };

    // ----------------------------------------------------------------------------------------
    // 4D Vector; Templated components.
    template <typename Type> struct Vector4
    {
        // Members
        // --------------------------------------------------------------------
        // Swizzle union (forbidden magic).
        union
        {
            struct
            {
                // X component of the 4D vector.
                Type x;
                union
                {
                    struct
                    {
                        union
                        {
                            struct
                            {
                                // Y component of the 4D vector.
                                Type y;
                                // Z component of the 4D vector.
                                Type z;
                            };
                            // YZ components of the 4D vector.
                            Vector2<Type> yz;
                        };
                        // W component of the 4D vector.
                        Type w;
                    };
                    // YZW components of the 4D vector.
                    Vector3<Type> yzw;
                };
            };
            struct
            {
                // XY components of the 4D vector.
                Vector2<Type> xy;
                // ZW components of the 4D vector.
                Vector2<Type> zw;
            };
            // XYZ components of the 4D vector.
            Vector3<Type> xyz;
            // The vector components as an array.
            Type m[4];
        };

        // Constructors
        // --------------------------------------------------------------------
        // Constructor; Default intialization
        constexpr Vector4() : x(), y(), z(), w() {};
        // Constructor; Initialize all components to one value
        constexpr Vector4(Type value) : x(value), y(value), z(value), w(value) {};
        // Constructor; Directly initialize each value
        constexpr Vector4(Type x, Type y, Type z, Type w) : x(x), y(y), z(z), w(w) {};
        // Constructor; 2D Vector promotion with explicit z and w
        constexpr Vector4(const Vector2<Type>& vector, Type z, Type w) : x(vector.x), y(vector.y), z(z), w(w) {};
        // Constructor; 2D Vector promotion with explicit x and w
        constexpr Vector4(Type x, const Vector2<Type>& vector, Type w) : x(x), y(vector.x), z(vector.y), w(w) {};
        // Constructor; 2D Vector promotion with explicit x and y
        constexpr Vector4(Type x, Type y, const Vector2<Type>& vector) : x(x), y(y), z(vector.x), w(vector.y) {};
        // Constructor; Merging two 2D Vectors
        constexpr Vector4(const Vector2<Type>& vectorA, const Vector2<Type>& vectorB) : x(vectorA.x), y(vectorA.y), z(vectorB.x), w(vectorB.y) {};
        // Constructor; 3D Vector promotion with explicit w
        constexpr Vector4(const Vector3<Type>& vector, Type w) : x(vector.x), y(vector.y), z(vector.z), w(w) {};
        // Constructor; 3D Vector promotion with explicit x
        constexpr Vector4(Type x, const Vector3<Type>& vector) : x(x), y(vector.x), z(vector.y), w(vector.z) {};
        // Constructor; Initialize components from an array of values
        constexpr Vector4(const Type(&array)[4]) : x(array[0]), y(array[1]), z(array[2]), w(array[3]) {};
        // Copy Constructor; Copy over components from another vector
        constexpr Vector4(const Vector4<Type>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {};

        // Constructor; Conversion from one vector type to another
        template <typename Other> inline constexpr Vector4(const Vector4<Other>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {};

        // Conversion operators
        // --------------------------------------------------------------------
        template <typename Other> inline constexpr Vector4<Type>& operator = (const Vector4<Other>& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; };
        // Conversion casting and initialization
        template <typename Other> inline constexpr operator Vector4<Other>() { return Vector4<Other>(*this); };

        // Assignment operators
        // --------------------------------------------------------------------
        inline constexpr Vector4<Type>& operator = (const Vector4<Type>& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; };
        // Arithmetic operators
        inline constexpr Vector4<Type>& operator += (const Vector4<Type>& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; };
        inline constexpr Vector4<Type>& operator -= (const Vector4<Type>& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; };
        inline constexpr Vector4<Type>& operator *= (const Vector4<Type>& other) { x *= other.x; y *= other.y; z *= other.z; w *= other.w; return *this; };
        inline constexpr Vector4<Type>& operator /= (const Vector4<Type>& other) { x /= other.x; y /= other.y; z /= other.z; w /= other.w; return *this; };
        inline constexpr Vector4<Type>& operator *= (const Type& value) { x *= value; y *= value; z *= value; w *= value; return *this; };
        inline constexpr Vector4<Type>& operator /= (const Type& value) { x /= value; y /= value; z /= value; w /= value; return *this; };
        // Bitwise operators
        inline constexpr Vector4<Type>& operator &= (const Vector4<Type>& other) { x &= other.x; y &= other.y; z &= other.z; w &= other.w; return *this; };
        inline constexpr Vector4<Type>& operator |= (const Vector4<Type>& other) { x |= other.x; y |= other.y; z |= other.z; w |= other.w; return *this; };
        inline constexpr Vector4<Type>& operator ^= (const Vector4<Type>& other) { x ^= other.x; y ^= other.y; z ^= other.z; w ^= other.w; return *this; };

        // Unary operators
        // --------------------------------------------------------------------
        // Arithmetic operators
        inline constexpr Vector4<Type> operator + () const { return *this; };
        inline constexpr Vector4<Type> operator - () const { return Vector4<Type>(-x, -y, -z, -w); };
        // Bitwise operators
        inline constexpr Vector4<Type> operator ~ () const { return Vector4<Type>(~x, ~y, ~z, ~w); };

        // Binary operators
        // --------------------------------------------------------------------
        // Arithmetic operators
        inline constexpr Vector4<Type> operator + (const Vector4<Type>& other) const { return Vector4<Type>(x + other.x, y + other.y, z + other.z, w + other.w); };
        inline constexpr Vector4<Type> operator - (const Vector4<Type>& other) const { return Vector4<Type>(x - other.x, y - other.y, z - other.z, w - other.w); };
        inline constexpr Vector4<Type> operator * (const Vector4<Type>& other) const { return Vector4<Type>(x * other.x, y * other.y, z * other.z, w * other.w); };
        inline constexpr Vector4<Type> operator / (const Vector4<Type>& other) const { return Vector4<Type>(x / other.x, y / other.y, z / other.z, w / other.w); };
        inline constexpr Vector4<Type> operator * (const Type& value) const { return Vector4<Type>(x * value, y * value, z * value, w * value); };
        inline constexpr Vector4<Type> operator / (const Type& value) const { return Vector4<Type>(x / value, y / value, z / value, w / value); };
        // Bitwise operators
        inline constexpr Vector4<Type> operator & (const Vector4<Type>& other) const { return Vector4<Type>(x & other.x, y & other.y, z & other.z, w & other.w); };
        inline constexpr Vector4<Type> operator | (const Vector4<Type>& other) const { return Vector4<Type>(x | other.x, y | other.y, z | other.z, w | other.w); };
        inline constexpr Vector4<Type> operator ^ (const Vector4<Type>& other) const { return Vector4<Type>(x ^ other.x, y ^ other.y, z ^ other.z, w ^ other.w); };
        // Other operators
        inline constexpr Vector4<Type> operator % (const Vector4<Type>& other) const { return Vector4<Type>(x % other.x, y % other.y, z % other.z, w % other.w); };

        // --------------------------------------------------------------------
        // Comparison operators
        inline constexpr Bool operator == (const Vector4<Type>& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; };
        inline constexpr Bool operator != (const Vector4<Type>& other) const { return x != other.x && y != other.y && z != other.z && w != other.w; };

        // --------------------------------------------------------------------
        // Array access
        inline constexpr Type& operator [] (Int index) { return m[index]; };
        inline constexpr const Type& operator [] (Int index) const { return m[index]; };
    };

    //-----------------------------------------------------------------------------------------
    // Vectors
    //-----------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // 2D Vector; 32-bit integer components.
    typedef Vector2<Int> Int2;
    // 3D Vector; 32-bit integer components.
    typedef Vector3<Int> Int3;
    // 4D Vector; 32-bit integer components.
    typedef Vector4<Int> Int4;

    //-------------------------------------------------------------------------
    // 2D Vector; 32-bit floating-point components.
    typedef Vector2<Float> Float2;
    // 3D Vector; 32-bit floating-point components.
    typedef Vector3<Float> Float3;
    // 4D Vector; 32-bit floating-point components.
    typedef Vector4<Float> Float4;

    //-----------------------------------------------------------------------------------------
    // Matrices
    //-----------------------------------------------------------------------------------------

    // 2D matrices
    //-------------------------------------------------------------------------

    // 2x2 Matrix; Two 2D 32-bit floating-point vectors.
    typedef Vector2<Float2> Float2x2;
}
