#ifndef VECTOR_MATH
#define VECTOR_MATH
#include "raylib.h"
#include <immintrin.h>
#include <stdint.h>

#pragma GCC diagnostic ignored "-Wshift-sign-overflow"

constexpr long BITMASK = 0xffffffff; // mask to zero first 32 bits of long
// And (mask) rhs to lhs and return result as a new Vector
Vector2 VecAndVec(Vector2 lhs, Vector2 rhs);

// Compares rhs and lhs and return result as a new Vector
Vector2 VecEqVec(Vector2 lhs, float rhs);

// Add rhs to lhs and return result as a new Vector
Vector2 VecAddVec(Vector2 lhs, Vector2 rhs);

// Sub rhs from lhs and return result as a new Vector
Vector2 VecSubVec(Vector2 lhs, Vector2 rhs);

// Mul rhs by lhs and return result as a new Vector
Vector2 VecMulVec(Vector2 lhs, Vector2 rhs);

// Sub rhs by lhs and return result as a new Vector
Vector2 VecDivVec(Vector2 lhs, Vector2 rhs);

// And (mask) rhs to lhs and return result as a new Vector
Vector2 VecAndI32(Vector2 lhs, int32_t rhs);

// Add rhs to lhs and return result as a new Vector
Vector2 VecAddFloat(Vector2 lhs, float rhs);

// Sub rhs from lhs and return result as a new Vector
Vector2 VecSubFloat(Vector2 lhs, float rhs);

// Mul rhs by lhs and return result as a new Vector
Vector2 VecMulFloat(Vector2 lhs, float rhs);

// Sub rhs by lhs and return result as a new Vector
Vector2 VecDivFloat(Vector2 lhs, float rhs);

// And (mask) rhs to lhs and return result as a new Vector
Vector2 VecAndVec(Vector2 lhs, Vector2 rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs.x, rhs.y, 0, 0 };
    ret   = _mm_and_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Compares rhs and lhs and return result as a new Vector
Vector2 VecEqVec(Vector2 lhs, float rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs, rhs, 0, 0 };
    ret   = _mm_cmpeq_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Add rhs to lhs and return result as a new Vector
Vector2 VecAddVec(Vector2 lhs, Vector2 rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs.x, rhs.y, 0, 0 };
    ret   = _mm_add_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Sub rhs from lhs and return result as a new Vector
Vector2 VecSubVec(Vector2 lhs, Vector2 rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs.x, rhs.y, 0, 0 };
    ret   = _mm_sub_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Mul rhs by lhs and return result as a new Vector
Vector2 VecMulVec(Vector2 lhs, Vector2 rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs.x, rhs.y, 0, 0 };
    ret   = _mm_mul_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Sub rhs by lhs and return result as a new Vector
Vector2 VecDivVec(Vector2 lhs, Vector2 rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs.x, rhs.y, 0, 0 };
    ret   = _mm_div_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Add rhs to lhs and return result as a new Vector
Vector2 VecAddFloat(Vector2 lhs, float rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs, rhs, 0, 0 };
    ret   = _mm_add_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// And (mask) rhs to lhs and return result as a new Vector
Vector2 VecAndI32(Vector2 lhs, int32_t rhs) {
    __m128 left, right, ret;
    left = (__m128){ lhs.x, lhs.y, 0, 0 };

    // I hate this so fucking much...
    // 1. Make rhs long, so it doesn't get 0d when shifted
    // 2. Move first 32 bits of rhs to the left
    // 3. make another long and set first 32 bits to 0, because
    //      otherwise they will be all equal to 1
    //      Thus the need for the `BITMASK = 0xffffffff`
    // 4. Combine them into mask by OR'ing them together
    long mask = ((long)rhs << 32) | (BITMASK & (long)rhs);

    right = (__m128)(__m128i){ mask, mask };
    ret   = _mm_and_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Sub rhs from lhs and return result as a new Vector
Vector2 VecSubFloat(Vector2 lhs, float rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs, rhs, 0, 0 };
    ret   = _mm_sub_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Mul rhs by lhs and return result as a new Vector
Vector2 VecMulFloat(Vector2 lhs, float rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs, rhs, 0, 0 };
    ret   = _mm_mul_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}

// Sub rhs by lhs and return result as a new Vector
Vector2 VecDivFloat(Vector2 lhs, float rhs) {
    __m128 left, right, ret;
    left  = (__m128){ lhs.x, lhs.y, 0, 0 };
    right = (__m128){ rhs, rhs, 0, 0 };
    ret   = _mm_div_ps(left, right);
    return (Vector2){ ret[0], ret[1] };
}
#endif // VECTOR_MATH
