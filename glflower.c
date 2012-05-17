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

glfloat g_screen_width = 400.0f;
glfloat g_screen_height = 400.0f;

//colors for center
static glconst_float g_center_colors[][9] = {
                {1.0f, 1.0f, 204.0f/255.0f, 245.0f/255.0f, 169.0f/255.0f, 205.0f/255.0f, 1.0f, 1.0f, 204.0f/255.0f}
};

glfloat g_center_stop[][1] = {
                                {0.30f}
};


//colors for petal
glconst_float g_petal_colors[][6] = {
    {185.0f/255.0f, 51.0f/255.0f, 76.0f/255.0f, 240.0f/255.0f, 166.0f/255.0f, 199.0f/255.0f}
};

static const glchar * petal_vshader = 
{
    "#version 120\n\n" \
    "attribute vec3 vertexs;\n" \
    "uniform mat4 matrix_s;\n" \
    "uniform mat4 matrix_m;\n" \
    "uniform mat4 matrix_r;\n" \
    "invariant varying vec2 vertex_pos;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tgl_Position =  matrix_m * matrix_r * matrix_s * vec4(vertexs, 1.0);\n" \
    "\tvertex_pos = vertexs.xy;\n" \
    "}"
};

static const glchar * petal_fshader =
{
    "#version 120\n\n" \
    "uniform vec3 liner_gradient_colors[2];\n" \
    "invariant varying vec2 vertex_pos;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tfloat gp;\n" \
    "\tgp = length(vertex_pos) * cos(atan(vertex_pos.y, vertex_pos.x) - 3.1415926f/4.0f) / sqrt(2.0f);\n" \
    "\tgl_FragColor = mix(vec4(liner_gradient_colors[0], 1.0f), vec4(liner_gradient_colors[1], 1.0f), gp);\n" \
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
    "\tver_gradient_position = vertexs.xy;\n" \
    "}"
};

