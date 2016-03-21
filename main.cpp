#include "tgaimage.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <sstream>
#include <algorithm>
#include <limits>
#include "Matrix.h"
#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif
using std::ifstream;

const TGAColor white = TGAColor(255,255,255,255);
TGAImage* Img;
TGAImage* Imgtexture;
TGAImage* imgnm;
TGAImage* imgspecmap;
std::vector<char> tabvx;
std::vector<char> tabvy;
std::vector<char> tabvz;
std::vector<double> tabv;

std::vector<char> tabvtx;
std::vector<char> tabvty;
std::vector<double> tabvt;

std::vector<char> tabvnx;
std::vector<char> tabvny;
std::vector<char> tabvnz;
std::vector<double> tabvn;

std::vector<char> sommet1;
std::vector<char> sommet2;
std::vector<char> sommet3;
std::vector<char> couleursommet1;
std::vector<char> couleursommet2;
std::vector<char> couleursommet3;
std::vector<char> vecteursommet1;
std::vector<char> vecteursommet2;
std::vector<char> vecteursommet3;

std::vector<std::vector<double> > taballvn;
std::vector<std::vector<double> > taballvt;
std::vector<std::vector<double> > taballv;
int widthimg = 600;
int heightimg = 600;
int depthimg = 600;
int zbuffer[600][600];
double lightvector[3];
double lightvector2[3];
double vecteurnormal[3];
double r[3];
double r2[3];

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

void getLineCoordV(ifstream &myFile){

    while (!myFile.eof())
    {
      std::string myString;
      getline(myFile, myString);
	  int taille = myString.size() & INT_MAX;
	  if ((myString[0] == 'v') && (myString[1] == ' ')) {
        myString.erase(myString.begin(),myString.begin()+3);
		  int i = 0;
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != ' ') {
			  char c = myString[i];

			  tabvx.push_back(c);
			  i++;
		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != ' ') {
			  char c = myString[i];
			  tabvy.push_back(c);
			  i++;
		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (i < (taille + 1)) {
			  char c = myString[i];

			  tabvz.push_back(c);
			  i++;
		  }

		  std::string strx(tabvx.begin(), tabvx.end());
		  std::string stry(tabvy.begin(), tabvy.end());
		  std::string strz(tabvz.begin(), tabvz.end());
          double x = atof(strx.c_str());
          double y = atof(stry.c_str());
          double z = atof(strz.c_str());
		  tabv.push_back(x);
		  tabv.push_back(y);
		  tabv.push_back(z);
          taballv.push_back(tabv);
		  tabv.pop_back();
		  tabv.pop_back();
		  tabv.pop_back();
		  tabvx.clear();
		  tabvy.clear();
		  tabvz.clear();

	  }

  }
  myFile.close();
}

Matrix viewport(int width, int height, int depth) {
    Matrix m(4,4);
    m[0][3] = width/2.f;
    m[1][3] = height/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = width/2.f;
    m[1][1] = height/2.f;
    m[2][2] = depth/2.f;
    m[3][3] = 1;
    return m;
}

Matrix rotation(int alpha){
    Matrix m(4,4);
    double angleradient = alpha*M_PI/180;
    m[0][0] = std::cos(angleradient);
    m[1][1] = 1;
    m[2][2] = std::cos(angleradient);
    m[3][3] = 1;
    m[2][0] = -std::sin(angleradient);
    m[0][2] = std::sin(angleradient);
    return m;
}

