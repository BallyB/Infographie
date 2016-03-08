#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <cmath>


const int DEFAULT_ALLOC=4;

class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r=DEFAULT_ALLOC, int c=DEFAULT_ALLOC);
    inline int nrows();
    inline int ncols();

    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();

    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};


#endif // MATRIX_H_INCLUDED
