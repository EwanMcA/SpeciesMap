#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <GL\glew.h>
#include <freeglut.h>
#include <vector>
#include <SOIL.h>
#include <iostream>
#include <time.h>

#include "map.h"

// Sloppy constants.. clean this up.
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
float cam_x = -50.0f;
float cam_y = 150.0f;
float cam_z = 200.0f;
float cam_theta = atan(cam_x/cam_z);
float cam_hyp = cam_x / sin(cam_theta);
bool menu_up = true;
float menu_width = 100.0f;
GLfloat light_pos[4] = { 1.0f, -1.0f, 0.0f, 0 };
GLfloat light_amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
int aoe = 10;
int height_cap = 2;
int temp_cap = 10;
int prec_cap = 100;
bool grass_on = true;
float scale_x = 200;
float scale_z = 200;
int z_vertices = 129;
int x_vertices = 129;
Map map(x_vertices, z_vertices, 5);
int mountain_line = 40;
enum visual_mode {NORMAL, TEMP, PREC};
visual_mode mode(NORMAL);
bool species_menu = false;
float x = 0;


float* getNormal(float * p1, float * p2, float * p3)
{
	float u[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
	float v[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };

	float normal[3];
	normal[0] = (u[1] * v[2]) - (u[2] * v[1]);
	normal[1] = (u[2] * v[0]) - (u[0] * v[2]);
	normal[2] = (u[0] * v[1]) - (u[1] * v[0]);

	float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] = normal[0] / length;
	normal[1] = normal[1] / length;
	normal[2] = normal[2] / length;
	return normal;
}

GLfloat * getMouseWorldCoords(int x, int y)
{

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	GLfloat coords[3] = { posX, posY, posZ };
	return coords;
}

// From "Simple Open Image Library" Documentation
GLuint load_texture(const char * filename)
{/* load an image file directly as a new OpenGL texture */
	GLuint tex_2d = SOIL_load_OGL_texture
		(
			filename,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);

	/* check for an error during the load process */
	if (0 == tex_2d)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}
	return tex_2d;
}

void changeViewPort(int x, int y) {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, x, y);
	gluPerspective(45.0, (float)x / (float)y, 50.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}

void setColour(int i, int k) {
	if (i < 0 || i > x_vertices || k < 1 || k > z_vertices + 1) { return; }
	switch (mode) {
	case NORMAL:
		if (map.getElev(i, k - 1) < 1) { glColor3f(0.0f, 0.0f, 1.0f); }
		else { glColor3f(1.0f, 1.0f, 1.0f); }
		break;
	case TEMP:
		if (map.getElev(i, k - 1) < 1) { glColor3f(0.2f, 0.2f, 0.2f); }
		else { glColor3f(1.0f, 1.0f - map.getTemp(i, k - 1) / 15, 1.0f - (map.getTemp(i, k - 1)) / 30); }
		break;
	case PREC:
		if (map.getElev(i, k - 1) < 1) { glColor3f(0.2f, 0.2f, 0.2f); }
		else { glColor3f(1.0f - map.getPrec(i, k - 1) / 500, 1.0f - map.getPrec(i, k - 1) / 250, 1.0f); }
		//else { glColor3f(1.0f - map.getPrec(i, k - 1) / 500, 1.0f, 1.0f - map.getPrec(i, k - 1) / 500); }
		break;
	}
}

