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
#define BRANCH_STEP (2.0f/180.0f*PI)

#define LENGTH(x,y) ((glfloat)(sqrt(pow((x),2)+pow((y),2))))

static const glfloat g_petal_depth = 0.0f;
static const glfloat g_center_depth = -0.01f;

extern const glfloat PI;

static glfloat g_screen_width = 800.0f;
static glfloat g_screen_height = 800.0f;

static glbranch * g_main_branchs = NULL;
static glbranch * g_sub_branchs = NULL;
static glbranch_obj * g_branch_objs = NULL;

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

static glfloat g_branch_color[3] = { 43.0f/255.0f, 13.0f/255.0f, 13.0f/255.0f };

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
	"\tvec2 gpline = vec2(sqrt(2.0f)/2.0f, sqrt(2.0f)/2.0f);\n" \
    "\tgp = 1.0f - dot(vertex_pos, gpline)/sqrt(2.0f);\n" \
    "\tif (gp <= liner_gradient_stop[0])\n" \
    "\t\tgl_FragColor = mix(vec4(liner_gradient_colors[1], 0.85f), vec4(liner_gradient_colors[1], 1.0f), gp/liner_gradient_stop[0]);\n" \
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


static const glchar * branch_vshader =
{
    "#version 120\n\n" \
	"attribute vec3 vertexs;\n" \
    "uniform mat4 mat;\n\n" \
    "void main()\n" \
    "{\n" \
    "\tgl_Position = mat * vec4(vertexs, 1.0f);\n" \
    "}"
};

static const glchar * branch_fshader =
{
    "#version 120\n\n" \
    "uniform vec3 color;\n" \
    "void main()\n" \
    "{\n" \
    "\tgl_FragColor = vec4(color, 1.0f);\n" \
    "}"
};

static glflower_context gfcontext;
static glbranch_context gbcontext;

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
    glset_petal(&petal, 1.0f, 0.25f*1.0f, 0.50f*1.0f, g_petal_depth); 
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
push_branch(glvector * * v, glbranch * b, glfloat step) {
    glfloat rx = b->rx;
    glfloat ry = b->ry;
    glfloat max = b->wmax;
    glfloat min = b->wmin;
    size_t ns = (size_t)(b->al/step);
    glfloat ws = (max - min)/(ns<<1);
	glfloat cx = rx * (glfloat)sin(b->al/2);
	glfloat cy = ry * (glfloat)cos(b->al/2);

	glfloat	fa;
    glvec3 p1, p2;
    size_t i;

    for (i = 0; i <= ns; ++i) {
        if (0 == i)
            fa = (1.50f*(glfloat)PI - b->al/2.0f); 
        else if (ns == i)
            fa = 1.50f*(glfloat)PI + b->al/2.0f;
        else
            fa = 1.50f*(glfloat)PI - b->al/2 + i*step; 

        //p1.x = cx + rx*cos(fa);
        //p1.y = cy + ry*sin(fa);
        //p1.z = b->z;
        //if (b->isflip) p1.y = -p1.y;
        //glpush_vec3(v, &p1);

        p1.x = cx + (rx - 0.5f*max + i*ws)*(glfloat)cos(fa);
        p1.y = (b->isflip) ?  (0.5f*max - i*ws - ry)*(glfloat)sin(fa) - cy : cy + (ry - 0.5f*max + i*ws)*(glfloat)sin(fa);
        p1.z = b->z;

        p2.x = cx + (rx + 0.5f*max - i*ws)*(glfloat)cos(fa);
        p2.y = (b->isflip) ? (i*ws - 0.5f*max - ry)*(glfloat)sin(fa) - cy : cy + (ry + 0.5f*max - i*ws)*(glfloat)sin(fa);
        p2.z = b->z;

        (b->isflip) ? glpush_2vec3(v, &p2, &p1) : glpush_2vec3(v, &p1, &p2);
    }
}

static void
gltransform_branch(glvector * v, glbranch * b) {
    size_t n = glget_vector_size(v);
    glfloat * vec = v->vec;
    glfloat mx = b->p.x;
    glfloat my = b->p.y;
    glfloat a = b->ar;
    glfloat x, y;
    size_t i;

    for (i = 0; i < n; i += 3) {
        x = vec[i];
        y = vec[i+1];
        vec[i]   = mx + x*(glfloat)cos(a) - y*(glfloat)sin(a);
        vec[i+1] = my + x*(glfloat)sin(a) + y*(glfloat)cos(a);
    }
}

