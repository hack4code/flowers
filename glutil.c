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

const glfloat PI = 3.14159265358979323846f;

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
glappend_vector(glvector * * pv, glvector * v) {
    glvector * t;
    size_t sdst = (*pv)->size;
    size_t ssrc = v->size;
    size_t tdst = (*pv)->total;

    size_t size = sdst + ssrc;

    if (size*sizeof(glfloat) >= tdst) {
        t = glalloc_vector(size*sizeof(glfloat));
        t->size = sdst;
        t->total = size*sizeof(glfloat);
        memcpy(t->vec, (*pv)->vec, tdst);
        glfree_vector(*pv);
        *pv = t;
        t = NULL;
    }

    memcpy((char *)((*pv)->vec) + sizeof(glfloat)*(*pv)->size, (char *)(v->vec), sizeof(glfloat)*v->size);
    (*pv)->size = size;
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
glscale_mat4(glmat4 * m, glvec3 * s) {
    m->vecs[0] *= s->x;
    m->vecs[5] *= s->y;
    m->vecs[10] *= s->z;
}

void glrotatefz_mat4(glmat4 * m, glfloat a) {
	glfloat sina = (glfloat) sin(a);
    glfloat cosa = (glfloat) cos(a);

    m->vecs[0] = cosa;
    m->vecs[1] = -sina;
    m->vecs[4] = sina;
    m->vecs[5] = cosa;
}

void
glrotatez_mat4(glmat4 * m, glangle ang) {
    glfloat a = glang_transform(ang);
	glrotatefz_mat4(m, a);
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

void
glassign_mat4(glmat4 * dest, glmat4 * src) {
    memcpy((void *)dest, (void *)src, sizeof(*dest));
}

void 
glmutiply_mat4(glmat4 * destm, glmat4 * srcm) {
    glfloat * dest = destm->vecs;
    glfloat * src = srcm->vecs;

    glfloat a11 = src[0];
    glfloat a12 = src[1];
    glfloat a13 = src[2];
    glfloat a14 = src[3];

    glfloat a21 = src[4];
    glfloat a22 = src[5];
    glfloat a23 = src[6];
    glfloat a24 = src[7];

    glfloat a31 = src[8];
    glfloat a32 = src[9];
    glfloat a33 = src[10];
    glfloat a34 = src[11];

    glfloat a41 = src[12];
    glfloat a42 = src[13];
    glfloat a43 = src[14];
    glfloat a44 = src[15];

    glfloat b11 = dest[0];
    glfloat b12 = dest[1];
    glfloat b13 = dest[2];
    glfloat b14 = dest[3];

    glfloat b21 = dest[4];
    glfloat b22 = dest[5];
    glfloat b23 = dest[6];
    glfloat b24 = dest[7];

    glfloat b31 = dest[8];
    glfloat b32 = dest[9];
    glfloat b33 = dest[10];
    glfloat b34 = dest[11];

    glfloat b41 = dest[12];
    glfloat b42 = dest[13];
    glfloat b43 = dest[14];
    glfloat b44 = dest[15];


    dest[0] = a11*b11 + a12*b21 + a13*b31 + a14*b41;
    dest[1] = a11*b12 + a12*b22 + a13*b32 + a14*b42;
    dest[2] = a11*b13 + a12*b23 + a13*b33 + a14*b43;
    dest[3] = a11*b14 + a12*b24 + a13*b34 + a14*b44;
    
    dest[4] = a21*b11 + a22*b21 + a23*b31 + a24*b41;
    dest[5] = a21*b12 + a22*b22 + a23*b32 + a24*b42;
    dest[6] = a21*b13 + a22*b23 + a23*b33 + a24*b43;
    dest[7] = a21*b14 + a22*b24 + a23*b34 + a24*b44;

    dest[8] = a31*b11 + a32*b21 + a33*b31 + a34*b41;
    dest[9] = a31*b12 + a32*b22 + a33*b32 + a34*b42;
    dest[10] = a31*b13 + a32*b23 + a33*b33 + a34*b43;
    dest[11] = a31*b14 + a32*b24 + a33*b34 + a34*b44;

    dest[12] = a41*b11 + a42*b21 + a43*b31 + a44*b41;
    dest[13] = a41*b12 + a42*b22 + a43*b32 + a44*b42;
    dest[14] = a41*b13 + a42*b23 + a43*b33 + a44*b43;
    dest[15] = a41*b14 + a42*b24 + a43*b34 + a44*b44;
}

//
glfloat glang_transform(unsigned int a) {
    return (glfloat)(a*PI/180.f);
}
