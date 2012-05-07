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

typedef struct _glpetal {
    glfloat m;
    glfloat r1;
    glfloat r2;
    glfloat z;
    glfloat l;
    glangle a;
    glvec3 gradient_color[2];
} glpetal;

typedef struct _glcircle {
    glvec3 c;
    glfloat r;
    glvec3 gradient_color[3];
} glcircle;

typedef struct _glarc {
    glvec3 c;
    glfloat r;
    unsigned int from;
    unsigned int to;
} glarc;

typedef struct _glflower {
    glcircle circle;
    glpetal petal;
    glvec3 v;
} glflower;

typedef struct _glflower_context {
    glprograme pprg;
    glvbo  pvbo;
    glvao  pvao;
    size_t pbsize;
    
    glprograme cprg;
    glvbo  cvbo;
    glvao  cvao;
    size_t cbsize;
} glflower_context;

void glinit_flower_context();
void glrender_flower_context();


#endif //__GLFLOWER_H__
