#include <GL/glut.h>
#include <iostream>
#include "model.h"
#include "renderer.h"
#include "geometry.h"

Model *model = NULL;

Vec3f light_dir(1, -1, 1); // y is reversed

fmatrix ModelView     = fmatrix::identity(4);
fmatrix ModelViewPick = fmatrix::identity(4);
int windoww = 800;
int windowh = 800;
Renderer image = Renderer(windoww, windowh, 3);;

void dump_zbuffer(int *zbuffer, int width, int height, const char *filename) {
	TGAImage zb(width, height, 1);
	int maxv = -1;
	int minv = image.ZBUF_RESOLUTION;
	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) {
			int z = std::max(zbuffer[i+j*width],-1);
			if (z>maxv) maxv = z;
			if (z<minv) minv = z;
		}
	}
	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) {
			int v = int((zbuffer[i+j*width]-minv)*255.f/(float)(maxv-minv));
			TGAColor c(v, 1);
			zb.set(i, j, c);
		}
	}
	zb.write_tga_file(filename);
}

struct Quaternion {
	Quaternion(float W=0.f) : w(W), v(0,0,0) {}
	Quaternion(float W, Vec3f V) : w(W), v(V) {}
	Quaternion operator *(const Quaternion &q) const {
		return Quaternion(w*q.w-v*q.v, (v^q.v) + q.v*w + v*q.w);
	}
	fmatrix rot_matrix() {
		fmatrix mat(4,4);
		float xx = v.x*v.x;
		float xy = v.x*v.y;
		float xz = v.x*v.z;
		float xw = v.x*w;
		float yy = v.y*v.y;
		float yz = v.y*v.z;
		float yw = v.y*w;
		float zz = v.z*v.z;
		float zw = v.z*w;
		mat[0][0] = 1. - 2.*(yy + zz);
		mat[0][1] =      2.*(xy - zw);
		mat[0][2] =      2.*(xz + yw);
		mat[1][0] =      2.*(xy + zw);
		mat[1][1] = 1. - 2.*(xx + zz);
		mat[1][2] =      2.*(yz - xw);
		mat[2][0] =      2.*(xz - yw);
		mat[2][1] =      2.*(yz + xw);
		mat[2][2] = 1. - 2.*(xx + yy);
		mat[0][3] = mat[1][3] = mat[2][3] = mat[3][0] = mat[3][1] = mat[3][2] = 0;
		mat[3][3] = 1;
		return mat;
	}
	float w;
	Vec3f v;
};

Quaternion startq = Quaternion(1.f);
bool drag = false;

static GLuint tex_name;

void bind_texture() {
	glBindTexture(GL_TEXTURE_2D, tex_name);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 3==image.get_bytespp() ? GL_RGB8 : GL_RGBA8 , image.get_width(), image.get_height(), 0, 3==image.get_bytespp() ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, image.buffer());
}

void display(void) {
	fmatrix look, unlook;
	lookat(Vec3f(0,0,0), light_dir*100000., look, unlook); // *bignumber just to simulate some sort of directional light
	zoom(look, .7); // dirty hack to fit the demon horns to the lighting buffer
	look = ModelView*look; // the (static wrt world coordinates) light is moving wrt to the model
	image.set_matrices(look);
	image.render(*model, NULL);
	std::vector<int> zbuffer = image._zbuffer;
	dump_zbuffer(zbuffer.data(), image.get_width(), image.get_height(), "shadowmap.tga");

	image.set_light(light_dir);
	image.set_matrices(ModelView, look);
	image.render(*model, zbuffer.data());
	image.write_tga_file("render.tga");
	zbuffer = image._zbuffer;
	dump_zbuffer(zbuffer.data(), image.get_width(), image.get_height(), "zbuffer.tga");
	bind_texture();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_name);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex2f(-1.0,  1.0);
	glTexCoord2f(1.0, 0.0); glVertex2f( 1.0,  1.0);
	glTexCoord2f(1.0, 1.0); glVertex2f( 1.0, -1.0);
	glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, -1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glFlush();
}

void reshape(int w, int h) {
	windoww = w;
	windowh = h;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0);
			break;
		default:
			std::cerr << key << std::endl;
		break;
	}
	glutPostRedisplay();
}


Vec3f project_to_sphere(Vec2i p) {
	Vec2f center(windoww/2., windowh/2.);
	float radius = sqrt(pow(windoww/2., 2.) + pow(windowh/2., 2.));
	Vec3f result((p.x-center.x)/radius, (p.y-center.y)/radius, 0);
	float r = result.norm();
	if (r>1.) {
		result.x /= r;
		result.y /= r;
	} else {
		result.z = sqrtf(1. - r*r);
	}
	return result;
}

Vec3f startp, curp;
fmatrix calc_rotation_matrix(Vec3f startp, Vec3f curp) {
	Quaternion rot = Quaternion(startp*curp, startp^curp);
	return ModelViewPick * (startq*rot).rot_matrix();
}

void mousemove(int x, int y) {
	curp = project_to_sphere(Vec2i(std::min(std::max(windoww-1-x, 0), windoww-1), std::min(std::max(windowh-1-y, 0), windowh-1)));
	if (!drag||(curp-startp).norm()<.00001) return;
	ModelView = calc_rotation_matrix(startp, curp);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (GLUT_LEFT_BUTTON!=button) return;
	if (GLUT_UP==state) {
		drag = false;
	} else {
		drag = true;
		startp = project_to_sphere(Vec2i(windoww-1-x, windowh-1-y));
		ModelViewPick = ModelView;
	}
}

int main(int argc, char** argv) {
	if (2==argc) {
		model = new Model(argv[1]);
	} else {
		model = new Model("obj/anchor.obj");
	}
	ModelView[1][1] = -1.; // flip vertically
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windoww, windowh);
	glutInitWindowPosition(-1, -1);
	glutCreateWindow(argv[0]);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glGenTextures(1, &tex_name);
	bind_texture();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mousemove);
	glutMainLoop();
	delete model;
	return 0;
}

