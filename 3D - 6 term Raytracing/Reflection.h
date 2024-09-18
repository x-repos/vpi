#include <stdio.h>

#include<math.h>

float *Returnij()
{
    /*
    y[0]: z
    y[1]: x
    y[2]: y
    y[3]: angle
    y[4]: angle
    y[5]: angle
    */
    // static int i,j;
    // j = (y[1]-h[0][0])/(h[1][0]-h[0][0]); 
    // a = (h[j+1][1]-h[j][1])/(h[j+1][0]-h[j][0]);
    // b = (h[j+1][1]*h[j][0]-h[j][1]*h[j+1][0])/(h[j][0]-h[j+1][0]);
    // z0 = a*y[1]+b;
	
		// if (z0 < y[0])
}




float rayUnderHorizon(  float *y;
                        float **ray,
                        float **h,
                        float a1,
                        float b1,
                        float c1,
                        float a2,
                        float b2,
                        float c2,
                        float d2
)
{   
    int j1;
    j1 = (y[1]-h[0][0])/(h[1][0]-h[0][0]);
     
    j2 = (y[2]-h[0][0])/(h[1][0]-h[0][0]); 

    float t, T;
    static float Array[3];
    T=(-d2-(a1*a2+b1*b2+c1*c2))/(a2*a2+b2*b2+c2*c2);
    t=T;
    Array[0]=a1+a2*t;
    Array[1]=b1+b2*t;
    Array[2]=c1+c2*t;

    if (c1+c2*t)>y[0];

}

float *Reflection(float x1,
                  float y1,
                  float z1,
                  float x2,
                  float y2,
                  float z2,
                  float a,
                  float b,
                  float c,
                  float d)
{
  float static Array[3]; 
  float a1=x2-x1;
  float b1=y2-y1;
  float c1=z2-z1;
  float t;
  t=(-d-x1*a-y1*b-z1*c)/(a1*a+b1*b+c1*c);
  Array[0]=x1+t*a1;
  Array[1]=y1+t*b1;
  Array[2]=z1+t*c1;
  return Array;
}


float *PlaneEq  (float x1,
                float y1,
                float z1,
                float x2,
                float y2,
                float z2,
                float x3,
                float y3,
                float z3)
{   
    //  26.00 x + 7.00 y + 9.00 z + 3.00 = 0
    static float Array[4];
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
    Array[0]=a;
    Array[1]=b;
    Array[2]=c;
    Array[3]=d;
    return Array;
}
void main()
{
    float *plane, *RflectionPoint, *reflection_Point;
    float point[3];
    point[0]=0;
    point[1]=0;
    point[2]=0;
    point[3]=5;
    point[4]=5;
    point[5]=6;
    
    plane=PlaneEq(1,2,4,6,1,3,3,3,3);/*A B C*/

    for ( int i = 0; i < 4; i++ ) {
      printf( "*(p + %d) : %f\n", i, *(plane + i));
    }

    RflectionPoint=ReflectionPoint(point[0],point[1],point[2],plane[0],plane[1],plane[2],plane[3]);
    for ( int i = 0; i < 3; i++ ) {
      printf( "*(p + %d) : %f\n", i, *(RflectionPoint + i));
    }
    reflection_Point=Reflection(point[0],point[1],point[2],point[3],point[4],point[5],plane[0],plane[1],plane[2],plane[3]);
    for ( int i = 0; i < 3; i++ ) {
      printf( "*(p + %d) : %f\n", i, *(reflection_Point + i));
    }

}