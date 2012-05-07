/*
 * =====================================================================================
 *
 *       Filename:  flower.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2012 00:43:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WarTalker (), wartalker@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "glflower.h"

#ifdef _WIN32
#pragma comment(lib, "glew32.lib")
#endif //_WIN32

#define STEP 10

extern const double PI;

const glfloat g_view_max = 400;
static float g_view_aspect = 2.0f/g_view_max;

static const glchar * petal_vshader = 
{
    "#version 120\n\n" \
    "attribute vec3 vertexs;\n" \
    "attribute vec3 colors;\n" \
    "uniform mat4 matrix_s;\n" \
    "uniform mat4 matrix_m;\n" \
    "uniform mat4 matrix_r;\n" \
    "invariant varying vec4 vcolor;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tgl_Position =  matrix_m * matrix_r * matrix_s * vec4(vertexs, 1.0);\n" \
    "\tvcolor = vec4(colors, 1.0f);\n" \
    "}"
};

static const glchar * petal_fshader =
{
    "#version 120\n\n" \
    "invariant varying vec4 vcolor;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tgl_FragColor = vcolor;\n" \
    "}"
};

static const glchar * center_vshader = 
{
    "#version 120\n\n" \
    "attribute vec3 vertexs;\n" \
    "invariant varying vec2 ver_gradient_position;\n" \
    "uniform mat4 matrix_s;\n\n" \
    "uniform mat4 matrix_m;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tgl_Position = matrix_m * matrix_s * vec4(vertexs, 1.0f);\n" \
    "\tver_gradient_position = (matrix_m * matrix_s * vec4(vertexs, 1.0f)).xy;\n" \
    "}"
};

static const glchar * center_fshader =
{
    "#version 120\n\n" \
    "uniform vec3 radial_gradient_color[3];\n" \
    "uniform float radial_gradient_stop;\n" \
    "uniform float radial_gradient_r;\n" \
    "uniform vec2 radial_gradient_center;\n" \
    "invariant varying vec2 ver_gradient_position;\n" \
    "void main()\n" \
    "{\n" \
    "\tfloat gp = distance(ver_gradient_position, radial_gradient_center)/radial_gradient_r;\n" \
    "\tif (gp >= radial_gradient_stop)\n" \
    "\t\tgl_FragColor = vec4(mix(radial_gradient_color[1], radial_gradient_color[0], gp), 1.0f);\n" \
    "\telse\n" \
    "\t\tgl_FragColor = vec4(mix(radial_gradient_color[2], radial_gradient_color[1], gp), 0.6f);\n" \
    "}"
};

static glflower_context gfcontext = {{0}};

static void
glset_arc(glarc * parc, glvec3 * pp, glfloat r, unsigned int from, unsigned int to) {
    glassign_vec3(&(parc->c), pp);
    parc->r = r;
    parc->from = from;
    parc->to = to;
}

static void
glset_center(glcircle * pc, glvec3 * pp, glfloat r) {
    glassign_vec3(&(pc->c), pp);
    pc->r = r;
}

static void
glset_petal(glpetal * pp, glfloat m, glfloat r1, glfloat r2, glfloat z, glvec3 * c1, glvec3 * c2) {
    pp->m = m;
    pp->r1 = r1;
    pp->r2 = r2;
    pp->z = z;
    glassign_vec3(&(pp->gradient_color[0]), c1);
    glassign_vec3(&(pp->gradient_color[1]), c2);
}

static void
push_arc(glvector ** pv, glarc * pa, glcolor * pc) {
    glvec3 pt = {0};
	unsigned int from = pa->from;
	unsigned int to = pa->to;

    int step = (to < from) ? -STEP : STEP;
    int ang;

    for (ang = from; ang != to; ang += step) {
        pt.x = (glfloat) (pa->c.x + pa->r * cos(PI*ang/180.0));
        pt.y = (glfloat) (pa->c.y + pa->r * sin(PI*ang/180.0));
        pt.z = (glfloat) pa->c.z;
        if (NULL != pc) 
            glpush_2vec3(pv, &pt, pc);
        else 
            glpush_vec3(pv, &pt);
    }
    pt.x = (glfloat) (pa->c.x + pa->r * cos(PI*ang/180.0));
    pt.y = (glfloat) (pa->c.y + pa->r * sin(PI*ang/180.0));
    pt.z = (glfloat) pa->c.z;
    if (NULL != pc) 
        glpush_2vec3(pv, &pt, pc);
    else 
        glpush_vec3(pv, &pt);
}

static void
push_center_obj(glvector * * pv, glcircle * pc) {
    glvec3 p = {0};
    glarc a = {{0}};
    
    glpush_vec3(pv, &(pc->c));

    glassign_vec3(&p, &(pc->c));
    glset_arc(&a, &(pc->c), pc->r, 0, 360);
    push_arc(pv, &a, NULL);
}

static void
create_center_vbo() {
    glvector * v = NULL;
    glvec3 p = {0.0f, 0.0f, 0.0f};
    glcircle c = {{0}};

	glset_center(&c, &p, 1.0f);
    v = glalloc_vector(0);
    push_center_obj(&v, &c);
    gfcontext.cbsize = glget_vector_size(v);

    glGenBuffers(1, &(gfcontext.cvbo));
    glBindBuffer(GL_ARRAY_BUFFER, gfcontext.cvbo);
    glBufferData(GL_ARRAY_BUFFER, gfcontext.cbsize*sizeof(glfloat), glget_vector_array(v), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfree_vector(v);
}

static void
push_petal_obj(glvector * * pv, glpetal * pp) {
    glvec3 p = {0};
    glarc a = {{0}};

    glset_vec3(&p, 0, 0, 0);
    glpush_2vec3(pv, &p, &(pp->gradient_color[0]));

    glset_vec3(&p, pp->r1, pp->m - pp->r1, pp->z);
    glset_arc(&a, &p, pp->r1, 180, 90);
    push_arc(pv, &a, &(pp->gradient_color[1]));

    glset_vec3(&p, pp->m - pp->r2, pp->m - pp->r2, pp->z);
    glset_arc(&a, &p, pp->r2, 90, 0);
    push_arc(pv, &a, &(pp->gradient_color[1]));

    glset_vec3(&p, pp->m - pp->r1, pp->r1, pp->z);
    glset_arc(&a, &p, pp->r1, 360, 270);
    push_arc(pv, &a, &(pp->gradient_color[1]));

    glset_vec3(&p, 0, 0, 0);
    glpush_2vec3(pv, &p, &(pp->gradient_color[0]));
}

static glcolor cs = {185.0f/255.0f, 51.0f/255.0f, 76.0f/255.0f};
static glcolor ce = {240.0f/255.0f, 166.0f/255.0f, 199.0f/255.0f};

static void
create_petal_vbo() {
    glvector * v = NULL;
    glpetal petal = {0};

    v = glalloc_vector(0);
    glset_petal(&petal, 0.50f,0.25f*0.50f, 0.50f*0.50f, 0.0f, &cs, &ce); 
    push_petal_obj(&v, &petal);
//	glprint_vector(v);
    gfcontext.pbsize = glget_vector_size(v);

    glGenBuffers(1, &(gfcontext.pvbo));
    glBindBuffer(GL_ARRAY_BUFFER, gfcontext.pvbo);
    glBufferData(GL_ARRAY_BUFFER, gfcontext.pbsize*sizeof(glfloat), glget_vector_array(v), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfree_vector(v);
}

void
glinit_flower_context() {
    bool status;

    memset(&gfcontext, 0, sizeof(gfcontext));
    status = glcreate_programe(&(gfcontext.pprg), petal_vshader, petal_fshader);
    if (!status) {
        fprintf(stdout, "compile petal programe error!\n");
        exit(EXIT_FAILURE);
    }
    create_petal_vbo();

    status = glcreate_programe(&(gfcontext.cprg), center_vshader, center_fshader);
    if (!status) {
        fprintf(stdout, "compile center programe error!\n");
        exit(EXIT_FAILURE);
    }
    create_center_vbo();
}

static void
gltransform_flower(glflower * dst, glflower * src) {
    dst->circle.r = src->circle.r*g_view_aspect;
    dst->circle.c.x = 1.0f - src->circle.c.x*g_view_aspect; 
    dst->circle.c.y = src->circle.c.y*g_view_aspect - 1.0f;
    dst->circle.c.z = src->circle.c.z;

    dst->petal.m = src->petal.m * g_view_aspect;
    dst->petal.r1 = src->petal.r1 * g_view_aspect;
    dst->petal.r2 = src->petal.r2 * g_view_aspect;
    dst->petal.z  = src->petal.z;
    dst->petal.l = src->petal.l * g_view_aspect;
}

static void 
gldraw_petal(glflower * pf) {
    gllocation pvloc_ver;
    gllocation pvloc_cor;
    gllocation pvloc_mat_s;
    gllocation pvloc_mat_r;
    gllocation pvloc_mat_m;
    int ang;
    glmat4 * m_s;
    glmat4 * m_r;
    glmat4 * m_m;
    glflower pdst = {{{0}}};
    glvec3 v = {0};


    glUseProgram(gfcontext.pprg.pid);
    pvloc_ver = glGetAttribLocation(gfcontext.pprg.pid, "vertexs");
    pvloc_cor = glGetAttribLocation(gfcontext.pprg.pid, "colors");
    pvloc_mat_s = glGetUniformLocation(gfcontext.pprg.pid, "matrix_s");
    pvloc_mat_r = glGetUniformLocation(gfcontext.pprg.pid, "matrix_r");
    pvloc_mat_m = glGetUniformLocation(gfcontext.pprg.pid, "matrix_m");

    glGenVertexArrays(1, &(gfcontext.pvao));
    glBindVertexArray(gfcontext.pvao);

    glBindBuffer(GL_ARRAY_BUFFER, gfcontext.pvbo);
    glVertexAttribPointer(pvloc_ver, 3, GL_FLOAT, GL_FALSE, 6*sizeof(glfloat), 0);
    glVertexAttribPointer(pvloc_cor, 3, GL_FLOAT, GL_FALSE, 6*sizeof(glfloat), (const void *)(3*sizeof(glfloat)));
    glEnableVertexAttribArray(pvloc_ver);
    glEnableVertexAttribArray(pvloc_cor);

    gltransform_flower(&pdst, pf);
    for (ang = pf->petal.a; ang < 360; ang += 90) {
        m_s = glcreate_identify_mat4();
        m_r = glcreate_identify_mat4();
        m_m = glcreate_identify_mat4();

        glscale_mat4(m_s, pdst.petal.m);
        glUniformMatrix4fv(pvloc_mat_s, 1, true,  glget_mat4_array(m_s));

        v.x = pdst.petal.l * cos(glang_transform(ang + 45));
        v.y = pdst.petal.l * sin(glang_transform(ang + 45));
        v.z = 0.0f;
        glmove_mat4(m_m, &v);
        glmove_mat4(m_m, &(pdst.circle.c));
        glUniformMatrix4fv(pvloc_mat_m, 1, true,  glget_mat4_array(m_m));

        glrotatez_mat4(m_r, ang);
        glUniformMatrix4fv(pvloc_mat_r, 1, true,  glget_mat4_array(m_r));

        glDrawArrays(GL_TRIANGLE_FAN, 0, gfcontext.pbsize/6);

        glfree_mat4(m_s);
        glfree_mat4(m_r);
        glfree_mat4(m_m);
        m_s = NULL;
        m_r = NULL;
        m_m = NULL;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(pvloc_ver);
	glDisableVertexAttribArray(pvloc_cor);
    glUseProgram(0);
}

static void
gldraw_center(glflower * pf) {
    gllocation cvloc_ver;
    gllocation cvloc_mat_s;
    gllocation cvloc_mat_m;
    gllocation cfloc_rgs;
    gllocation cfloc_rgc;
    gllocation cfloc_rgp;
    gllocation cfloc_rgr;
    glconst_float gcolor[] = {1.0f, 1.0f, 204.0f/255.0f, 245.0f/255.0f, 209.0f/255.0f, 211.0f/255.0f, 1.0f, 1.0f, 204.0f/255.0f};
    glfloat rgstop = 0.30f;
    glflower fdst = {{{0}}};
    glmat4 * m_s = glcreate_identify_mat4();
    glmat4 * m_m = glcreate_identify_mat4();
    glpid pid = gfcontext.cprg.pid;

    glUseProgram(pid);

    cvloc_ver = glGetAttribLocation(pid, "vertexs");
    cvloc_mat_s = glGetUniformLocation(pid, "matrix_s");
    cvloc_mat_m = glGetUniformLocation(pid, "matrix_m");
    cfloc_rgs = glGetUniformLocation(pid, "radial_gradient_stop");
    cfloc_rgr = glGetUniformLocation(pid, "radial_gradient_r");
    cfloc_rgc = glGetUniformLocation(pid, "radial_gradient_color");
    cfloc_rgp = glGetUniformLocation(pid, "radial_gradient_center");

    gltransform_flower(&fdst, pf);

    glUniform1f(cfloc_rgs, rgstop);
    glUniform1f(cfloc_rgr, fdst.circle.r);
    glUniform3fv(cfloc_rgc, 3, (const GLfloat *)gcolor);
    glUniform2f(cfloc_rgp, fdst.circle.c.x, fdst.circle.c.y);
    glscale_mat4(m_s, fdst.circle.r);
    glUniformMatrix4fv(cvloc_mat_s, 1, true,  glget_mat4_array(m_s));
    glmove_mat4(m_m, &(fdst.circle.c));
    glUniformMatrix4fv(cvloc_mat_m, 1, true,  glget_mat4_array(m_m));

    glGenVertexArrays(1, &(gfcontext.cvao));
    glBindVertexArray(gfcontext.cvao);

    glBindBuffer(GL_ARRAY_BUFFER, gfcontext.cvbo);
    glVertexAttribPointer(cvloc_ver, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(cvloc_ver);

    glDrawArrays(GL_TRIANGLE_FAN, 0, gfcontext.pbsize/3);

    glDisableVertexAttribArray(cvloc_ver);
    glUseProgram(0);
    glfree_mat4(m_s);
    glfree_mat4(m_m);
    m_s = NULL;
    m_m = NULL;
}

void 
glrender_flower_context() {
    glflower f1;

    glset_vec3(&(f1.circle.c), 250.0f, 150.0f, 0.0f);
    f1.circle.r = 6;
    f1.petal.m = 36;
    f1.petal.r1 = 10;
    f1.petal.r2 = 20;
    f1.petal.l = 1;
    f1.petal.a = rand() % 90;
    glassign_vec3(&(f1.petal.gradient_color[0]), &cs);
    glassign_vec3(&(f1.petal.gradient_color[1]), &ce);

    gldraw_petal(&f1);
    gldraw_center(&f1);
}
