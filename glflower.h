/*
 * =====================================================================================
 *
 *       Filename:  flower.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2012 00:39:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: WarTalker
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __GLFLOWER_H__
#define __GLFLOWER_H__

#include "glutil.h"

typedef glvec3 glcolor;
typedef unsigned int glcorlor_id;

typedef struct _glarc {
    glvec3 c;
    glfloat r;
    unsigned int from;
    unsigned int to;
} glarc;


typedef struct _glpetal {
    glfloat m;
    glfloat r1;
    glfloat r2;
    glfloat z;
    glfloat l;
    glvec3 gradient_color[2];
} glpetal;

typedef struct _glcircle {
    glvec3 c;
    glfloat r;
    glvec3 gradient_color[3];
} glcircle;

typedef struct _glflower_obj {
    glangle fa;
    glfloat pscalx;
    glfloat pscaly;
    glfloat cscalx;
    glfloat cscaly;
    glfloat lscalx;
    glfloat lscaly;
    glfloat mx;
    glfloat my;
    glcolor_id cp;
    glcolor_id cc;
} glflower_obj;

typedef struct _glflower_context {
    glprograme pprg;
    glvbo  pvbo;
    glvao  pvao;
    size_t pbsize;
    gllocation pvloc_ver;
    gllocation pvloc_cor;
    gllocation pvloc_mat_s;
    gllocation pvloc_mat_r;
    
    glprograme cprg;
    glvbo  cvbo;
    glvao  cvao;
    size_t cbsize;
    gllocation cvloc_ver;
    gllocation cvloc_mat_s;
    gllocation cvloc_mat_m;
    gllocation cfloc_rgs;
    gllocation cfloc_rgc;
    gllocation cfloc_rgp;
    gllocation cfloc_rgr;
} glflower_context;

typedef struct _glflower {
    glfloat sp;
    glfloat sl;
    glfloat sc;
    glvec3 p;
    glcolor_id cp;
    glcolor_id cc;
} glflower;

void glinit_flower_context();
void glrender_flower_context();

#endif //__GLFLOWER_H__
