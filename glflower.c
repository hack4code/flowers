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

#define STEP 30

static const glfloat g_petal_depth = 0.0f;
static const glfloat g_center_depth = -0.01f;


extern const double PI;

glfloat g_screen_width = 400.0f;
glfloat g_screen_height = 300.0f;

//colors for center
static glfloat g_center_colors[][9] = {
                {1.0f, 1.0f, 204.0f/255.0f, 245.0f/255.0f, 169.0f/255.0f, 205.0f/255.0f, 1.0f, 1.0f, 204.0f/255.0f}
};

static glfloat g_center_stop[][3] = {
                {0.0f, 0.30f, 0.80f}
};


//colors for petal
static glfloat g_petal_colors[][6] = {
    {185.0f/255.0f, 51.0f/255.0f, 76.0f/255.0f, 240.0f/255.0f, 166.0f/255.0f, 199.0f/255.0f}
};

glfloat g_petal_stop[][3] = {
                {0.40f, 0.90f}
};

static const glchar * petal_vshader = 
{
    "#version 120\n\n" \
    "attribute vec3 vertexs;\n" \
    "uniform mat4 matrix_s;\n" \
    "uniform mat4 matrix_mf;\n" \
    "uniform mat4 matrix_mp;\n" \
    "uniform mat4 matrix_r;\n" \
    "invariant varying vec2 vertex_pos;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tgl_Position =  matrix_mf * matrix_s * matrix_r * matrix_mp * vec4(vertexs, 1.0);\n" \
    "\tvertex_pos = vertexs.xy;\n" \
    "}"
};

static const glchar * petal_fshader =
{
    "#version 120\n\n" \
    "uniform vec3 liner_gradient_colors[3];\n" \
    "uniform float liner_gradient_stop[3];\n" \
    "invariant varying vec2 vertex_pos;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tfloat gp;\n" \
    "\tgp = 1.0f - length(vertex_pos) * cos(atan(vertex_pos.y, vertex_pos.x) - 3.1415926f/4.0f)/sqrt(2.0f);\n" \
    "\tif (gp <= liner_gradient_stop[0])\n" \
    "\t\tgl_FragColor = vec4(liner_gradient_colors[1], 0.95f);\n" \
    "\telse if (liner_gradient_stop[0] <= gp && liner_gradient_stop[1] > gp)\n" \
    "\t\tgl_FragColor = mix(vec4(liner_gradient_colors[1], 1.0f), vec4(liner_gradient_colors[0], 1.0f), " \
                        "(gp - liner_gradient_stop[0])/(liner_gradient_stop[1] - liner_gradient_stop[0]));\n" \
    "\telse\n" \
    "\t\tgl_FragColor = vec4(liner_gradient_colors[0], 1.0f);\n" \
    "}"
};