static const glchar * center_fshader =
{
    "#version 120\n\n" \
    "uniform vec3 radial_gradient_color[3];\n" \
    "uniform float radial_gradient_stop;\n" \
    "invariant varying vec2 ver_gradient_position;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tfloat gp = length(ver_gradient_position);\n" \
    "\tif (gp >= radial_gradient_stop)\n" \
    "\t\tgl_FragColor = mix(vec4(radial_gradient_color[1], 1.0f), vec4(radial_gradient_color[0], 1.0f), gp);\n" \
    "\telse\n" \
    "\t\tgl_FragColor = mix(vec4(radial_gradient_color[2], 1.0f), vec4(radial_gradient_color[1], 1.0f), gp);\n" \
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
glset_circle(glcircle * pc, glvec3 * pv, glfloat r) {
    glassign_vec3(&(pc->c), pv);
    pc->r = r;
}

static void
glset_petal(glpetal * pp, glfloat m, glfloat r1, glfloat r2, glfloat z) {
    pp->m = m;
    pp->r1 = r1;
    pp->r2 = r2;
    pp->z = z;
}

static void
push_arc(glvector ** pv, glarc * pa) {
    glvec3 pt = {0};
	unsigned int from = pa->from;
	unsigned int to = pa->to;

    int step = (to < from) ? -STEP : STEP;
    int ang = from;

    while (true) {
        pt.x = (glfloat) (pa->c.x + pa->r * cos(PI*ang/180.0));
        pt.y = (glfloat) (pa->c.y + pa->r * sin(PI*ang/180.0));
        pt.z = (glfloat) pa->c.z;
        glpush_vec3(pv, &pt);
        if (ang == to)
            break;
        else
            ang += step;
    }
}

static void
push_center_obj(glvector * * pv, glcircle * pc) {
    glvec3 p = {0};
    glarc a = {{0}};
    
    glpush_vec3(pv, &(pc->c));

    glassign_vec3(&p, &(pc->c));
    glset_arc(&a, &(pc->c), pc->r, 0, 360);
    push_arc(pv, &a);
}

static void
create_center_vbo() {
    glvector * v = NULL;
    glvec3 p = {0.0f, 0.0f, 0.0f};
    glcircle c = {{0}};

	glset_circle(&c, &p, 1.0f);
    v = glalloc_vector(0);
    push_center_obj(&v, &c);
//	glprint_vector(v);
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
    glpush_vec3(pv, &p);

    glset_vec3(&p, pp->r1, pp->m - pp->r1, pp->z);
    glset_arc(&a, &p, pp->r1, 180, 90);
    push_arc(pv, &a);

    glset_vec3(&p, pp->m - pp->r2, pp->m - pp->r2, pp->z);
    glset_arc(&a, &p, pp->r2, 90, 0);
    push_arc(pv, &a);

    glset_vec3(&p, pp->m - pp->r1, pp->r1, pp->z);
    glset_arc(&a, &p, pp->r1, 360, 270);
    push_arc(pv, &a);

    glset_vec3(&p, 0, 0, 0);
    glpush_vec3(pv, &p);
}


static void
create_petal_vbo() {
    glvector * v = NULL;
    glpetal petal = {0};

    v = glalloc_vector(0);
    glset_petal(&petal, 1.0f,0.25f*1.0f, 0.50f*1.0f, 0.0f); 
    push_petal_obj(&v, &petal);
	glprint_vector(v);
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

    glUseProgram(gfcontext.pprg.pid);
    gfcontext.pvloc_ver = glGetAttribLocation(gfcontext.pprg.pid, "vertexs");
    gfcontext.pvloc_mat_s = glGetUniformLocation(gfcontext.pprg.pid, "matrix_s");
    gfcontext.pvloc_mat_r = glGetUniformLocation(gfcontext.pprg.pid, "matrix_r");
    gfcontext.pvloc_mat_m = glGetUniformLocation(gfcontext.pprg.pid, "matrix_m");
    gfcontext.pfloc_cor = glGetUniformLocation(gfcontext.pprg.pid, "liner_gradient_colors");
    glUseProgram(0);

    status = glcreate_programe(&(gfcontext.cprg), center_vshader, center_fshader);
    if (!status) {
        fprintf(stdout, "compile center programe error!\n");
        exit(EXIT_FAILURE);
    }
    create_center_vbo();

    glUseProgram(gfcontext.cprg.pid);
    gfcontext.cvloc_ver = glGetAttribLocation(gfcontext.cprg.pid, "vertexs");
    gfcontext.cvloc_mat_s = glGetUniformLocation(gfcontext.cprg.pid, "matrix_s");
    gfcontext.cvloc_mat_m = glGetUniformLocation(gfcontext.cprg.pid, "matrix_m");
    gfcontext.cfloc_rgs = glGetUniformLocation(gfcontext.cprg.pid, "radial_gradient_stop");
    gfcontext.cfloc_rgc = glGetUniformLocation(gfcontext.cprg.pid, "radial_gradient_color");
    glUseProgram(0);
}

static void 
gldraw_petal(glflower_obj * pf) {
    glmat4 * m_s;
    glmat4 * m_r;
    glmat4 * m_m;
    glvec3 v = {0};
    glangle ang;

    glUseProgram(gfcontext.pprg.pid);

    glGenVertexArrays(1, &(gfcontext.pvao));
    glBindVertexArray(gfcontext.pvao);

    glBindBuffer(GL_ARRAY_BUFFER, gfcontext.pvbo);
    glVertexAttribPointer(gfcontext.pvloc_ver, 3, GL_FLOAT, GL_FALSE, 3*sizeof(glfloat), 0);
    glEnableVertexAttribArray(gfcontext.pvloc_ver);

    for (ang = pf->fa; ang < 360; ang += 90) {
        m_s = glcreate_identify_mat4();
        m_r = glcreate_identify_mat4();
        m_m = glcreate_identify_mat4();

        glscale_mat4(m_s, &(pf->ps));
        glUniformMatrix4fv(gfcontext.pvloc_mat_s, 1, true,  glget_mat4_array(m_s));

        if ((270 <= ang && ang < 360) || (0 <= ang && ang < 90))
            v.x = pf->lm.x;
        else
            v.x = -pf->lm.x;

        if (0 <= ang && 180 > ang)
            v.y = pf->lm.y;
        else
            v.y = -pf->lm.y;

        v.z = 0.0f;

        glmove_mat4(m_m, &v);
        glmove_mat4(m_m, &(pf->fm));
        glUniformMatrix4fv(gfcontext.pvloc_mat_m, 1, true,  glget_mat4_array(m_m));

        glrotatez_mat4(m_r, ang);
        glUniformMatrix4fv(gfcontext.pvloc_mat_r, 1, true,  glget_mat4_array(m_r));

        glUniform3fv(gfcontext.pfloc_cor, 2, (const GLfloat *)g_petal_colors[pf->cf]);

        glDrawArrays(GL_TRIANGLE_FAN, 0, gfcontext.pbsize/3);

        glfree_mat4(m_s);
        glfree_mat4(m_r);
        glfree_mat4(m_m);
        m_s = NULL;
        m_r = NULL;
        m_m = NULL;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(gfcontext.pvloc_ver);
    glUseProgram(0);
}

static void
gldraw_center(glflower_obj * pf) {
    glmat4 * m_s = glcreate_identify_mat4();
    glmat4 * m_m = glcreate_identify_mat4();

    glUseProgram(gfcontext.cprg.pid);

    glUniform1f(gfcontext.cfloc_rgs, g_center_stop[pf->cf][0]);
    glUniform3fv(gfcontext.cfloc_rgc, 3, (const GLfloat *)g_center_colors[pf->cf]);

    glscale_mat4(m_s, &(pf->cs));
    glUniformMatrix4fv(gfcontext.cvloc_mat_s, 1, true,  glget_mat4_array(m_s));

    glmove_mat4(m_m, &(pf->fm));
    glUniformMatrix4fv(gfcontext.cvloc_mat_m, 1, true,  glget_mat4_array(m_m));

    glGenVertexArrays(1, &(gfcontext.cvao));
    glBindVertexArray(gfcontext.cvao);

    glBindBuffer(GL_ARRAY_BUFFER, gfcontext.cvbo);
    glVertexAttribPointer(gfcontext.cvloc_ver, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(gfcontext.cvloc_ver);

    glDrawArrays(GL_TRIANGLE_FAN, 0, gfcontext.cbsize/3);

    glDisableVertexAttribArray(gfcontext.cvloc_ver);
    glUseProgram(0);
    glfree_mat4(m_s);
    glfree_mat4(m_m);
    m_s = NULL;
    m_m = NULL;
}

void
set_flower_obj(glflower_obj * fo, glflower * fs) {
    glfloat sx = 2.0f/g_screen_width;
    glfloat sy = 2.0f/g_screen_height;

    fo->ps.x = sx * fs->sp;
    fo->ps.y = sy * fs->sp;
    fo->ps.z = 1.0f;

    fo->cs.x = sx * fs->sc;
    fo->cs.y = sy * fs->sc;
    fo->cs.z = 1.0f;

    fo->fm.x = 1.0f - fs->p.x * sx;
    fo->fm.y = fs->p.y * sy - 1.0f;
    fo->fm.z = 0.0f;

    fo->lm.x = sx * fs->sl;
    fo->lm.y = sy * fs->sl;
    fo->lm.z = 0.0f;

    fo->fa = fs->a;
    fo->cf = fs->cf;
}

void
glrender_flower_context() {
    glflower f;
    glflower_obj fo;

    f.sp = 15.0f;
    f.sl = 1.0f;
    f.sc = 8.0f;
    f.p.x = 250.0f;
    f.p.y = 150.0f;
    f.p.z = 0.0f;
    f.cf = 0;
    f.a = 0;

    set_flower_obj(&fo, &f);

	gldraw_petal(&fo);
    gldraw_center(&fo);   
}
