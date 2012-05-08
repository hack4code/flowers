/*
 * =====================================================================================
 *
 *       Filename:  glutil.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2012 23:55:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WarTalker 
 *        Company:  
 *
 * =====================================================================================
 */



#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef _WIN32
#include <GL/glew.h>
#endif //_WIN32

#include <stdio.h>
#include "glutil.h"

const double PI = 3.14159265358979323846;

//vector function
glvector * 
glalloc_vector(size_t n) {
    size_t t;
    glvector * v;

    n = (n <= 36) ? 36 : n;
    t = sizeof(glvector) + n;
    v = (glvector *) malloc(t);
    memset((void *)v, 0, t);
    v->total = n;
    return v;
}

void 
glfree_vector(glvector * v) {
    if (NULL != v) free(v);
}

size_t 
glget_vector_size(glvector * v) {
    return v->size;
}

glfloat *
glget_vector_array(glvector * v) {
    return v->vec; 
}

void
glpush_vector(glvector * * pv, glfloat e) {
    glvector * t;
    size_t size = (*pv)->size;
    size_t total = (*pv)->total;

    if (size*sizeof(glfloat) >= total) {
        t = glalloc_vector(2*total);
        t->size = size;
        t->total = 2*total;
        memcpy(t->vec, (*pv)->vec, total);
        glfree_vector(*pv);
        *pv = t;
        t = NULL;
    }

    (*pv)->vec[size] = e;
    (*pv)->size++;
}

void 
glpush_vec3(glvector * * pv, glvec3 * pp) {
    glpush_vector(pv, pp->x);
    glpush_vector(pv, pp->y);
    glpush_vector(pv, pp->z);
}

void
glpush_2vec3(glvector * * pv, glvec3 * pp1, glvec3 * pp2) {
	glpush_vec3(pv, pp1);
	glpush_vec3(pv, pp2);
}

void
glprint_vector(glvector * v) {
    size_t i;
    for (i = 0; i < glget_vector_size(v); i += 3) {
        fprintf(stdout, "%f %f %f\n", v->vec[i], v->vec[i+1], v->vec[i+2]);
    }
}

//shader compile function
static glshader
glcreate_shader(const glchar * src, glshader_type type) {
    glshader shid;
    glbool status;

    GLint n;
    char * log;

    shid = glCreateShader(type);
    glShaderSource(shid, 1, (const GLchar**)&src, NULL);
    glCompileShader(shid);
    
    glGetShaderiv(shid, GL_COMPILE_STATUS, &status);    
    if (!status) {
        glGetShaderiv(shid, GL_INFO_LOG_LENGTH, &n);
        log = (char *) malloc(n);
        glGetShaderInfoLog(shid, n, NULL, log);
        fprintf(stdout, "%s\n\n%s", log, src);
        free(log);
    }

    return (!status) ? 0 : shid;
}

glprograme *
glalloc_programe() {
    glprograme * p = (glprograme *) malloc(sizeof(*p));
    if (p) memset(p, 0, sizeof(*p));
    return p;
}

bool 
glcreate_programe(glprograme *  p, const glchar * vsource, const glchar * fsource) {
    glshader vid, fid;
    glpid pid;
    glbool status;

    vid = glcreate_shader(vsource, GL_VERTEX_SHADER);
    if (0 == vid) return false;

    fid = glcreate_shader(fsource, GL_FRAGMENT_SHADER);
    if (0 == fid) return false;

    pid = glCreateProgram();
    glAttachShader(pid, vid);
    glAttachShader(pid, fid);
    glLinkProgram(pid);

    glGetProgramiv(pid, GL_LINK_STATUS, &status);

    if (!status) return false;

    p->pid = pid;
    p->vid = vid;
    p->fid = fid;

    return true;
}

void 
glfree_programe(glprograme * p) {
    if (NULL == p) return;

    glUseProgram(0);
    glDetachShader(p->pid, p->vid);
    glDetachShader(p->pid, p->fid);
    glDeleteShader(p->vid);
    glDeleteShader(p->fid);
    glDeleteProgram(p->pid);

    memset(p, 0, sizeof(*p));
}

//vec3 functions
void
glset_vec3(glvec3 * pp, glfloat x, glfloat y, glfloat z) {
    pp->x = x;
    pp->y = y;
    pp->z = z;
}

void
glassign_vec3(glvec3 * pdes, glvec3 * psrc) {
    glset_vec3(pdes, psrc->x, psrc->y, psrc->z);
}


//matrix functions

glmat4 *
glalloc_mat4() {
    glmat4 * m;

    m = (glmat4 *) malloc(sizeof(*m));
    memset((void *)m, 0, sizeof(*m));
    return m;
}

void
glfree_mat4(glmat4 * m) {
    free((void *)m);
}

void
glset_identify_mat4(glmat4 * m) {
    memset(m->vecs, 0, sizeof(m->vecs));
    m->vecs[0] = 1.0f;
    m->vecs[5] = 1.0f;
    m->vecs[10] = 1.0f;
    m->vecs[15] = 1.0f;
}

glmat4 *
glcreate_identify_mat4() {
    glmat4 * m;

    m = glalloc_mat4();
    glset_identify_mat4(m);
    return m;
}

void 
glscale_mat4(glmat4 * m, glfloat s) {
    m->vecs[0] *= s;
    m->vecs[5] *= s;
    m->vecs[10] *= s;
}

void
glrotatez_mat4(glmat4 * m, glangle ang) {
    glfloat a = glang_transform(ang);
    glfloat sina = (glfloat) sin(a);
    glfloat cosa = (glfloat) cos(a);

    m->vecs[0] = cosa;
    m->vecs[1] = -sina;
    m->vecs[4] = sina;
    m->vecs[5] = cosa;
}

void
glmove_mat4(glmat4 * m, glvec3 * v) {
    m->vecs[3] += v->x;
    m->vecs[7] += v->y;
    m->vecs[11] += v->z;
}

void
glmvtrans_mat4(glmat4 * m, glfloat w, glfloat h) {
    m->vecs[0] /= w;
    m->vecs[0] -= 0.50f;
    m->vecs[5] /= h;
    m->vecs[5] -= 0.50f;
}

glfloat * 
glget_mat4_array(glmat4 * m) {
    return m->vecs;
}


//
glfloat glang_transform(unsigned int a) {
    return (glfloat)(a*PI/180.f);
}