static const glchar * center_vshader = 
{
    "#version 120\n\n" \
    "attribute vec3 vertexs;\n" \
    "uniform mat4 matrix_s;\n\n" \
    "uniform mat4 matrix_m;\n\n" \
    "invariant varying vec2 ver_gradient_position;\n" \
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
    "uniform float radial_gradient_stop[3];\n" \
    "invariant varying vec2 ver_gradient_position;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tfloat gp;\n" \
    "\tgp = length(ver_gradient_position)/0.50f;\n" \
    "\tif (gp <= radial_gradient_stop[0])\n" \
    "\t\tgl_FragColor = vec4(radial_gradient_color[0], 1.0f);\n" \
    "\telse if ((gp > radial_gradient_stop[0]) && (gp < radial_gradient_stop[1]))\n" \
    "\t\tgl_FragColor = mix(vec4(radial_gradient_color[0], 1.0f), vec4(radial_gradient_color[1], 1.0f)," \
                            "(gp - radial_gradient_stop[0])/(radial_gradient_stop[1] - radial_gradient_stop[0]));\n" \
    "\telse if ((gp >= radial_gradient_stop[1]) && (gp < radial_gradient_stop[2]))\n" \
    "\t\tgl_FragColor = mix(vec4(radial_gradient_color[1], 1.0f), vec4(radial_gradient_color[2], 1.0f), " \
                            "(gp - radial_gradient_stop[1])/(radial_gradient_stop[2]-radial_gradient_stop[1]));\n" \
    "\telse\n" \
    "\t\tgl_FragColor = vec4(radial_gradient_color[2], 1.0f);\n" \
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
push_arc(glvector ** pv, glarc * pa, int step) {
    unsigned int ang;
    glvec3 pt;

    for (ang = pa->from; ang <= pa->to; ang += step)  {
        pt.x = (glfloat) (pa->c.x + pa->r * cos(PI*ang/180.0f));
        pt.y = (glfloat) (pa->c.y + pa->r * sin(PI*ang/180.0f));
        pt.z = (glfloat) pa->c.z;
        glpush_vec3(pv, &pt);
    }
}

static void
push_center_obj(glvector * * pv, glcircle * pc) {
    glvec3 p;
    glarc a;
    
    glpush_vec3(pv, &(pc->c));

    glassign_vec3(&p, &(pc->c));
    glset_arc(&a, &(pc->c), pc->r, 0, 360);
    push_arc(pv, &a, STEP);
}

static void
create_center_vbo() {
    glvector * v;
    glcircle c;
    glvec3 p;

    glset_vec3(&p, 0.0f, 0.0f, g_center_depth);
	glset_circle(&c, &p, 0.5f);
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
    glvec3 p;
    glarc a;

    glset_vec3(&p, 0, 0, 0);
    glpush_vec3(pv, &p);

    glset_vec3(&p, pp->m - pp->r1, pp->r1, pp->z);
    glset_arc(&a, &p, pp->r1, 270, 360);
    push_arc(pv, &a, STEP);

    glset_vec3(&p, pp->m - pp->r2, pp->m - pp->r2, pp->z);
    glset_arc(&a, &p, pp->r2, 0, 90);
    push_arc(pv, &a, STEP);

    glset_vec3(&p, pp->r1, pp->m - pp->r1, pp->z);
    glset_arc(&a, &p, pp->r1, 90, 180);
    push_arc(pv, &a, STEP);

    glset_vec3(&p, 0, 0, 0);
    glpush_vec3(pv, &p);
}


static void
create_petal_vbo() {
    glvector * v;
    glpetal petal;

    v = glalloc_vector(0);
    glset_petal(&petal, 1.0f,0.25f*1.0f, 0.50f*1.0f, g_petal_depth); 
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

    glUseProgram(gfcontext.pprg.pid);
    gfcontext.pvloc_ver = glGetAttribLocation(gfcontext.pprg.pid, "vertexs");
    gfcontext.pvloc_mat_s = glGetUniformLocation(gfcontext.pprg.pid, "matrix_s");
    gfcontext.pvloc_mat_r = glGetUniformLocation(gfcontext.pprg.pid, "matrix_r");
    gfcontext.pvloc_mat_mf = glGetUniformLocation(gfcontext.pprg.pid, "matrix_mf");
    gfcontext.pvloc_mat_mp = glGetUniformLocation(gfcontext.pprg.pid, "matrix_mp");
    gfcontext.pfloc_lgc = glGetUniformLocation(gfcontext.pprg.pid, "liner_gradient_colors");
	gfcontext.pfloc_lgs = glGetUniformLocation(gfcontext.pprg.pid, "liner_gradient_stop");
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
    gfcontext.cfloc_rgc = glGetUniformLocation(gfcontext.cprg.pid, "radial_gradient_color");
    gfcontext.cfloc_rgs = glGetUniformLocation(gfcontext.cprg.pid, "radial_gradient_stop");
    glUseProgram(0);
}

static void 
gldraw_petal(glflower_obj * pf) {
    glmat4 * m_s;
    glmat4 * m_r;
    glmat4 * m_mf;
    glmat4 * m_mp;
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
        m_mf = glcreate_identify_mat4();
        m_mp = glcreate_identify_mat4();

        glscale_mat4(m_s, &(pf->ps));
        glUniformMatrix4fv(gfcontext.pvloc_mat_s, 1, true,  glget_mat4_array(m_s));

        glmove_mat4(m_mp, &(pf->pm));
        glUniformMatrix4fv(gfcontext.pvloc_mat_mp, 1, true,  glget_mat4_array(m_mp));

        glrotatez_mat4(m_r, ang);
        glUniformMatrix4fv(gfcontext.pvloc_mat_r, 1, true,  glget_mat4_array(m_r));

        glmove_mat4(m_mf, &(pf->fm));
        glUniformMatrix4fv(gfcontext.pvloc_mat_mf, 1, true,  glget_mat4_array(m_mf));

        glUniform3fv(gfcontext.pfloc_lgc, 2, (const GLfloat *)g_petal_colors[pf->cf]);
		glUniform1fv(gfcontext.pfloc_lgs, 2, (const GLfloat *)g_petal_stop[pf->cf]);

        glDrawArrays(GL_TRIANGLE_FAN, 0, gfcontext.pbsize/3);

        glfree_mat4(m_s);
        glfree_mat4(m_r);
        glfree_mat4(m_mf);
        glfree_mat4(m_mp);
        m_s = NULL;
        m_r = NULL;
        m_mf = NULL;
        m_mp = NULL;
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

    glscale_mat4(m_s, &(pf->cs));
    glUniformMatrix4fv(gfcontext.cvloc_mat_s, 1, true,  glget_mat4_array(m_s));

    glmove_mat4(m_m, &(pf->fm));
    glUniformMatrix4fv(gfcontext.cvloc_mat_m, 1, true,  glget_mat4_array(m_m));

    glUniform3fv(gfcontext.cfloc_rgc, 3, (const glfloat *)g_center_colors[pf->cf]);
    glUniform1fv(gfcontext.cfloc_rgs, 3, (const glfloat *)g_center_stop[pf->cf]);

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
    fo->fm.y = 1.0f - fs->p.y * sy;
    fo->fm.z = 0.0f;

    fo->pm.x = fs->sl/fs->sp;
    fo->pm.y = fs->sl/fs->sp;
    fo->pm.z = 0.0f;

    fo->fa = fs->a;
    fo->cf = fs->cf;
}

void
glrender_flower_context() {
    glflower f;
    glflower_obj fo;

    f.sp = 20.0f;
    f.sl = 0.5f;
    f.sc = 10.0f;
    f.p.x = 50.0f;
    f.p.y = 50.0f;
    f.p.z = 0.0f;
    f.cf = 0;
    f.a = 30;

    set_flower_obj(&fo, &f);

	gldraw_petal(&fo);
    gldraw_center(&fo);   
}