void renderSpeciesMenu(float x) 
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0f, -1.0f, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.6f, 0.6f, 0.6f);
	glBegin(GL_QUADS);
		glVertex3f(menu_width, 0.0f, 0.0f);
		glVertex3f(menu_width, (float)SCREEN_HEIGHT/1.5f, 0.0f);
		glVertex3f(menu_width+300.0f, (float)SCREEN_HEIGHT/1.5f, 0.0f);
		glVertex3f(menu_width+300.0f, 0.0f, 0.0f);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
		glVertex3f(menu_width, 0.0f, 0.1f);
		glVertex3f(menu_width, (float)SCREEN_HEIGHT / 1.5f, 0.1f);
		glVertex3f(menu_width + 300.0f, (float)SCREEN_HEIGHT / 1.5f, 0.1f);
		glVertex3f(menu_width + 300.0f, 0.0f, 0.1f);
		glVertex3f(menu_width, 0.0f, 0.1f);
	glEnd();
	const char *c;
	const char *text[6] = { "Altitude min", "Altitude max", "Temperature min", "Temperature max", "Preciptation min", "Precipitation max" };
	for (int i = 0; i < 6; i++) {
		glRasterPos3f(menu_width+20.0f, 70.0f+50*i, 1.0f);
		for (c = text[i]; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
		}
		glBegin(GL_QUADS);
			glVertex3f(menu_width + 200.0f, 60.0f + 50 * i, 0.1f);
			glVertex3f(menu_width + 200.0f, 70.0f + 50 * i, 0.1f);
			glVertex3f(menu_width + 210.0f, 70.0f + 50 * i, 0.1f);
			glVertex3f(menu_width + 210.0f, 60.0f + 50 * i, 0.1f);
		glEnd();
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (menu_up) {
		glViewport(menu_width, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	else {
		glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	gluLookAt(cam_x, cam_y, cam_z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glLightf(GL_LIGHT0, GL_POSITION, *light_pos);
	glColor3f(1.0f, 1.0f, 1.0f);
	static GLuint grass = load_texture("grass.jpg");
	static GLuint snow = load_texture("snow.JPG");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grass);
	grass_on = true;
	for (int k = 1; k < z_vertices; k++) {
		glBegin(GL_TRIANGLE_STRIP);
		setColour(0, k);
		glVertex3f(-scale_x / 2, map.getElev(0,k-1), -scale_z / 2 + (k - 1)*(scale_z / z_vertices));
		glVertex3f(-scale_x / 2, map.getElev(0, k), -scale_z / 2 + k*(scale_z / z_vertices));
		for (int i = 1; i < x_vertices; i++) {
			float p3[3] = { -scale_x / 2 + (i - 1)*(scale_x / x_vertices), map.getElev(i - 1,k - 1), -scale_z / 2 + (k - 1)*(scale_z / z_vertices) };
			float p2[3] = { -scale_x / 2 + (i - 1)*(scale_x / x_vertices), map.getElev(i - 1,k), -scale_z / 2 + k*(scale_z / z_vertices) };
			float p1[3] = { -scale_x / 2 + i*(scale_x / x_vertices), map.getElev(i,k-1), -scale_z / 2 + (k-1)*(scale_z / z_vertices) };
			float * normal = getNormal(p1, p3, p2);
			glNormal3f(normal[0], normal[1], normal[2]);
			setColour(i, k);
			if ((grass_on && map.getElev(i, k - 1) >= mountain_line) || (!grass_on && map.getElev(i, k - 1) < mountain_line)) {
				glEnd();
				glDisable(GL_TEXTURE_2D);
				if (grass_on)
					glBindTexture(GL_TEXTURE_2D, snow);
				else
					glBindTexture(GL_TEXTURE_2D, grass);
				glEnable(GL_TEXTURE_2D);
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f((float)(i-1)*(float)(1.0f / x_vertices), (float)(k - 1)*(1.0f / z_vertices));
				glVertex3f(-scale_x / 2 + (i-1)*(scale_x / x_vertices), map.getElev(i-1, k - 1), -scale_z / 2 + (k - 1)*(scale_z / z_vertices));
				glTexCoord2f((float)(i - 1)*(float)(1.0f / x_vertices), (float)k*(1.0f / z_vertices));
				glVertex3f(-scale_x / 2 + (i - 1)*(scale_x / x_vertices), map.getElev(i - 1, k), -scale_z / 2 + k*(scale_z / z_vertices));
				grass_on = (grass_on) ? false : true;
			}
			glTexCoord2f((float)i*(float)(1.0f / x_vertices), (float)(k-1)*(1.0f / z_vertices));
			glVertex3f(-scale_x / 2 + i*(scale_x / x_vertices), map.getElev(i, k - 1), -scale_z / 2 + (k - 1)*(scale_z / z_vertices));
			*p3 = *p1;
			p1[0] = -scale_x / 2 + i*(scale_x / x_vertices);
			p1[1] = map.getElev(i, k);
			p1[2] = -scale_z / 2 + k*(scale_z / z_vertices);
			normal = getNormal(p1, p3, p2);
			glNormal3f(normal[0], normal[1], normal[2]);
			glTexCoord2f((float)i*(float)(1.0f / x_vertices), (float)k*(1.0f / z_vertices));
			glVertex3f(-scale_x / 2 + i*(scale_x / x_vertices), map.getElev(i, k), -scale_z / 2 + k*(scale_z / z_vertices));
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_LINES);
	for (float i = -90; i < 100; i += 10) {
		glVertex3f(i, 0.0f, 100.0f);
		glVertex3f(i, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f, i);
		glVertex3f(100.0f, 0.0f, i);
	}
	glEnd();
	if (menu_up) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glViewport(0, 0, menu_width, glutGet(GLUT_WINDOW_HEIGHT));
		glOrtho(0, menu_width, SCREEN_HEIGHT, 0.0f, -1.0f, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glColor3f(0.6f, 0.6f, 0.6f);
		glBegin(GL_QUADS);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, (float)SCREEN_HEIGHT, 0.0f);
			glVertex3f(menu_width, (float)SCREEN_HEIGHT, 0.0f);
			glVertex3f(menu_width, 0.0f, 0.0f);
		glEnd();
		for (int i = 0; i < 3; i++) {
			if (i == 0 && mode == NORMAL) {
				glColor3f(0.2f,1.0f,0.2f);
			}
			else if (i == 1 && mode == TEMP) {
				glColor3f(1.0f, 0.2f, 0.2f);
			}
			else if (i == 2 && mode == PREC) {
				glColor3f(0.2f, 0.2f, 1.0f);
			}
			else {
				glColor3f(0.4f, 0.4f, 0.4f);
			}
			glBegin(GL_QUADS);
				glVertex3f(25.0f + 20 * i, 30.0f, 1.0f);
				glVertex3f(25.0f + 20 * i, 40.0f, 1.0f);
				glVertex3f(35.0f + 20 * i, 40.0f, 1.0f);
				glVertex3f(35.0f + 20 * i, 30.0f, 1.0f);
			glEnd();
		}
		if (species_menu) {
			renderSpeciesMenu(x++);
		}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}
	glutSwapBuffers();
}

// x and y are screen coords from (0,0) at top-left.
void mouseActive(int x, int y)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (menu_up) {
		glViewport(menu_width, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	else {
		glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
	gluLookAt(cam_x, cam_y, cam_z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	GLfloat *objectCoords = getMouseWorldCoords(x, y);
	x = (objectCoords[0] + scale_x / 2)/(scale_x / x_vertices);
	int z = (objectCoords[2] + scale_z / 2)/(scale_z / z_vertices);
	if (mode == NORMAL) {
		for (int k = 0; k < z_vertices; k++) {
			for (int i = 0; i < x_vertices; i++) {
				if (sqrt(std::pow(abs(x - i), 2) + std::pow(abs(z - k), 2)) < aoe && map.getElev(i, k) < height_cap) {
					float amount = (height_cap - map.getElev(i, k) < 1) ? 0.1f : 1.0f;
					if (height_cap - map.getElev(i,k) > 10) { amount = 2.0f; }
					map.addElev(i, k, amount);
				}
			}
		}
	}
	else if (mode == TEMP) {
		for (int k = 0; k < z_vertices; k++) {
			for (int i = 0; i < x_vertices; i++) {
				if (sqrt(std::pow(abs(x - i), 2) + std::pow(abs(z - k), 2)) < aoe && map.getTemp(i, k) < temp_cap) {
					map.addTemp(i, k, 1);
				}
			}
		}
	}
	else if (mode == PREC) {
		for (int k = 0; k < z_vertices; k++) {
			for (int i = 0; i < x_vertices; i++) {
				if (sqrt(std::pow(abs(x - i), 2) + std::pow(abs(z - k), 2)) < aoe && map.getPrec(i, k) < prec_cap) {
					map.addPrec(i, k, 10);
				}
			}
		}
	}
}

void specPress(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		cam_y += 5.0f;
		break;
	case GLUT_KEY_DOWN:
		cam_y -= 5.0f;
		break;
	case GLUT_KEY_LEFT:
		cam_theta -= 0.05f;
		cam_x = sin(cam_theta)*cam_hyp;
		cam_z = cos(cam_theta)*cam_hyp;
		break;
	case GLUT_KEY_RIGHT:
		cam_theta += 0.05f;
		cam_x = sin(cam_theta)*cam_hyp;
		cam_z = cos(cam_theta)*cam_hyp;
		break;
	case GLUT_KEY_CTRL_L:
		menu_up = menu_up ? false : true;
		break;
	case GLUT_KEY_CTRL_R:
		species_menu = species_menu ? false : true;
		break;
	}
}

void keyPress(unsigned char key, int x, int y)
{
	switch ((int)key) {
	case 110:
		mode = NORMAL;
		break;
	case 112:
		mode = PREC;
		break;
	case 116:
		mode = TEMP;
		break;
	case 49:
		aoe = 5;
		break;
	case 50:
		aoe += 5;
		break;
	case 51:
		if (mode == NORMAL) { height_cap -= 5; }
		else if (mode == TEMP) { temp_cap -= 5; }
		else if (mode == PREC) { prec_cap -= 100; }
		break;
	case 52:
		if (mode == NORMAL) { height_cap += 5; }
		else if (mode == TEMP) { temp_cap += 5; }
		else if (mode == PREC) { prec_cap += 100; }
		break;
	case 53:
		if (mode == NORMAL) { height_cap = 100; }
		else if (mode == TEMP) { temp_cap = 50; }
		else if (mode == PREC) { prec_cap = 1000; }
		break;
	case 114:
		map.clear(x_vertices, z_vertices);
		map.addElev(0, 0, std::rand() % 30 - 5);
		map.addElev(128, 0, std::rand() % 30 - 5);
		map.addElev(0, 128, std::rand() % 30 - 5);
		map.addElev(128, 128, std::rand() % 30 - 5);
		map.randomise(80.0f, 1.0f);
		map.smooth(3);
		break;
	case 115:
		cam_hyp += 5.0f;
		cam_x = sin(cam_theta)*cam_hyp;
		cam_z = cos(cam_theta)*cam_hyp;
		break;
	case 119:
		cam_hyp -= 5.0f;
		cam_x = sin(cam_theta)*cam_hyp;
		cam_z = cos(cam_theta)*cam_hyp;
		break;
	}
}

int main(int argc, char* argv[]) {

	// Initialize GLUT
	glutInit(&argc, argv);
	// Set up some memory buffers for our display
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// This one allows me to close the network before the program exits after "X" is pressed.
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Species&Landscape");
	//glutFullScreen();
	// Bind functions
	glutReshapeFunc(changeViewPort);
	glutDisplayFunc(render);
	glutIdleFunc(render);
	glutKeyboardFunc(keyPress);
	glutSpecialFunc(specPress);
	/*glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mousePassive);
	*/
	glutMotionFunc(mouseActive);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightf(GL_LIGHT0, GL_AMBIENT, *light_amb);
	glLightf(GL_LIGHT0, GL_DIFFUSE, *light_diff);
	glutMainLoop();
	return 0;
}