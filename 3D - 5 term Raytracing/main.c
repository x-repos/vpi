/* Ray tracing by a Runge-Kutta integrator.

Takes: > rays.rsf

Rays can be plotted with sfplotrays.
*/
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

#include <math.h>
#include <rsf.h>
#include "mraytrace.h"
#include "grid2.h"
#include "grid3.h"
#include "atela.h"
#include "eno3.h"
#include "mraytrace.c"
#include "grid2.c"
#include "grid3.c"
#include "atela.c"
#include "eno3.c"



int main(int argc, char* argv[])
{
    FILE *fp;
    fp = fopen("rays.txt", "w");
    bool velocity, sym, verb, escvar;
    int is, n[3], im, nm, order, nshot, ndim;
    int nx, nt, nt1, nr, ir, it, i;
    float dt, da=0., db=0., a0, amax, b0, bmax, v0, deg2rad;
    float x[3], p[3], d[3], o[3],**s;
    float *a, *b;
    // float **h;
    float *slow;
    float **traj;

    int nhor[2],horizonSize;
    float dhor[2];
    float ohor[2];
    float **horizon,*temp;

    raytrace rt;
    sf_file shots, vel, rays, angles, hor;
    sf_init (argc,argv);
    vel = sf_input("in");
    hor = sf_input("hor");
	rays = sf_output("out");
    sf_warning(".");

    /* get 3-D grid parameters */
    if (!sf_histint(vel,"n1",n))     sf_error("No n1= in input");
    if (!sf_histint(vel,"n2",n+1))   sf_error("No n2= in input");
    if (!sf_histint(vel,"n3",n+2))   sf_error("No n3= in input");
    if (!sf_histfloat(vel,"d1",d))   sf_error("No d1= in input");
    if (!sf_histfloat(vel,"d2",d+1)) sf_error("No d2= in input");
    if (!sf_histfloat(vel,"d3",d+2)) sf_error("No d3= in input");
    if (!sf_histfloat(vel,"o1",o))   o[0]=0.;
    if (!sf_histfloat(vel,"o2",o+1)) o[1]=0.;
    if (!sf_histfloat(vel,"o3",o+2)) o[2]=0.;
    
    if (!sf_getint("nt",&nt))           sf_error("Need nt=");
    /* Number of time steps */
    if (!sf_getfloat("dt",&dt))         sf_error("Need dt=");
    /* Sampling in time */

    /* additional parameters */
    if(!sf_getbool("vel",&velocity))    velocity=true;
    /* If y, input is velocity; if n, slowness */
    if(!sf_getint("order",&order))      order=4;
    /* Interpolation order */
    if (!sf_getbool("sym",&sym))        sym=true;
    /* if y, use symplectic integrator */
    if(!sf_getbool("verb",&verb))       verb=true;
    /* Verbosity flag */
    if(!sf_getbool("escvar",&escvar))   escvar=false;
    /* If y - output escape values, n - trajectories */

    /* get shot locations */   
    nshot = 1;
    ndim = 3;
    s = sf_floatalloc2 (ndim,nshot);
    if (!sf_getfloat("zshot",&s[0][0]))   sf_error("No zshot= in input");
    if (!sf_getfloat("xshot",&s[0][1]))   sf_error("No xshot= in input");
    if (!sf_getfloat("yshot",&s[0][2]))   sf_error("No yshot= in input");

    deg2rad = SF_PI/180.;

    if (!sf_getint("nr",&nr))           sf_error("Need nr=");
    /* number of angles (if no anglefile) */
    if (!sf_getfloat("a0",&a0))         sf_error("Need a0=");
    /* minimum angle (if no anglefile) */
    if (!sf_getfloat("amax",&amax))     sf_error("Need aMax=");
    /* maximum angle (if no anglefile) */
    if (!sf_getfloat("b0",&b0))         sf_error("Need b0=");
    /* minimum angle (if no anglefile) */
    if (!sf_getfloat("bmax",&bmax))     sf_error("Need bMax=");
    /* maximum angle (if no anglefile) */

    /* convert degrees to radians */
    a0   *= deg2rad;
    amax *= deg2rad;
    b0   *= deg2rad;
    bmax *= deg2rad;
    da = (nr > 1)? (amax - a0)/(nr-1) : 0.;
    db = (nr > 1)? (bmax - b0)/(nr-1) : 0.;
    a  = sf_floatalloc(nr);
    b  = sf_floatalloc(nr);

	/* load horizon file */
    if (!sf_histint(hor,"n1",nhor))     sf_error("No nhor1= in input");
    if (!sf_histint(hor,"n2",nhor+1))   sf_error("No nhor2= in input");
    if (!sf_histfloat(hor,"d1",dhor))   sf_error("No dhor1= in input");
    if (!sf_histfloat(hor,"d2",dhor+1)) sf_error("No dhor2= in input");
    if (!sf_histfloat(hor,"o1",ohor))   ohor[0]=0.;
    if (!sf_histfloat(hor,"o2",ohor+1)) ohor[1]=0.;
    nm=nhor[0]*nhor[1];
	horizon = sf_floatalloc2(nhor[0],nhor[1]);
    sf_floatread(horizon[0],nhor[0]*nhor[1],hor);    

    /* specify output dimensions */
    nt1 = nt+1;
    /* get slowness squared */    
    nm = n[0]*n[1]*n[2];
    slow = sf_floatalloc(nm);
    sf_floatread(slow,nm,vel);    
    for (int i = 0; i < nm; i++)
    {
        v0 = slow[i];
        slow[i] = velocity? 1./(v0*v0): v0*v0;
    }
    /* initialize ray tracing object */
    rt = raytrace_init (ndim, sym, nt, dt, n, o, d, slow, order);
    free (slow);
    traj = sf_floatalloc2 (sym? ndim: 2*ndim,nt1);
    for( is = 0; is < nshot; is++) { /* loop over shots */
        /* initialize angles */
        for (ir = 0; ir < nr; ir++) {
            a[ir] = a0+da*ir;
            b[ir] = b0+db*ir;
        }
        if (verb)
            sf_warning ("Shooting from z=%g x=%g y=%g", s[is][0], s[is][1], s[is][2]);
        for (ir = 0; ir < nr; ir++) { /* loop over rays */
            /* initialize position */
            x[0] = s[is][0]; 
            x[1] = s[is][1];
            x[2] = s[is][2];
            p[0]=a[ir];
            p[1]=b[ir];

            // p[0] = -cosf(a[ir]);                //a: incline angle
            // p[1] =  sinf(a[ir])*cosf(b[ir]);    //b: azimuth angle
            p[2] =  sinf(a[ir])*sinf(b[ir]);
            it = mtrace_ray (rt, x, p, traj, horizon, nhor, dhor, ohor);
            if (it < 0) it = -it; /* keep side-exiting rays */
            for (i=0; i < nt1; i++) {
                if (0==it || it > i) {
                    fprintf (fp,"%10.5f %10.5f %10.5f\n",traj[i][0],traj[i][1],traj[i][2]);
                } else {
                    fprintf (fp,"%10.5f %10.5f %10.5f\n",traj[it][0],traj[it][1],traj[it][2]);
                    break;
                }
            }
        }
    }
    // sf_warning("%f",h[487][0]);

    sf_warning("Build rays.txt:");
    if (verb)
        sf_warning (".");
    exit (0);
}
