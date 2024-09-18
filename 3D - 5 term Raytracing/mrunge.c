/* Runge-Kutta ODE solvers. */
/*
  Copyright (C) 2004 University of Texas at Austin
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <math.h>

#include "mrunge.h"
#include "alloc.h"

static int dim, nt;
static float dt, **k, *yk;

void runge_init(int dim1 /* dimensionality */, 
		   int n1   /* number of ray tracing steps */, 
		   float d1 /* step in time */)
/*< initialize >*/
{
    dim = dim1;
    nt = n1;
    dt = d1;
    yk = sf_floatalloc(dim);
    k = sf_floatalloc2(dim,3);
}

void runge_close(void)
/*< free allocated storage >*/
{
    free(yk);
    free(*k);
    free(k);
}

bool rayUnderHorizon(float *y,
                    float **traj,
                    float **horizon,
                    float *xaxis,
                    float *yaxis,
                    float *reflectionPoint,
                    float *plane,
                    int it,
                    int j1,
                    int j2)
{
    /*
    horizon[y][x];
    */
    float A[3], B[3], C[3], D[3];
    // float P[]

    /*
    A B C D are four points of considered plane Q
    Lower triangle (1)
        
                      C ______ D
                        |\ 2 |
                        | \  |
                        |1 \ |
                      A |___\| B
    */

    A[0]= xaxis[j1];
    A[1]= yaxis[j2];
    A[2]= horizon[j2][j1];
    B[0]= xaxis[j1+1];
    B[1]= yaxis[j2];
    B[2]= horizon[j2][j1+1];
    C[0]= xaxis[j1];
    C[1]= yaxis[j2+1];
    C[2]= horizon[j2+1][j1];
    /*ignore D*/
    D[0]= xaxis[j1+1];
    D[1]= yaxis[j2+1];
    D[2]= horizon[j2+1][j1+1];

    /* Check side
       y[0]: z;
       y[1]: x;
       y[2]: y;
    */
    float BCx = C[0] - B[0];
    float BCy = C[1] - B[1];
    /*Normal Vector*/
    float aLine = BCx;
    float bLine = - BCy;
    float cLine = - (aLine*C[0]+bLine*C[1]);
    if ((aLine*A[0]+bLine*A[1]+cLine)*(aLine*y[1]+bLine*y[2]+cLine) < 0)
    {
        A[0]=D[0];
        A[1]=D[1];
        A[2]=D[2];
    }
    
    float a1 = B[0] - A[0];
    float b1 = B[1] - A[1];
    float c1 = B[2] - A[2];
    float a2 = C[0] - A[0];
    float b2 = C[1] - A[1];
    float c2 = C[2] - A[2];
    float aPlane = b1 * c2 - b2 * c1;
    float bPlane = a2 * c1 - a1 * c2;
    float cPlane = a1 * b2 - b1 * a2;
    float dPlane = (- aPlane * A[0] - bPlane * A[1] - cPlane * A[2]);
    plane[0]=aPlane;
    plane[1]=bPlane;
    plane[2]=cPlane;
    plane[3]=dPlane;
    /*zxy*/
    if (aPlane*aPlane+bPlane*bPlane+cPlane*cPlane == 0) sf_warning ("Horizon Fail - mrunge.c");
    float T=(-dPlane-(y[1]*aPlane+y[2]*bPlane+y[0]*cPlane))/(aPlane*aPlane+bPlane*bPlane+cPlane*cPlane);
    float perpendicularPointx = y[1]+aPlane*T;
    float perpendicularPointy = y[2]+bPlane*T;
    float perpendicularPointz = y[0]+cPlane*T;

    if (perpendicularPointz<y[0])
    {
        /*
        A and B correspond to trajectory[it][0..2] and trajectory[it+1][]=y[0..2],
        a,b,c,d are parameters of plane Q.
        */
        A[0]= traj[it][1];  /* x */
        A[1]= traj[it][2];  /* y */
        A[2]= traj[it][0];  /* z */
        B[0]= y[1];         /* x */
        B[1]= y[2];         /* y */
        B[2]= y[0];         /* z */
        a1 = B[0] - A[0];
        b1 = B[1] - A[1];
        c1 = B[2] - A[2];
        if (a1*aPlane+b1*bPlane+c1*cPlane == 0) sf_warning ("Ray do not propagate - mrunge.c");
        float t=(-dPlane-A[0]*aPlane-A[1]*bPlane-A[2]*cPlane)/(a1*aPlane+b1*bPlane+c1*cPlane);
        reflectionPoint[0]=A[0] + t * a1; /* x */
        reflectionPoint[1]=A[1] + t * b1; /* y */
        reflectionPoint[2]=A[2] + t * c1; /* z */
        // sf_warning("perpendicularPoint = %f %f %f", perpendicularPointz, perpendicularPointx, perpendicularPointy);
        // sf_warning("reflectionPoint    = %f %f %f",reflectionPoint[2], reflectionPoint[0], reflectionPoint[1]);
        // sf_warning("vector = %f %f %f",reflectionPoint[2]-c/20.1, reflectionPoint[0]-a/20.1, reflectionPoint[1]-b/20.1);
        return true;
    }
    else return false;
}

