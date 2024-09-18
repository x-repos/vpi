#include<math.h>

float *PlainFrom3Points( float x1,
                        float y1,
                        float z1,
                        float x2,
                        float y2,
                        float z2,
                        float x3,
                        float y3,
                        float z3,
                    )
{   
    //  26.00 x + 7.00 y + 9.00 z + 3.00 = 0
    static float A[4];

    float a1 = x2 - x1;
    float b1 = y2 - y1;
    float c1 = z2 - z1;
    float a2 = x3 - x1;
    float b2 = y3 - y1;
    float c2 = z3 - z1;
    float a = b1 * c2 - b2 * c1;
    float b = a2 * c1 - a1 * c2;
    float c = a1 * b2 - b1 * a2;
    float d = (- a * x1 - b * y1 - c * z1);
    A[0]=a;
    A[1]=b;
    A[2]=c;
    A[3]=d;
    return A;
}