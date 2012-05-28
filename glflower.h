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
typedef unsigned int glcolor_id;

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
} glpetal;

typedef struct _glcircle {
    glvec3 c;
    glfloat r;
} glcircle;

typedef struct _glflower_obj {
    glangle fa;
    glvec3 ps;
    glvec3 cs;
    glvec3 fm;
    glvec3 pm;
    glcolor_id cf;
} glflower_obj;

typedef struct _glflower_context {
    glprograme pprg;
    glvbo  pvbo;
    glvao  pvao;
    size_t pbsize;
    gllocation pvloc_ver;
    gllocation pvloc_mat_s;
    gllocation pvloc_mat_r;
    gllocation pvloc_mat_mf;
    gllocation pvloc_mat_mp;
    gllocation pfloc_lgc;
	gllocation pfloc_lgs;
    
    glprograme cprg;
    glvbo  cvbo;
    glvao  cvao;
    size_t cbsize;
    gllocation cvloc_ver;
    gllocation cvloc_mat_s;
    gllocation cvloc_mat_m;
    gllocation cfloc_rgs;
    gllocation cfloc_rgc;
} glflower_context;

typedef struct _glflower {
    glfloat sp;
    glfloat sl;
    glfloat sc;
    glvec3 p;
    glangle a;
    glcolor_id cf;
} glflower;

typedef struct _glbranch_context {
    glprograme bprg;
    gllocation bvloc_ver;
    gllocation bvloc_mat;
    gllocation bfloc_cor;
} glbranch_context;

typedef struct _glbranch_obj {
    glvbo bvbo;
    size_t bbsize;
    glmat4 m;
} glbranch_obj;

typedef struct _glbranch {
    glfloat rx;
    glfloat ry;
    glfloat wmax;
    glfloat wmin;
    glangle al;
    glfloat z;
    glangle ar;
} glbranch;

void glinit_tree_context();
void glrender_tree_context();

#endif //__GLFLOWER_H__