static void
set_branch_obj(glbranch_obj * bo, glbranch * b) {
    glvector * vec;
    glmat4 m_m;
    glmat4 m_t;
    glvec3 v;

    glset_identify_mat4(&m_m);

    glset_identify_mat4(&m_t);
    glrotatefz_mat4(&m_t, PI+b->ar);
    glmutiply_mat4(&m_m, &m_t);

    glset_identify_mat4(&m_t);
    v.x = -b->p.x;
    v.y = -b->p.y;
    v.z = 0;
    glmove_mat4(&m_t, &v);
    glmutiply_mat4(&m_m, &m_t);

    v.x = 2.0f/g_screen_width;
    v.y = 2.0f/g_screen_height;
    v.z = 1.0f;
    glset_identify_mat4(&m_t);
    glscale_mat4(&m_t, &v);
    glmutiply_mat4(&m_m, &m_t);

    glset_identify_mat4(&m_t);
    v.x = 1.0f;
    v.y = 1.0f;
    v.z = 0.0f;
    glmove_mat4(&m_t, &v);

    glmutiply_mat4(&m_m, &m_t);
    glassign_mat4(&(bo->m), &m_m);

    vec = glalloc_vector(0);
    push_branch(&vec, b, BRANCH_STEP);
    bo->bbsize = glget_vector_size(vec);
	//glprint_vector(vec);
    
    glGenBuffers(1, &(bo->bvbo));
    glBindBuffer(GL_ARRAY_BUFFER, bo->bvbo);
    glBufferData(GL_ARRAY_BUFFER, bo->bbsize*sizeof(glfloat), glget_vector_array(vec), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	glfree_vector(vec);
    vec = NULL;
}

static glbranch *
glcreate_branch() {
    glbranch * b;
    
    b = (glbranch *)malloc(sizeof(*b));
    memset((void *)b, 0, sizeof(*b));
    return b;
}

static glbranch_obj *
glcreate_branch_obj() {
    glbranch_obj * bo;

    bo = (glbranch_obj *)malloc(sizeof(*bo));
    memset((void *)bo, 0, sizeof(*bo));
    return bo;
}

static void
glpush_branch_obj(glbranch_obj * bo) {
    bo->next = g_branch_objs;
    g_branch_objs = bo;
}

static void
glcreate_main_branch_obj(glvector * * pv) {
    glvector * vec = glalloc_vector(0);
    glbranch_obj * bo = glcreate_branch_obj();
    glbranch * tb;
    glvector * tv;
    glmat4 m_m;
    glvec3 v;

    for (tb = g_main_branchs; tb != NULL; tb = tb->next) {
        tv = glalloc_vector(0);
        push_branch(&tv, tb, BRANCH_STEP);
        gltransform_branch(tv, tb);
        glappend_vector(&vec, tv);
        glfree_vector(tv);
        tv = NULL;
    }

    bo->bbsize = glget_vector_size(vec);

    glGenBuffers(1, &(bo->bvbo));
    glBindBuffer(GL_ARRAY_BUFFER, bo->bvbo);
    glBufferData(GL_ARRAY_BUFFER, bo->bbsize*sizeof(glfloat), glget_vector_array(vec), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    *pv = vec;
    vec = NULL;

    glset_identify_mat4(&m_m);

    glmutiply_rotatez_mat4(&m_m, PI);

    v.x = 2.0f/g_screen_width;
    v.y = 2.0f/g_screen_height;
    v.z = 1.0f;
    glmutiply_scale_mat4(&m_m, &v);

    v.x = 1.0f;
    v.y = 1.0f;
    v.z = 0.0f;
    glmutiply_move_mat4(&m_m, &v);

    glassign_mat4(&(bo->m), &m_m);

    glpush_branch_obj(bo);
    bo = NULL;
}

static void
glcreate_branch_objs() {
    glbranch * b;
    glbranch_obj * bo;

    for (b = g_sub_branchs; b != NULL; b = b->next) {
        bo = glcreate_branch_obj();
        set_branch_obj(bo, b);
        glpush_branch_obj(bo);
        bo = NULL;
    }
}

static void
glpush_main_branch(glbranch * b) {
    glbranch * t;

    b->next = NULL;
    if (NULL == g_main_branchs) 
        g_main_branchs = b;
    else {
        for (t = g_main_branchs; t->next != NULL; t = t->next)
            ; 
        t->next = b;
    }
}

static void
glpush_sub_branch(glbranch * b) {
    b->next = g_sub_branchs;
    g_sub_branchs = b;
}


static glfloat
get_slope(glbranch * b, glfloat a) {
	glfloat s = b->rx/b->ry * (glfloat)tan(a);
	return (s > 0.0f) ? s : -s;
}

static glfloat
get_al(glbranch * b, glfloat s) {
    return  (glfloat)(PI - 2*atan(s));
}

static void
get_p(glvec3 * v, glbranch * b, glfloat fa) {
    glfloat rx = b->rx;
    glfloat ry = b->ry;
    glfloat cx = rx * (glfloat)sin(b->al/2.0f);
    glfloat cy = ry * (glfloat)cos(b->al/2.0f);

    v->x = cx + rx * (glfloat)cos(fa);
    v->y = cy + ry * (glfloat)sin(fa);
    v->z = b->z;
}

static void
generate_main_branch(glbranch * bf) {
    glvec3 p;
    glfloat as = 1.50f*PI + bf->al/2.0f;
    glfloat s = get_slope(bf, as);
    glfloat ar = bf->ar;
    glbranch * b = glcreate_branch();

    b->ar = ar; 
    b->rx = bf->rx;
    b->ry = bf->ry;
	b->wmax = bf->wmin;
    b->wmin = b->wmax * 0.618f;
    b->isflip = !bf->isflip;
	b->z = bf->z;

    get_p(&p, bf, as);
	b->p.x = bf->p.x + p.x * (glfloat)cos(ar) - p.y * (glfloat)sin(ar);
	b->p.y = bf->p.y + p.x * (glfloat)sin(ar) + p.y * (glfloat)cos(ar);
    b->p.z = 0;

    b->al = get_al(b, s);

    glpush_main_branch(b);
}

static glfloat
get_width(glvector * vec, size_t i) {
    glfloat * v = vec->vec;
    glfloat x1 = v[i*6];
    glfloat y1 = v[i*6+1];
    glfloat x2 = v[i*6+3];
    glfloat y2 = v[i*6+4];

    return LENGTH(x2-x1, y2-y1);
}

static void
get_center_p(glvector * vec, size_t i, glfloat * px, glfloat * py) {
    glfloat * v = vec->vec;

    *px = (v[i*6] + v[i*6+3])/2.0f;
    *py = (v[i*6+1] + v[i*6+4])/2.0f;
}

static glfloat
get_length(glvector * v, size_t i) {
    glfloat x, y;

    get_center_p(v, i, &x, &y);
    return LENGTH(x,y);
}

static glfloat
get_ratio(glvector * v, size_t i, glfloat len, glfloat a) {
    glfloat x, y, l;

    get_center_p(v, i, &x, &y);
    l = LENGTH(x,y);
    return l*(glfloat)cos(abs(atan(x/y)-a))/len;
}

static void
create_sub_branchs(glvector * v, glfloat a) {
    size_t n = glget_vector_size(v)/6;
    glfloat l = get_length(v, n-1);
}

static void
create_branch() {
    glvector * v;

    g_main_branchs = glcreate_branch();

	g_main_branchs->al = 120.0f/180.0f*PI;
	g_main_branchs->ar = 60.0f/180.0f*PI;

	g_main_branchs->rx = 250;
    g_main_branchs->ry = 200;

	g_main_branchs->wmax = 20;
	g_main_branchs->wmin = 20*0.618f;

	g_main_branchs->z = 0.6f;
    g_main_branchs->p.x = 0.0f;
    g_main_branchs->p.y = 0.0f;
    g_main_branchs->p.z = 0.0f;
	g_main_branchs->isflip = false;

    generate_main_branch(g_main_branchs);

    glcreate_main_branch_obj(&v);
}

void
glinit_branch_context() {
    bool status;

    memset(&gbcontext, 0, sizeof(gbcontext));

    status = glcreate_programe(&(gbcontext.bprg), branch_vshader, branch_fshader);
    if (!status) {
        fprintf(stdout, "compile branch programe error!\n");
        exit(EXIT_FAILURE);
    }

    glUseProgram(gbcontext.bprg.pid);
    gbcontext.bvloc_ver = glGetAttribLocation(gbcontext.bprg.pid, "vertexs");
    gbcontext.bvloc_mat = glGetUniformLocation(gbcontext.bprg.pid, "mat");
    gbcontext.bfloc_cor = glGetUniformLocation(gbcontext.bprg.pid, "color");
    glUseProgram(0);

    create_branch();
}

static void 
gldraw_petal(glflower_obj * pf) {
    glmat4 * m_s;
    glmat4 * m_r;
    glmat4 * m_mf;
    glmat4 * m_mp;
    glangle ang;

    glUseProgram(gfcontext.pprg.pid);

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

static void
gldraw_branch(glbranch_obj * bo) {
    glUseProgram(gbcontext.bprg.pid);

    glUniformMatrix4fv(gbcontext.bvloc_mat, 1, true,  glget_mat4_array(&(bo->m)));
    glUniform3fv(gbcontext.bfloc_cor, 1, (const glfloat *)g_branch_color);

    glBindBuffer(GL_ARRAY_BUFFER, bo->bvbo);
    glVertexAttribPointer(gbcontext.bvloc_ver, 3, GL_FLOAT, GL_FALSE, 3*sizeof(glfloat), 0);
	glEnableVertexAttribArray(gbcontext.bvloc_ver);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, bo->bbsize/3);
//	glDrawArrays(GL_LINE_STRIP, 0, bo->bbsize/3);

	glDisableVertexAttribArray(gbcontext.bvloc_ver);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}


static void
gldraw_branchs() {
    glbranch_obj * bo;

    for (bo = g_branch_objs; bo != NULL; bo = bo->next)
        gldraw_branch(bo);
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
    f.p.z = 0.5f;
    f.cf = 0;
    f.a = 30;


    set_flower_obj(&fo, &f);

    gldraw_petal(&fo);
    gldraw_center(&fo);   

    gldraw_branchs();
}

void
glinit_tree_context() {
    glinit_flower_context();
    glinit_branch_context();
}

void
glrender_tree_context() {
    glrender_flower_context();
}
