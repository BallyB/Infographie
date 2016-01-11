#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return sqrtf(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_ALLOC 4

template <class t> class matrix {
	typedef std::vector<std::vector<t> > data_t;
	std::vector<std::vector<t> > m;
	int rows, cols;
public:
	matrix(int r=DEFAULT_ALLOC, int c=DEFAULT_ALLOC);
	matrix(Vec3f v, float fourth);
	inline int nrows();
	inline int ncols();

	static matrix<t> identity(int dimensions);
	inline std::vector<t>& operator[](const int i);
	matrix<t> operator*(const matrix<t>& a);
	matrix<t> transpose();
	matrix<t> inverse();
	matrix<t> rref();
	matrix<t> kernel();

	template <class > friend std::ostream& operator<<(std::ostream& s, matrix<t>& m);
};

typedef matrix<float>  fmatrix;
void zoom(fmatrix &m, float factor);
void lookat(Vec3f p0, Vec3f p1, fmatrix &look, fmatrix &unlook);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> matrix<t>::matrix(int r, int c) : m(std::vector<std::vector<t> >(r, std::vector<t>(c, t()))), rows(r), cols(c) { }

template <class t> matrix<t>::matrix(Vec3f v, float fourth) : rows(1), cols(4) {
	m.resize(1);
	m[0].resize(4);
	for (int i=0; i<3; i++) m[0][i] = v.raw[i];
	m[0][3] = fourth;
}

template <class t> int matrix<t>::nrows() {
	return rows;
}

template <class t> int matrix<t>::ncols() {
	return cols;
}

template <class t> matrix<t> matrix<t>::identity(int dimensions) {
	matrix<t> E(dimensions, dimensions);
	for (int i=0; i<dimensions; i++) {
		for (int j=0; j<dimensions; j++) {
			E[i][j] = (i==j ? t(1) : t(0));
		}
	}
	return E;
}

template <class t> std::vector<t>& matrix<t>::operator[](const int i) {
	assert(i>=0 && i<rows);
	return m[i];
}

template <class t> matrix<t> matrix<t>::operator*(const matrix<t>& a) {
	if (cols!=a.rows) {
		matrix<t> z = a;
		std::cerr << cols << "!=" << a.rows << "\n" << *this << "\n===========\n" << z << std::endl;
	}
	assert(cols == a.rows);
	matrix<t> result(rows, a.cols);
	for (int i=0; i<rows; i++) {
		for (int j=0; j<a.cols; j++) {
			result.m[i][j] = t();
			for (int k=0; k<cols; k++) {
				result.m[i][j] += m[i][k]*a.m[k][j];
			}
		}
	}
	return result;
}

template <class t> matrix<t> matrix<t>::transpose() {
	matrix<t> result(cols, rows);
	for(int i=0; i<rows; i++)
		for(int j=0; j<cols; j++)
			result[j][i] = m[i][j];
	return result;
}

template <class t> matrix<t> matrix<t>::inverse() {
	assert(rows==cols);
	// augmenting the square matrix with the identity matrix of the same dimensions A => [AI]
	matrix<t> result(rows, cols*2);
	for(int i=0; i<rows; i++)
		for(int j=0; j<cols; j++)
			result[i][j] = m[i][j];
	for(int i=0; i<rows; i++)
		result[i][i+cols] = 1;
	// first pass
	for (int i=0; i<rows-1; i++) {
		// normalize the first row
		for(int j=result.cols-1; j>=0; j--)
			result[i][j] /= result[i][i];
		for (int k=i+1; k<rows; k++) {
			t coeff = result[k][i];
			for (int j=0; j<result.cols; j++) {
				result[k][j] -= result[i][j]*coeff;
			}
		}
	}
	// normalize the last row
	for(int j=result.cols-1; j>=rows-1; j--)
		result[rows-1][j] /= result[rows-1][rows-1];
	// second pass
	for (int i=rows-1; i>0; i--) {
		for (int k=i-1; k>=0; k--) {
			t coeff = result[k][i];
			for (int j=0; j<result.cols; j++) {
				result[k][j] -= result[i][j]*coeff;
			}
		}
	}
	// cut the identity matrix back
	matrix<t> truncate(rows, cols);
	for(int i=0; i<rows; i++)
		for(int j=0; j<cols; j++)
			truncate[i][j] = result[i][j+cols];
	return truncate;
}

template <class t> std::ostream& operator<<(std::ostream& s, matrix<t>& m) {
	for (int i=0; i<m.nrows(); i++)  {
		for (int j=0; j<m.ncols(); j++) {
			s << m[i][j];
			if (j<m.ncols()-1) s << "\t";
		}
		s << "\n";
	}
	return s;
}

#endif //__GEOMETRY_H__