int ode23_step (float* y    /* [dim] solution */, 
		   void* par   /* parameters for function evaluation */,
		   void (*rhs)(void*,float*,float*) 
		   /* RHS function */, 
		   int (*term)(void*,float*)
		   /* function returning 1 if the ray needs to terminate */, 
		   float** traj /* [nt+1][dim] - ray trajectory (output) */,
		   float** horizon, /*horizon*/
           int *nhor,
           float *dhor,
           float *ohor) 
/*< ODE solver for dy/dt = f where f comes from rhs(par,y,f)
  Note:
  1. Value of y is changed inside the function.
  2. The output code for it = ode23_step(...)
  it=0 - ray traced to the end without termination
  it>0 - ray terminated
  The total traveltime along the ray is 
  nt*dt if (it = 0); it*dt otherwise 
  >*/
{ 
    int it, i, j, ndim=3, nterm=5;
    float *xaxis, *yaxis; /*x-array and y-array*/
    float *reflectionPoint;
    float *consideredPoint;
    float *symmetricPoint;
    float *plane;
    plane=sf_floatalloc(4);
    symmetricPoint=sf_floatalloc(3);
    reflectionPoint=sf_floatalloc(3);
    consideredPoint=sf_floatalloc(3);
    xaxis=sf_floatalloc(nhor[0]);
    yaxis=sf_floatalloc(nhor[1]);
    for (int i = 0; i < nhor[0]; i++)
    {
        xaxis[i]=ohor[0]+dhor[0]*i;
    }
    for (int i = 0; i < nhor[1]; i++)
    {
        yaxis[i]=ohor[1]+dhor[1]*i;
    }

    if (traj != NULL) {
	for (i=0; i < ndim; i++) {
	    traj[0][i] = y[i];
	}
    }
    bool reflect = true;
    for (it = 0; it < nt; it++) {
		rhs (par, y, k[0]); 
		for (i=0; i < nterm; i++) {
			yk[i] = y[i] + 0.5*dt*k[0][i];
		}
		
		rhs (par, yk, k[1]); 
		for (i=0; i < nterm; i++) {
			yk[i] = y[i] + dt*(2.0*k[1][i]-k[0][i]);
		}
		
		rhs (par, yk, k[2]); 
		for (i=0; i < nterm; i++) {
			y[i] += dt*(k[0][i]+4.*k[1][i]+k[2][i])/6.0;
			
		}        
        for (i=0; i < ndim; i++) {
            if (traj != NULL) traj[it+1][i] = y[i];
        }
        
        if (term != NULL && term (par, y)) return (it+1);
       
        else{
            int j1 = (int)(y[1]-xaxis[0])/(xaxis[1]-xaxis[0]);
            int j2 = (int)(y[2]-yaxis[0])/(yaxis[1]-yaxis[0]);
            if (rayUnderHorizon(y, traj, horizon, xaxis, yaxis, reflectionPoint,plane, it, j1, j2)  )
            {
                sf_warning("it=%d (Reflection)",it);
                float zVector = reflectionPoint[2] - traj[it][0];
                float xVector = reflectionPoint[0] - traj[it][1];
                float yVector = reflectionPoint[1] - traj[it][2];
                
                if (zVector==0 || xVector == 0) {
                    zVector = traj[it+1][0]-reflectionPoint[2];
                    xVector = traj[it+1][1]-reflectionPoint[0];
                    yVector = traj[it+1][2]-reflectionPoint[1];
                    sf_warning("Recalculate (x,y,z)-Vector on Horizon");
                }

                consideredPoint[0]=reflectionPoint[2]+zVector;  /* z */
                consideredPoint[1]=reflectionPoint[0]+xVector;  /* x */
                consideredPoint[2]=reflectionPoint[1]+yVector;  /* y */
                
                float a=plane[0];                 /* x */
                float b=plane[1];                 /* y */
                float c=plane[2];                 /* z */
                float d=plane[3];
                float T=(-d-(consideredPoint[1]*a+consideredPoint[2]*b+consideredPoint[0]*c))
                        /(a*a+b*b+c*c);
                
                symmetricPoint[1]=consideredPoint[1]+a*2*T;     /* x */
                symmetricPoint[2]=consideredPoint[2]+b*2*T;     /* x */
                symmetricPoint[0]=consideredPoint[0]+c*2*T;     /* z */
                
                zVector = symmetricPoint[0]-reflectionPoint[2]; /* z */
                xVector = symmetricPoint[1]-reflectionPoint[0]; /* x */
                yVector = symmetricPoint[2]-reflectionPoint[1]; /* y */

                /* Recalculate Angle*/
                float tanInclinationAngle, inclinationAngle;
                float tanAzimuthAngle, azimuthAngle;

                /* -pi/2 < atanf(x) < pi/2 */
                tanInclinationAngle = sqrtf(xVector*xVector+yVector*yVector)/zVector;
                tanAzimuthAngle = (yVector/xVector);
                
                if (y[3]<0) {
                    inclinationAngle = - (SF_PI - (-atanf(tanInclinationAngle)));
                    if (xVector*yVector > 0) {
                        if (xVector > 0)
                        azimuthAngle = atanf(tanAzimuthAngle);
                        else 
                        azimuthAngle = atanf(tanAzimuthAngle) + SF_PI;
                    }
                    else {
                        if (xVector > 0)
                        azimuthAngle = atanf(tanAzimuthAngle) ;
                        else
                        azimuthAngle = atanf(tanAzimuthAngle) + SF_PI;
                    }
                }
                if (y[3]>0) {
                    inclinationAngle = + (SF_PI - (-atanf(tanInclinationAngle)));
                    if (xVector*yVector > 0) {
                        if (xVector > 0)
                        azimuthAngle = atanf(tanAzimuthAngle) +  SF_PI;
                        else 
                        azimuthAngle = atanf(tanAzimuthAngle) ;
                    }
                    else {
                        if (xVector > 0)
                        azimuthAngle = atanf(tanAzimuthAngle) + SF_PI;
                        else
                        azimuthAngle = atanf(tanAzimuthAngle) ;
                    }
                }
                y[0]=reflectionPoint[2];          /* z */
                y[1]=reflectionPoint[0];          /* x */
                y[2]=reflectionPoint[1];          /* y */
                y[3]=inclinationAngle;
                y[4]=azimuthAngle;

                for (i=0; i < ndim; i++) {
				    if (traj != NULL) traj[it+1][i] = y[i];	
			    }
                // return (it+1);
            }
        }
	}
    return 0;
}

/* END */
