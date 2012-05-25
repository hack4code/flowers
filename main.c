/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2012 01:24:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WarTalker (), wartalker@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "glflower.h"

#ifdef __APPLE_CC__

#include <GL/glfw.h>

int main( void ) {
    int running = GL_TRUE;

    // Initialize GLFW
    if( !glfwInit() ) {
        exit( EXIT_FAILURE );
    }

    // Open an OpenGL window
    if( !glfwOpenWindow(400,300, 0,0,0,0,0,0, GLFW_WINDOW)) {
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

	glinit_flower_context();

    while(running) {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_SMOOTH);
        glClear(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glrender_flower_context();
        glfwSwapBuffers();
        running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
    }

    glfwTerminate();

    exit(EXIT_SUCCESS);
}
#endif

#ifdef _WIN32

#include <GL/glew.h>
#include <GL/glut.h>

static void
render(void) {
	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_SMOOTH);
    glClear(GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	glClearColor(1.0, 1.0, 1.0, 1.0);
    glrender_flower_context();
    glutSwapBuffers();
}

int 
main(int argc, char** argv)
{
    //char * sversion;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    glutCreateWindow("flower");
//    glutIdleFunc(&update);
    glutDisplayFunc(&render);

    glewInit();
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "OpenGL 2.0 not available\n");
        exit(EXIT_FAILURE);
    }
	//sversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
 //   fprintf(stdout, "glsl version = %s\n", sversion);
	glinit_flower_context();
	glinit_branch_context();

    glutMainLoop();

    exit(EXIT_SUCCESS);
}

#endif
