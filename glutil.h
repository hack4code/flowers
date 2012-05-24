/*
 * =====================================================================================
 *
 *       Filename:  glutil.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2012 23:53:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WarTalker
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __GLUTIL_H__
#define __GLUTIL_H__

#include <stdlib.h>

#if defined(__APPLE_CC__)
    #include <stdbool.h>
    #include <OpenGL/gl3.h>
#endif //__APPLE_CC__


#ifdef _WIN32
    #include <GL/glew.h>
    typedef unsigned char bool;
    #define true 1
    #define false 0
#endif //_WIN32


typedef GLfloat glfloat;
typedef GLuint glpid;
typedef GLuint glshader;
typedef GLuint glshader_type;
typedef GLint glbool;
typedef char glchar;
typedef GLuint glvao;
typedef GLuint glvbo;
typedef GLint gllocation;
typedef const GLfloat glconst_float;
typedef int glangle;

typedef struct _glvec3 {
    glfloat x;
    glfloat y;
    glfloat z;
} glvec3;

typedef struct _glvec4 {
    glfloat x;
    glfloat y;
    glfloat z;
    glfloat w;
} glvec4;

typedef struct _glmat4 {
    glfloat vecs[16];
} glmat4;

typedef struct _glvector {
    size_t size;
    size_t total;
    glfloat vec[0];
} glvector;

typedef struct _glprograme {
    glpid pid;
    glshader vid;
    glshader fid;
} glprograme;

void glset_vec3(glvec3 *, glfloat, glfloat, glfloat);
void glassign_vec3(glvec3 *, glvec3 *);

glvector * glalloc_vector();
void glfree_vector(glvector *);
size_t glget_vector_size(glvector *);
glfloat * glget_vector_array(glvector *);
void glpush_vector(glvector * *, glfloat);
void glpush_vec3(glvector * *, glvec3 *);
void glpush_2vec3(glvector * *, glvec3 *, glvec3 *);
void glprint_vector(glvector * v);

glprograme * glalloc_programe();
bool glcreate_programe(glprograme *, const glchar *, const glchar *);
void glfree_programe(glprograme *);


glmat4 * glalloc_mat4();
void glfree_mat4(glmat4 *);
void glset_identify_mat4(glmat4 *);
glmat4 * glcreate_identify_mat4();
void glscale_mat4(glmat4 *, glvec3 *);
void glrotatez_mat4(glmat4 *, glangle);
void glmove_mat4(glmat4 *, glvec3 *);
void glmvtrans_mat4(glmat4 *, glfloat, glfloat);
glfloat * glget_mat4_array(glmat4 *);
void glmutiply_mat4(glmat4 *, glmat4 *);
void glassign_mat4(glmat4 *, glmat4 *);

glfloat glang_transform(unsigned int);



#endif //__GLUTIL_H__
