#include "geometry.h"

void translate(fmatrix &m, Vec3f v) {
	assert(4==m.ncols());
	fmatrix Tr = fmatrix::identity(4);
	Tr[3][0] = v.x;
	Tr[3][1] = v.y;
	Tr[3][2] = v.z;
	m = m*Tr;
}

void zoom(fmatrix &m, float factor) {
	fmatrix Z = fmatrix::identity(4);
	Z[0][0] = Z[1][1] = Z[2][2] = factor;
	m = m*Z;
}

void rotate_x(fmatrix &m, float cosangle, float sinangle) {
	fmatrix R = fmatrix::identity(4);
	R[1][1] = R[2][2] = cosangle;
	R[1][2] =  sinangle;
	R[2][1] = -sinangle;
	m = m*R;
}

void rotate_y(fmatrix &m, float cosangle, float sinangle) {
	fmatrix R = fmatrix::identity(4);
	R[0][0] = R[2][2] = cosangle;
	R[0][2] = -sinangle;
	R[2][0] =  sinangle;
	m = m*R;
}

void rotate_z(fmatrix &m, float cosangle, float sinangle) {
	fmatrix R = fmatrix::identity(4);
	R[0][0] = R[1][1] = cosangle;
	R[0][1] =  sinangle;
	R[1][0] = -sinangle;
	m = m*R;
}

void lookat(Vec3f p0, Vec3f p1, fmatrix &look, fmatrix &unlook) {
	look = unlook = fmatrix::identity(4);
	translate(look, p0*(-1));
	float len  = (p1-p0).norm();
	if (len>.00001f) {
		rotate_y(unlook, sqrt(1.-pow(p1.x-p0.x, 2)/pow(len,2)), (p1.x-p0.x)/len);
		float plen = sqrt(pow(p1.y-p0.y, 2) + pow(p1.z-p0.z, 2));
		if (plen>.00001f) {
			rotate_x(look,  (p1.z-p0.z)/plen,  (p1.y-p0.y)/plen);
			rotate_x(unlook, (p1.z-p0.z)/plen, -(p1.y-p0.y)/plen);
		}
		rotate_y(look, sqrt(1.-pow(p1.x-p0.x, 2)/pow(len, 2)), -(p1.x-p0.x)/len);
	}
	translate(unlook, p0);
}