void barycentricFullMethod(double Ax, double Ay, double Az, double Bx, double By, double Bz, double Cx, double Cy, double Cz, double Atexturex, double Atexturey, double Btexturex, double Btexturey, double Ctexturex, double Ctexturey, double Avecteurx, double Avecteury, double Avecteurz,double Bvecteurx, double Bvecteury, double Bvecteurz,double Cvecteurx,double Cvecteury,double Cvecteurz, int zbuffer[600][600]){

    Matrix viewporttest = viewport(widthimg, heightimg, depthimg);
    Matrix rotates = rotation(330);

    Matrix Projection = Matrix::identity(4);
    Projection[3][2] = -1.f/1000;
    viewporttest = viewporttest*Projection*rotates;
    Matrix m1(4,1);
    Matrix m2(4,1);
    Matrix m3(4,1);
    m1[0][0] = Ax;
    m1[1][0] = Ay;
    m1[2][0] = Az;
    m1[3][0] = 1;

    m2[0][0] = Bx;
    m2[1][0] = By;
    m2[2][0] = Bz;
    m2[3][0] = 1;

    m3[0][0] = Cx;
    m3[1][0] = Cy;
    m3[2][0] = Cz;
    m3[3][0] = 1;



    m1 = viewporttest*m1;
    m2 = viewporttest*m2;
    m3 = viewporttest*m3;

    Ax = m1[0][0]/m1[3][0];
    Ay = m1[1][0]/m1[3][0];
    Az = m1[2][0]/m1[3][0];

    Bx = m2[0][0]/m2[3][0];
    By = m2[1][0]/m2[3][0];
    Bz = m2[2][0]/m2[3][0];

    Cx = m3[0][0]/m3[3][0];
    Cy = m3[1][0]/m3[3][0];
    Cz = m3[2][0]/m3[3][0];

    int starty = std::max(std::max(Ay,By),Cy);
    int startx = std::min(std::min(Ax,Bx),Cx);
    int endx = std::max(std::max(Ax,Bx),Cx);
    int endy = std::min(std::min(Ay,By),Cy);
    starty = std::min(starty,heightimg);
    endx = std::min(endx, heightimg);
    startx = std::max(0, startx);
    endy = std::max(0, endy);

    lightvector[0] = 1;
    lightvector[1] = 1;
    lightvector[2] = 0;

    lightvector2[0] = 0;
    lightvector2[1] = 1;
    lightvector2[2] = 1;

    // Normalisation lumi�re
    double tmp = sqrt(lightvector[0]*lightvector[0] + lightvector[1]*lightvector[1] + lightvector[2]*lightvector[2]);
    lightvector[0] /= tmp;
    lightvector[1] /= tmp;
    lightvector[2] /= tmp;

    double tmp8 = sqrt(lightvector2[0]*lightvector2[0] + lightvector2[1]*lightvector2[1] + lightvector2[2]*lightvector2[2]);
    lightvector2[0] /= tmp8;
    lightvector2[1] /= tmp8;
    lightvector2[2] /= tmp8;


    double denominateur = (((Bx-Ax)*(Cy-Ay))-((Cx-Ax)*(By-Ay)));
    double constantescalaire;
    double constantescalaire2;
    double coeff = 1./denominateur;

    for(int i = starty; i>=endy; i--){
        for(int j = startx; j<=endx;j++){

            int Px = j;
            int Py = i;
                //image.set(Px, Py, red);
            double u = (coeff*(Cy-Ay))*(Px-Ax)+(coeff*(Ax-Cx))*(Py-Ay);
            double v = (coeff*(Ay-By))*(Px-Ax)+(coeff*(Bx-Ax))*(Py-Ay);
            double w = 1-u-v;
            if (u < -1e-5 || v < -1e-5 || w < -1e-5){
            // Pixel hors du triangle

            }else{
                double Pz = w*Az+u*Bz+v*Cz;
                if(zbuffer[Px][Py]>Pz){
                    continue;

                }else{
                zbuffer[Px][Py] = Pz;


                int TextPx = (w*Atexturex+u*Btexturex+v*Ctexturex) * Imgtexture->get_width();
                int TextPy = (w*Atexturey+u*Btexturey+v*Ctexturey) * Imgtexture->get_height();
                TGAColor clrpix= imgnm->get(TextPx,TextPy);
                vecteurnormal[0] = clrpix.r;
                vecteurnormal[1] = clrpix.g;
                vecteurnormal[2] = clrpix.b;

                double tmp2 = sqrt(vecteurnormal[0]*vecteurnormal[0] + vecteurnormal[1]*vecteurnormal[1] + vecteurnormal[2]*vecteurnormal[2]);
                vecteurnormal[0] /= tmp2;
                vecteurnormal[1] /= tmp2;
                vecteurnormal[2] /= tmp2;
                constantescalaire2 = vecteurnormal[0]*lightvector2[0] + vecteurnormal[1]*lightvector2[1] + vecteurnormal[2]*lightvector2[2];
                constantescalaire = vecteurnormal[0]*lightvector[0] + vecteurnormal[1]*lightvector[1] + vecteurnormal[2]*lightvector[2];
                constantescalaire2*=2;
                constantescalaire*= 2;

                r2[0] = vecteurnormal[0];
                r2[1] = vecteurnormal[1];
                r2[3] = vecteurnormal[2];
                r2[0] *= constantescalaire2;
                r2[1] *= constantescalaire2;
                r2[2] *= constantescalaire2;
                r2[0] -= lightvector2[0];
                r2[1] -= lightvector2[1];
                r2[2] -= lightvector2[2];



                r[0] = vecteurnormal[0];
                r[1] = vecteurnormal[1];
                r[2] = vecteurnormal[2];
                r[0] *= constantescalaire;
                r[1] *= constantescalaire;
                r[2] *= constantescalaire;
                r[0] -= lightvector[0];
                r[1] -= lightvector[1];
                r[2] -= lightvector[2];


                double tmp9 = sqrt(r2[0]*r2[0] + r2[1]*r2[1] + r2[2]*r2[2]);
                r2[0] /= tmp9;
                r2[1] /= tmp9;
                r2[2] /= tmp9;

                double tmp3 = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
                r[0] /= tmp3;
                r[1] /= tmp3;
                r[2] /= tmp3;

                TGAColor colorspec= imgspecmap->get(TextPx,TextPy);
                double spec = pow(std::max(r[2], 0.0), colorspec.b);
                double spec2 = pow(std::max(r2[2], 0.0), colorspec.b);
                double intensity = std::max(0.,(vecteurnormal[0]*lightvector[0] + vecteurnormal[1]*lightvector[1] + vecteurnormal[2]*lightvector[2]));
                double intensity2 = std::max(0.,(vecteurnormal[0]*lightvector2[0]+ vecteurnormal[1]*lightvector2[1] + vecteurnormal[2]*lightvector2[2]));
                //TGAColor colortest = Imgtexture->get(TextPx,TextPy);
                TGAColor colortest = white;
                TGAColor finalcolor = TGAColor(std::min<float>(5 + colortest.r*(intensity + .6*spec), 255), std::min<float>(5 + colortest.g*(intensity + .6*spec), 255), std::min<float>(5 + colortest.b*(intensity2 + .6*spec2), 255), 0);
                Img->set(Px, Py, finalcolor);

                }

            }
        }
    }

}


