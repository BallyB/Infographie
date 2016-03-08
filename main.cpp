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

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
TGAImage* Img;
TGAImage* Imgtexture;
std::vector<char> tabvx;
std::vector<char> tabvy;
std::vector<char> tabvz;
std::vector<double> tabv;
std::vector<char> tabvtx;
std::vector<char> tabvty;
std::vector<double> tabvt;
std::vector<std::vector<double> > taballvt;
std::vector<std::vector<double> > taballv;
int widthimg = 600;
int heightimg = 600;
int depthimg = 600;
int zbuffer[600][600];
double lightvector[3];
double vecteurnormal[3];
// COodonnees barycentrique

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

void getLineCoordV(ifstream &myFile,std::vector<char> &tabvx, std::vector<char> &tabvy, std::vector<char> &tabvz, std::vector<double> &tabv, std::vector<std::vector<double> > &taballv){

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
		 // std::istringstream issx(strx);
		 // std::istringstream issy(stry);
		 // std::istringstream issz(strz);
		 // int nombrex;
		 // int nombrey;
		 // int nombrez;
		 // issx >> nombrex;
		//  issy >> nombrey;
		 // issz >> nombrez;
          double x = atof(strx.c_str());
          double y = atof(stry.c_str());
          double z = atof(strz.c_str());
        /*  x = x * 300;
          y = y * 300;
		  z = z * 300;
          x += 300;
		  y += 300;
		  z += 300;*/
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



	 // counter++;

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

void barycentricFullMethod(double Ax, double Ay, double Az, double Bx, double By, double Bz, double Cx, double Cy, double Cz, double Atexturex, double Atexturey, double Btexturex, double Btexturey, double Ctexturex, double Ctexturey, int zbuffer[600][600]){

    Matrix viewporttest = viewport(widthimg, heightimg, depthimg);
    Matrix rotates = rotation(90);

    Matrix Projection = Matrix::identity(4);
    Projection[3][2] = -1.f/0.9;
   // std::cout << Projection;
    viewporttest = viewporttest*Projection*rotates;
   // std::cout << viewporttest;
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
    vecteurnormal[0] = (By-Ay)*(Cz-Az) - (Bz-Az)*(Cy-Ay);
    vecteurnormal[1] = (Bz-Az)*(Cx-Ax) - (Bx-Ax)*(Cz-Az);
    vecteurnormal[2] = (Bx-Ax)*(Cy-Ay) - (By-Ay)*(Cx-Ax);

    lightvector[0] = 0;
    lightvector[1] = 0;
    lightvector[2] = 1;

    // Normalisation lumière
    double tmp = sqrt(lightvector[0]*lightvector[0] + lightvector[1]*lightvector[1] + lightvector[2]*lightvector[2]);
    lightvector[0] /= tmp;
    lightvector[1] /= tmp;
    lightvector[2] /= tmp;
   // std::cout << vecteurnormal[0] << " " << vecteurnormal[1] << " " << vecteurnormal[2] << std::endl;
    //Normalisation vecteur triangle
    double tmp2 = sqrt(vecteurnormal[0]*vecteurnormal[0] + vecteurnormal[1]*vecteurnormal[1] + vecteurnormal[2]*vecteurnormal[2]);
    vecteurnormal[0] /= tmp2;
    vecteurnormal[1] /= tmp2;
    vecteurnormal[2] /= tmp2;
   // std::cout << lightvector[0] << " " << lightvector[1] << " " << lightvector[2] << std::endl;
    //std::cout << vecteurnormal[0] << " " << vecteurnormal[1] << " " << vecteurnormal[2] << std::endl;
    double denominateur = (((Bx-Ax)*(Cy-Ay))-((Cx-Ax)*(By-Ay)));

   // Img->set(273, 235, red);
    double coeff = 1./denominateur;
   // TGAColor rndcolor = TGAColor(rand()%255, rand()%255, 255, 255);

       // cout << "Mon point de depart : " << startx << starty << "Mon point d'arrivée : " << endx << endy;
    for(int i = starty; i>=endy; i--){
        for(int j = startx; j<=endx;j++){

            int Px = j;
            int Py = i;
                //image.set(Px, Py, red);
            double u = (coeff*(Cy-Ay))*(Px-Ax)+(coeff*(Ax-Cx))*(Py-Ay);
            double v = (coeff*(Ay-By))*(Px-Ax)+(coeff*(Bx-Ax))*(Py-Ay);
            double w = 1-u-v;
            if (u < -1e-5 || v < -1e-5 || w < -1e-5){
            //if (u < 0 || v < 0 || w < 0){

            }else{
                double Pz = w*Az+u*Bz+v*Cz;


              // Pour la texture  int (Pu,Pv) = u*(Au,Av)+v*(Bu,Bv)+w*(Cu,Cv);

                if(zbuffer[Px][Py]>Pz){
                    continue;

                }else{
                    zbuffer[Px][Py] = Pz;
                    int TextPx = (w*Atexturex+u*Btexturex+v*Ctexturex) * Imgtexture->get_width();
                    int TextPy = (w*Atexturey+u*Btexturey+v*Ctexturey) * Imgtexture->get_height();
                  //  std::cout << Px << " x \n";
                  //  std::cout << Py;
                  double intensity = std::abs(vecteurnormal[0]*lightvector[0] + vecteurnormal[1]*lightvector[1] + vecteurnormal[2]*lightvector[2]);
                  //std::cout << vecteurnormal[0]*lightvector[0] << " + " << vecteurnormal[1]*lightvector[1] << " + " << vecteurnormal[2]*lightvector[2] << " = " << intensity << std::endl;
                 // std::cout << lightvector[0] << " " << lightvector[1] << " " << lightvector[2] << std::endl;
                 // std::cout << vecteurnormal[0] << " " << vecteurnormal[1] << " " << vecteurnormal[2] << std::endl;
                 // std::cout << intensity;
                    TGAColor colortest = Imgtexture->get(TextPx,TextPy);
               //   if(Px == 273 && Py == 235){
               //   std::cout << "Trou" << colortest.val;
               //   }
                   //
                    //    std::cout << "Coord Atexturex  :" << Atexturex << " Coord Btexturex  : " << Btexturex << " Coord Ctexturex  : " << Ctexturex <<std::endl;
                   //     std::cout << " valeur w : " << w << " valeur u :" << u << " valeur v " << v << std::endl;
                   //     std::cout << "Coord x texture :" << TextPx << " Coord y texture :" << TextPy << std::endl;
                   // }
                  // TGAColor cl = TGAColor(255,255,255,255);

                   TGAColor finalcolor = TGAColor(colortest.r*intensity, colortest.g*intensity, colortest.b*intensity, colortest.a*intensity);
                    Img->set(Px, Py, finalcolor);

                }




            }
        }
    }

}

void lineSweepingMethod(int Ax, int Ay, int Bx, int By, int Cx, int Cy){
    if(Ay > By) {std::swap(Ay,By);std::swap(Ax,Bx);}
    if(Ay > Cy) {std::swap(Ay,Cy);std::swap(Ax,Cx);}
    if(By > Cy) {std::swap(By,Cy);std::swap(Bx,Cx);}

    int bigHeight = Cy-Ay;


    for(int y = Ay;y<By;y++){
        int lowPartHeight = By-Ay;
        float alpha = (float)(y-Ay)/bigHeight;
        float beta  = (float)(y-Ay)/lowPartHeight; // be careful with divisions by zero
        int Tmpx = Ax + (Cx-Ax)*alpha;
        int Tmp2x = Ax + (Bx-Ax)*beta;
        if (Tmpx>Tmp2x) std::swap(Tmpx,Tmp2x);
        for (int j=Tmpx; j<=Tmp2x; j++) {
            Img->set(j, y, white); // attention, due to int casts t0.y+i != A.y
        }
    }
    for(int y = By;y<Cy;y++){
        int highPartHeight = Cy-By;
        float alpha = (float)(y-Ay)/bigHeight;
        float beta  = (float)(y-By)/highPartHeight; // be careful with divisions by zero
        int Tmpx = Ax + (Cx-Ax)*alpha;
      //  int Tmpy = Ay + (Cy-Ay)*alpha;
        int Tmp2x = Bx + (Cx-Bx)*beta;
      //  int Tmp2y = Ay + (By-Ay)*beta;
      //  Img->set(Tmpx, y, red);
      //  Img->set(Tmp2x, y, white);
        if (Tmpx>Tmp2x) std::swap(Tmpx,Tmp2x);
        for (int j=Tmpx; j<=Tmp2x; j++) {
            Img->set(j, y, white); // attention, due to int casts t0.y+i != A.y
        }
    }


}
void drawTriangle(int Ax, int Ay, int Bx, int By, int Cx, int Cy, TGAColor color){
    line(Ax,Ay,Bx,By,*Img,color);
    line(Ax,Ay,Cx,Cy,*Img,color);
    line(Bx,By,Cx,Cy,*Img,color);

}

void getLineCoordVt(ifstream &myFile,std::vector<char> &tabvtx, std::vector<char> &tabvty, std::vector<double> &tabvt, std::vector<std::vector<double> > &taballvt){
//int cpt = 0;
    while (!myFile.eof())
    {

        std::string myString;
        getline(myFile, myString);


	  int taille = myString.size() & INT_MAX;
      //std::cout<<myString[1];
	  if ((myString[0] == 'v') && (myString[1] == 't') && (myString[2] == ' ')) {
      //  cpt++;//std::cout << "la ta mere";
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
		  //std::istringstream issx(strx);
		  //std::istringstream issy(stry);
		  //int nombrex;
		 // int nombrey;
		  //issx >> nombrex;
		//  issy >> nombrey;
          double nombrex = atof(strx.c_str());
          double nombrey = atof(stry.c_str());
       //   nombrex *= 600;
		//  nombrey *= 600;
		  //std::cout << nombrex << " nombre x et "<< nombrey << " nombre y"<< std::endl;
		  tabvt.push_back(nombrex);
		  tabvt.push_back(nombrey);

          taballvt.push_back(tabvt);
		  tabvt.pop_back();
		  tabvt.pop_back();
		  tabvtx.clear();
		  tabvty.clear();

	  }
  }
 // std::cout << cpt;
  myFile.close();
}
void parsefile(ifstream &file, ifstream &file2, ifstream &file3,ifstream &file4, std::vector<char> &tabvx, std::vector<char> &tabvy, std::vector<char> &tabvz, std::vector<double> &tabv, std::vector<std::vector<double> > &taballv, std::vector<char> &tabvtx, std::vector<char> &tabvty, std::vector<double> &tabvt, std::vector<std::vector<double> > &taballvt){


    for (int i=0; i<600; i++) {
        for(int j=0; j<600; j++){
            zbuffer[i][j] = std::numeric_limits<int>::min();
        }

    }
    file.open("african_head.obj");
    file2.open("african_head.obj");
    file3.open("african_head.obj");
    file4.open("african_head.obj");

    getLineCoordV(file, tabvx, tabvy, tabvz, tabv, taballv);
   // std::cout << "LA";
    getLineCoordVt(file3, tabvtx, tabvty, tabvt, taballvt);
   // std::cout << "LAAUSSI";
    //getLineCoordTexture(file3);
    /* Lecture du fichier pour recup les ligne vt avec u et v
    recup de f x/x/x et le deuxieme est la ligne correspondante.
    u et v sont les coord du point dans la texture : utiliser read_tga_file pour ouvrir la texture*/



    std::vector<char> sommet1;
    std::vector<char> sommet2;
    std::vector<char> sommet3;
    std::vector<char> couleursommet1;
    std::vector<char> couleursommet2;
    std::vector<char> couleursommet3;
    //int count = 0;
    int cpt = 0;
    while (!file2.eof())
    {
       // std::cout << "ligne fini";
        std::string myString;
        getline(file2, myString);

        //int taille = myString.size() & INT_MAX;
	  if ((myString[0] == 'f') && (myString[1] == ' ')) {
             cpt++;
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
		  while (myString[i] != ' ') {
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
		  while (myString[i] != ' ') {
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
		  }

        std::string strx(sommet1.begin(), sommet1.end());
        std::string stry(sommet2.begin(), sommet2.end());
        std::string strz(sommet3.begin(), sommet3.end());
        std::string strvtx(couleursommet1.begin(), couleursommet1.end());
        std::string strvty(couleursommet2.begin(), couleursommet2.end());
        std::string strvtz(couleursommet3.begin(), couleursommet3.end());
        std::istringstream isstx(strvtx);
        std::istringstream issty(strvty);
        std::istringstream isstz(strvtz);
        std::istringstream issx(strx);
        std::istringstream issy(stry);
        std::istringstream issz(strz);
        int nombre1;
        int nombre2;
        int nombre3;
        int nombre1vt;
        int nombre2vt;
        int nombre3vt;
        issx >> nombre1;
        issy >> nombre2;
        issz >> nombre3;
        isstx >> nombre1vt;
        issty >> nombre2vt;
        isstz >> nombre3vt;

        double Ax = taballv[nombre1-1][0];
        double Ay = taballv[nombre1-1][1];
        double Az = taballv[nombre1-1][2];
       // std::cout << taballvt[nombre1vt-1][0] << " Atexturex " << std::endl;
        double Atexturex = taballvt[nombre1vt-1][0];
        double Atexturey = taballvt[nombre1vt-1][1];

        double Bx = taballv[nombre2-1][0];
        double By = taballv[nombre2-1][1];
        double Bz = taballv[nombre2-1][2];
        double Btexturex = taballvt[nombre2vt-1][0];
        double Btexturey = taballvt[nombre2vt-1][1];

        double Cx = taballv[nombre3-1][0];
        double Cy = taballv[nombre3-1][1];
        double Cz = taballv[nombre3-1][2];
        double Ctexturex = taballvt[nombre3vt-1][0];
        double Ctexturey = taballvt[nombre3vt-1][1];
       // std::cout << cpt;
       // if (cpt < 6){
     //       std::cout << cpt << std::endl;
            //std::cout << " Ax : " << Ax << " Ay : " << Ay << " Az : " << Az << std::endl;
     //       std::cout << " Atexturex : " << Atexturex << " Atexturey  : " << Atexturey << std::endl;
     //       std::cout << " Btexturex : " << Btexturex << " Btexturey  : " << Btexturey << std::endl;
     //       std::cout << " Ctexturex : " << Ctexturex << " Ctexturey  : " << Ctexturey << std::endl;
      //  }
       // std::cout << "PD";
        barycentricFullMethod(Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz, Atexturex, Atexturey, Btexturex, Btexturey, Ctexturex, Ctexturey, zbuffer);
        //lineSweepingMethod(Ax,Ay,Bx,By,Cx,Cy);
       // std::cout << "Triangle dessiné";
       // drawTrikangle(Ax,Ay,Bx,By,Cx,Cy,white);

        sommet1.clear();
        sommet2.clear();
        sommet3.clear();
        couleursommet1.clear();
        couleursommet2.clear();
        couleursommet3.clear();

	  }


  }

}





/*void getTexture(ifstream &file3,ifstream &file4, std::vector<char> &tabvtx, std::vector<char> &tabvty, std::vector<int> &tabvt, std::vector<std::vector<int> > &taballvt){


    std::vector<char> sommet1;
    std::vector<char> sommet2;
    std::vector<char> sommet3;

    while (!file4.eof())
    {
        std::string myString;
        getline(file4, myString);

        //int taille = myString.size() & INT_MAX;
	  if ((myString[0] == 'f') && (myString[1] == ' ')) {
        myString.erase(myString.begin(),myString.begin()+2);
		  int i = 0;
		  while (myString[i] == ' ') {
			  i++;
        }
		  while (myString[i] != '/') {
              i++;

		  }
		  i++;
		  while (myString[i] != '/') {

              char c = myString[i];

    // PREMIER SOMMET
			  sommet1.push_back(c);
			  i++;

		  }

		  while (myString[i] != ' ') {
			  i++;
		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != '/') {

			  i++;
		  }
		  i++;
		  while (myString[i] != '/') {
			  char c = myString[i];
    // DEUXIEME SOMMET
			 sommet2.push_back(c);
			  i++;
		  }
		  while (myString[i] != ' ') {
			  i++;
		  }
		  while (myString[i] == ' ') {
			  i++;
		  }
		  while (myString[i] != '/') {

			  i++;
		  }
		  i++;
		  while (myString[i] != '/') {
			  char c = myString[i];
    // Troisieme SOMMET
			 sommet3.push_back(c);
			  i++;
		  }
        std::string strx(sommet1.begin(), sommet1.end());
        std::string stry(sommet2.begin(), sommet2.end());
        std::string strz(sommet3.begin(), sommet3.end());
        std::istringstream issx(strx);
        std::istringstream issy(stry);
        std::istringstream issz(strz);
        int nombre1;
        int nombre2;
        int nombre3;
        issx >> nombre1;
        issy >> nombre2;
        issz >> nombre3;
        int Ax = taballvt[nombre1-1][0];
        int Ay = taballvt[nombre1-1][1];
        int Az = taballvt[nombre1-1][2];
        int Bx = taballvt[nombre2-1][0];
        int By = taballvt[nombre2-1][1];
        int Bz = taballvt[nombre2-1][2];
        int Cx = taballvt[nombre3-1][0];
        int Cy = taballvt[nombre3-1][1];
        int Cz = taballvt[nombre3-1][2];

       // barycentricFullMethod(Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz, zbuffer);
        //lineSweepingMethod(Ax,Ay,Bx,By,Cx,Cy);
       // std::cout << "Triangle dessiné";
       // drawTriangle(Ax,Ay,Bx,By,Cx,Cy,white);

        sommet1.clear();
        sommet2.clear();
        sommet3.clear();

	  }

  }
}*/
int main(int argc, char** argv) {

   // std::cout<< "SALUIT";

    TGAImage image(600, 600, TGAImage::RGB);
    TGAImage imagetexture(1000,1000,TGAImage::RGB);
    imagetexture.read_tga_file("african_head_diffuse.tga");
   //TGAImage texture;
    //texture.get_height();
   // texture.read_tga_file("african_head_diffuse.tga");
    Img = &image;
    imagetexture.flip_vertically();
    Imgtexture = &imagetexture;
    ifstream file;
    ifstream file2;
    ifstream file3;
    ifstream file4;
    //getTexture(file3, file4, tabvtx, tabvty, tabvt, taballvt);
    parsefile(file, file2,file3, file4, tabvx, tabvy, tabvz, tabv, taballv, tabvtx, tabvty, tabvt, taballvt);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
