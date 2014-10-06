#include <stdio.h>
#include <assert.h>
#include <fenv.h>
#define HIGH_HALF 1
#define LOW_HALF 0
typedef int int4;
typedef union {int4 i[2]; double x;} mynumber;
static const mynumber
   big = {{0x00000000, 0x42c80000 }},/*  52776558133248          */
   mp1 = {{0x58000000, 0x3FF921FB }},/*  1.5707963407039642      */
   mp2 = {{0x3C000000, 0xBE4DDE97 }},/* -1.3909067564377153e-08  */
   mp3 = {{0x99D747F2, 0xBC8CB3B3 }},/* -4.9789962505147994e-17  */
 hpinv = {{0x6DC9C883, 0x3FE45F30 }},/*  0.63661977236758138     */
 toint = {{0x00000000, 0x43380000 }};/*  6755399441055744        */

void check_k(double x, int k) {
    printf("computing sin(%20.20g)...\t k = %4d\n", x, k);
    assert(k < 440);
}

void __simplified_sin(double x) {
    double t,y,xn,a,da;
    mynumber u,v;
    int4 k,m,n;

    u.x = x;
    m   = u.i[HIGH_HALF];
    k   = 0x7fffffff&m;              /* no sign           */
    assert(k >= 0x400368fd && k < 0x419921FB);

    t   = (x * hpinv.x + toint.x);
    xn  = t - toint.x;
    v.x = t;
    y   = (x - xn * mp1.x) - xn * mp2.x;
    n   = v.i[LOW_HALF] & 3;
    da  = xn * mp3.x;
    a   = y - da;

    switch (n) { /* quarter of unit circle */
    case 0:
    case 2:
        if (n) { a=-a; }
        if (a * a < 0.01588) {
            return;
        }
        else {
            if (a<0) { a = -a;}
            u.x = big.x + a;
            k = u.i[LOW_HALF] << 2;
        }
        break;
    case 1:
    case 3:
        if (a<0) { a = -a; }
        u.x = big.x + a;
        k = u.i[LOW_HALF] << 2;
        break;
    }
    check_k(x, k);
}

int main() {
    fesetround(FE_TONEAREST);
    //fesetround(FE_DOWNWARD);
    double x = 1002.696926049077033996582;
    while(1) {
        __simplified_sin(x);
        x += 0.001;
    }
    return 0;
}