void drawTriangle(int Ax, int Ay, int Bx, int By, int Cx, int Cy, TGAColor color){
    line(Ax,Ay,Bx,By,*Img,color);
    line(Ax,Ay,Cx,Cy,*Img,color);
    line(Bx,By,Cx,Cy,*Img,color);

}
void getLineCoordVn(ifstream &myFile){
    double nombrex;
    double nombrey;
    double nombrez;
    std::string myString;
    while (!myFile.eof())
    {

      getline(myFile, myString);
	  if ((myString[0] == 'v') && (myString[1] == 'n') && (myString[2] == ' ')) {
        myString.erase(myString.begin(),myString.begin()+3);
		  int i = 0;
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != ' ') {
			  char c = myString[i];

			  tabvnx.push_back(c);
			  i++;
		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != ' ') {
			  char c = myString[i];
			  tabvny.push_back(c);
			  i++;
		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != ' ') {
			  char c = myString[i];
			  tabvnz.push_back(c);
			  i++;
		  }
		  std::string strx(tabvnx.begin(), tabvnx.end());
		  std::string stry(tabvny.begin(), tabvny.end());
		  std::string strz(tabvnz.begin(), tabvnz.end());
          nombrex = atof(strx.c_str());
          nombrey = atof(stry.c_str());
          nombrez = atof(strz.c_str());
		  tabvn.push_back(nombrex);
		  tabvn.push_back(nombrey);
          tabvn.push_back(nombrez);
          taballvn.push_back(tabvn);
		  tabvn.pop_back();
		  tabvn.pop_back();
          tabvn.pop_back();
		  tabvnx.clear();
		  tabvny.clear();
          tabvnz.clear();
	  }
  }
  myFile.close();
}
void getLineCoordVt(ifstream &myFile){

    double nombrex;
    double nombrey;
    std::string myString;
    while (!myFile.eof())
    {


      getline(myFile, myString);
	  if ((myString[0] == 'v') && (myString[1] == 't') && (myString[2] == ' ')) {

        myString.erase(myString.begin(),myString.begin()+3);
		  int i = 0;
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != ' ') {
			  char c = myString[i];

			  tabvtx.push_back(c);
			  i++;
		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != ' ') {
			  char c = myString[i];
			  tabvty.push_back(c);
			  i++;
		  }
		  std::string strx(tabvtx.begin(), tabvtx.end());
		  std::string stry(tabvty.begin(), tabvty.end());

          nombrex = atof(strx.c_str());
          nombrey = atof(stry.c_str());
		  tabvt.push_back(nombrex);
		  tabvt.push_back(nombrey);

          taballvt.push_back(tabvt);
		  tabvt.pop_back();
		  tabvt.pop_back();
		  tabvtx.clear();
		  tabvty.clear();

	  }
  }

  myFile.close();
}
void parsefile(ifstream &file, ifstream &file2, ifstream &file3,ifstream &file4, ifstream &file5){


    for (int i=0; i<600; i++) {
        for(int j=0; j<600; j++){
            zbuffer[i][j] = std::numeric_limits<int>::min();
        }

    }
    double Ax;
    double Ay;
    double Az;
    double Atexturex;
    double Atexturey;
    double Avecteurx;
    double Avecteury;
    double Avecteurz;

    double Bx;
    double By;
    double Bz;
    double Btexturex;
    double Btexturey;
    double Bvecteurx;
    double Bvecteury;
    double Bvecteurz;


    double Cx;
    double Cy;
    double Cz;
    double Ctexturex;
    double Ctexturey;
    double Cvecteurx;
    double Cvecteury;
    double Cvecteurz;

    file.open("african_head.obj");
    file2.open("african_head.obj");
    file3.open("african_head.obj");
    file4.open("african_head.obj");

    getLineCoordV(file);
    getLineCoordVt(file3);
    getLineCoordVn(file4);


    int nombre1;
    int nombre2;
    int nombre3;
    int nombre1vt;
    int nombre2vt;
    int nombre3vt;
    int nombre1vn;
    int nombre2vn;
    int nombre3vn;
    std::string myString;
    while (!file2.eof())
    {

      getline(file2, myString);
	  if ((myString[0] == 'f') && (myString[1] == ' ')) {
        myString.erase(myString.begin(),myString.begin()+2);
		  int i = 0;
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != '/') {
			  char c = myString[i];

    // PREMIER SOMMET
			  sommet1.push_back(c);
			  i++;
		  }
		  i++;
		  while (myString[i] != '/') {

              char c = myString[i];

    // Texture PREMIER SOMMET
			  couleursommet1.push_back(c);
			  i++;


		  }
		  i++;
		  while (myString[i] != ' ') {

              char c = myString[i];

    // Vecteur PREMIER SOMMET
			  vecteursommet1.push_back(c);
			  i++;

		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != '/') {
			  char c = myString[i];
    // DEUXIEME SOMMET
			 sommet2.push_back(c);
			  i++;
		  }
		  i++;
		  while (myString[i] != '/') {
			  char c = myString[i];
    // texture DEUXIEME SOMMET
			 couleursommet2.push_back(c);
			  i++;
		  }
		  i++;
		  while (myString[i] != ' ') {

              char c = myString[i];

    // Vecteur DEUXIEME SOMMET
			  vecteursommet2.push_back(c);
			  i++;

		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != '/') {
			  char c = myString[i];
    // Troisieme SOMMET
			 sommet3.push_back(c);
			  i++;
		  }
		  i++;
		  while (myString[i] != '/') {
			  char c = myString[i];
    // texture Troisieme SOMMET
			 couleursommet3.push_back(c);
			  i++;
		  }i++;
		  while (myString[i] != ' ') {

              char c = myString[i];

    // Vecteur TROISIEME SOMMET
			  vecteursommet3.push_back(c);
			  i++;

		  }

        std::string strx(sommet1.begin(), sommet1.end());
        std::string stry(sommet2.begin(), sommet2.end());
        std::string strz(sommet3.begin(), sommet3.end());
        std::string strvtx(couleursommet1.begin(), couleursommet1.end());
        std::string strvty(couleursommet2.begin(), couleursommet2.end());
        std::string strvtz(couleursommet3.begin(), couleursommet3.end());
        std::string strvnx(vecteursommet1.begin(), vecteursommet1.end());
        std::string strvny(vecteursommet2.begin(), vecteursommet2.end());
        std::string strvnz(vecteursommet3.begin(), vecteursommet3.end());
        std::istringstream isstx(strvtx);
        std::istringstream issty(strvty);
        std::istringstream isstz(strvtz);
        std::istringstream issx(strx);
        std::istringstream issy(stry);
        std::istringstream issz(strz);
        std::istringstream issnx(strvnx);
        std::istringstream issny(strvny);
        std::istringstream issnz(strvnz);
        issx >> nombre1;
        issy >> nombre2;
        issz >> nombre3;
        isstx >> nombre1vt;
        issty >> nombre2vt;
        isstz >> nombre3vt;
        issnx >> nombre1vn;
        issny >> nombre2vn;
        issnz >> nombre3vn;

        Ax = taballv[nombre1-1][0];
        Ay = taballv[nombre1-1][1];
        Az = taballv[nombre1-1][2];
        Atexturex = taballvt[nombre1vt-1][0];
        Atexturey = taballvt[nombre1vt-1][1];
        Avecteurx = taballvn[nombre1vn-1][0];
        Avecteury = taballvn[nombre1vn-1][1];
        Avecteurz = taballvn[nombre1vn-1][2];

        Bx = taballv[nombre2-1][0];
        By = taballv[nombre2-1][1];
        Bz = taballv[nombre2-1][2];
        Btexturex = taballvt[nombre2vt-1][0];
        Btexturey = taballvt[nombre2vt-1][1];
        Bvecteurx = taballvn[nombre2vn-1][0];
        Bvecteury = taballvn[nombre2vn-1][1];
        Bvecteurz = taballvn[nombre2vn-1][2];


        Cx = taballv[nombre3-1][0];
        Cy = taballv[nombre3-1][1];
        Cz = taballv[nombre3-1][2];
        Ctexturex = taballvt[nombre3vt-1][0];
        Ctexturey = taballvt[nombre3vt-1][1];
        Cvecteurx = taballvn[nombre3vn-1][0];
        Cvecteury = taballvn[nombre3vn-1][1];
        Cvecteurz = taballvn[nombre3vn-1][2];

        barycentricFullMethod(Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz, Atexturex, Atexturey, Btexturex, Btexturey, Ctexturex, Ctexturey, Avecteurx, Avecteury, Avecteurz, Bvecteurx, Bvecteury, Bvecteurz, Cvecteurx, Cvecteury, Cvecteurz, zbuffer);

        sommet1.clear();
        sommet2.clear();
        sommet3.clear();
        couleursommet1.clear();
        couleursommet2.clear();
        couleursommet3.clear();
        vecteursommet1.clear();
        vecteursommet2.clear();
        vecteursommet3.clear();

	  }


  }

}


int main(int argc, char** argv) {

    TGAImage image(600, 600, TGAImage::RGB);
    TGAImage imagetexture(1000,1000,TGAImage::RGB);
    TGAImage imagenm(1000,1000,TGAImage::RGB);
    TGAImage imagespecmapping(1000,1000,TGAImage::RGB);
    std::cout << " Ouverture des fichiers diffuse, nm et spec . . . \n";
    imagetexture.read_tga_file("african_head_diffuse.tga");
    imagenm.read_tga_file("african_head_nm.tga");
    imagespecmapping.read_tga_file("african_head_spec.tga");
    std::cout << " Done. \n";
    Img = &image;
    imagetexture.flip_vertically();
    imagenm.flip_vertically();
    imagespecmapping.flip_vertically();
    imgspecmap = &imagespecmapping;
    Imgtexture = &imagetexture;
    imgnm = &imagenm;
    ifstream file;
    ifstream file2;
    ifstream file3;
    ifstream file4;
    ifstream file5;
    parsefile(file, file2,file3, file4, file5);

    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}
