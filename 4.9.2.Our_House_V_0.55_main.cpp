#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"

Scene scene;

void printMatrix(const glm::mat4& mat) {
	for (int row = 0; row < 4; ++row) {
		printf("[ ");
		for (int col = 0; col < 4; ++col) {
			printf("%8.3f ", mat[col][row]);
		}
		printf("]\n");
	}
}

void printVec(const glm::vec3& v) {
	printf("[ ");
	for (int row = 0; row < 3; ++row) {
		printf("%8.3f ", v[row]);
	}
	printf("]\n");
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto& camera_wrapper : scene.camera_list) {
		Camera& cam = camera_wrapper.get();
		if (cam.cam_proj.projection_type == CAMERA_PROJECTION_PERSPECTIVE && scene.display_cam != cam.camera_id) continue;
		
		glViewport(cam.view_port.x, cam.view_port.y,cam.view_port.w, cam.view_port.h);
		scene.ViewMatrix = cam.ViewMatrix;
		scene.ProjectionMatrix = cam.ProjectionMatrix;

		scene.draw_world();
	}
	glutSwapBuffers();
}

void zoom_aux(Camera_Zoom zoom) {
	for (auto& camera_wrapper : scene.camera_list) {
		Camera& cam = camera_wrapper.get();
		if (cam.camera_id != CAMERA_MAIN) continue;

		Perspective_Camera* persp_cam = static_cast<Perspective_Camera*>(&cam);
		persp_cam->zoom_camera(zoom);
	}
	glutPostRedisplay();
}

void move_aux(Camera_Move move) {
	for (auto& camera_wrapper : scene.camera_list) {
		Camera& cam = camera_wrapper.get();
		if (cam.camera_id != CAMERA_MAIN) continue;
		cam.move_camera(move);
	}
	glutPostRedisplay();
}

void tilt_aux(Camera_Tilt tilt) {
	for (auto& camera_wrapper : scene.camera_list) {
		Camera& cam = camera_wrapper.get();
		if (cam.camera_id != CAMERA_MAIN) continue;
		cam.tilt_camera(tilt);
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) 
		 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else 
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	case 'w':
		move_aux(CAMERA_FRONT);
		break;
	case 's':
		move_aux(CAMERA_BACK);
		break;
	case 'a':
		move_aux(CAMERA_LEFT);
		break;
	case 'd':
		move_aux(CAMERA_RIGHT);
		break;
	case 'q':
		move_aux(CAMERA_UP);
		break;
	case 'e':
		move_aux(CAMERA_DOWN);
		break;

	case 'j':
		tilt_aux(V_RC);
		break;
	case 'l':
		tilt_aux(V_C);
		break;
	case 'i':
		tilt_aux(U_RC);
		break;
	case 'k':
		tilt_aux(U_C);
		break;
	case 'u':
		tilt_aux(N_RC);
		break;
	case 'o':
		tilt_aux(N_C);
		break;

	case '1':
		scene.display_cam = CAMERA_CC_1;
		glutPostRedisplay();
		break;
	case '2':
		scene.display_cam = CAMERA_CC_2;
		glutPostRedisplay();
		break;
	case '3':
		scene.display_cam = CAMERA_CC_3;
		glutPostRedisplay();
		break;
	case '0':
		scene.display_cam = CAMERA_MAIN;
		glutPostRedisplay();
		break;
	case 'D':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
	}
}

void reshape(int width, int height) {
	scene.window.width = width;
	scene.window.height = height;
	scene.window.aspect_ratio = (float)width / height;
	scene.create_camera_list(scene.window.width, scene.window.height, scene.window.aspect_ratio);
	glutPostRedisplay();
}

void mouse_wheel(int button, int state, int x, int y) {
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events

		if (button == 3) zoom_aux(ZOOM_IN);
		else zoom_aux(ZOOM_OUT);
	}
	else {  
		// printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
	}
}


void timer_scene(int index) {
	scene.clock(0);
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, 0);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
 	glutTimerFunc(100, timer_scene, 0);
	glutMouseFunc(mouse_wheel);
//	glutCloseFunc(cleanup_OpenGL_stuffs or else); // Do it yourself!!!
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
}

void initialize_renderer(void) {
	register_callbacks();
	initialize_OpenGL();
	scene.initialize();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170/AIE4012 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170/AIE4120 Our_House_GLSL_V_0.55";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: fill it yourself!" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
