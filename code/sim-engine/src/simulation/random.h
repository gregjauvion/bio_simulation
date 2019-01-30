#ifndef RANDOM_H
#define RANDOM_H

#include "math.h"

// basic type names (redefine if your bit lengths don't match)

typedef int Int; // 32 bit integer
typedef unsigned int Uint;

#ifdef _MSC_VER
typedef __int64 Llong; // 64 bit integer
typedef unsigned __int64 Ullong;
#else
typedef long long int Llong; // 64 bit integer
typedef unsigned long long int Ullong;
#endif

typedef char Char; // 8 bit integer
typedef unsigned char Uchar;

typedef double Doub; // default floating type
typedef long double Ldoub;

typedef bool Bool;

template<class T>
inline T SQR(const T a) {return a*a;}

struct Ran
{
    Ullong u,v,w;
    Ran(Ullong j) : v(4101842887655102017LL), w(1)
    {
        u = j ^ v; int64();
        v = u; int64();
        w = v; int64();
    }
    inline Ullong int64()
    {
        u = u * 2862933555777941757LL + 7046029254386353087LL;
        v ^= v >> 17; v ^= v << 31; v ^= v >> 8;
        w = 4294957665U*(w & 0xffffffff) + (w >> 32);
        Ullong x = u ^ (u << 21); x ^= x >> 35; x ^= x << 4;
        return (x + v) ^ w;
    }
    inline Doub doub() { return 5.42101086242752217E-20 * int64(); }
    inline Uint int32() { return (Uint)int64(); }
};

struct Expondev : Ran
{
    Doub beta;
    Expondev(Doub bbeta, Ullong i) : Ran(i), beta(bbeta) {}
    Doub dev()
    {
        Doub u;
        do u = doub(); while (u == 0.);
        return -log(u)/beta;
    }
};

struct Normaldev : Ran
{
    Normaldev(Ullong i)
    : Ran(i) {}
    Doub dev(Doub mu, Doub sig)
    {
        Doub u,v,x,y,q;
        do
        {
            u = doub();
            v = 1.7156*(doub()-0.5);
            x = u - 0.449871;
            y = abs(v) + 0.386595;
            q = SQR(x) + y*(0.19600*y-0.25472*x);
        }
        while (q > 0.27597
            && (q > 0.27846 || SQR(v) > -4.*log(u)*SQR(u)));
        return mu + sig*v/u;
    }
};


#endif // RANDOM_H
