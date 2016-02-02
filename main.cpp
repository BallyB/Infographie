#include "tgaimage.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <sstream>
#include <algorithm>

using std::ifstream;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
TGAImage* Img;


// COodonnees barycentrique

/*
On a Ax Ay
Bx By
Cx Cy

on a notre point qu'on test Px Py
je cherche u,v tel que P = (1-u-v)A + uB + vC)
cette équation vient du repere cartésien
Px = (1-u-v)Ax+ uBx + vCx
Py = (1-u-v)Ay+ uBy + vCy
On parcourt tout les pixels de l'image,
On regarde les coordonnées barycentrique de ce pixel par rapport au triangle,
si au moin un pixel est negatif alors on jette pixel



*/
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

void getLineNumberv(ifstream &myFile,std::vector<char> &tabvx, std::vector<char> &tabvy, std::vector<char> &tabvz, std::vector<int> &tabv, std::vector<std::vector<int> > &taballv){

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
		  std::istringstream issx(strx);
		  std::istringstream issy(stry);
		  std::istringstream issz(strz);
		  int nombrex;
		  int nombrey;
		  int nombrez;
		  issx >> nombrex;
		  issy >> nombrey;
		  issz >> nombrez;

        nombrex += 300;
		nombrey += 300;
		nombrez += 300;
		  tabv.push_back(nombrex);
		  tabv.push_back(nombrey);
		  tabv.push_back(nombrez);

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

void barymetricFullMethod(int Ax, int Ay, int Az, int Bx, int By, int Bz, int Cx, int Cy, int Cz){

    int starty = std::max(std::max(Ay,By),Cy);
    int startx = std::min(std::min(Ax,Bx),Cx);
    int endx = std::max(std::max(Ax,Bx),Cx);
    int endy = std::min(std::min(Ay,By),Cy);


    float denominateur = (((Bx-Ax)*(Cy-Ay))-((Cx-Ax)*(By-Ay)));


    float coeff = 1./denominateur;

       // cout << "Mon point de depart : " << startx << starty << "Mon point d'arrivée : " << endx << endy;
    for(int i = starty; i>=endy; i--){
        for(int j = startx; j<=endx;j++){
            int Px = j;
            int Py = i;
                //image.set(Px, Py, red);
            float u = (coeff*(Cy-Ay))*(Px-Ax)+(coeff*(Ax-Cx))*(Py-Ay);
            float v = (coeff*(Ay-By))*(Px-Ax)+(coeff*(Bx-Ax))*(Py-Ay);
            float w = 1-u-v;
            if (u < 0 || v < 0 || w < 0){

            }else{
                Img->set(Px, Py, white);
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

  /*  if (Ay==By && Ay==Cy) return; // i dont care about degenerate triangles
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
    if (Ay>By) std::swap(t0, t1);
    if (t0.y>t2.y) std::swap(t0, t2);
    if (t1.y>t2.y) std::swap(t1, t2);
    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        Vec2i A =               t0 + (t2-t0)*alpha;
        Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y
        }
    }*/

}
void drawTriangle(int Ax, int Ay, int Bx, int By, int Cx, int Cy, TGAColor color){
    line(Ax,Ay,Bx,By,*Img,color);
    line(Ax,Ay,Cx,Cy,*Img,color);
    line(Bx,By,Cx,Cy,*Img,color);

}
void parsefile(ifstream &file, ifstream &file2, std::vector<char> &tabvx, std::vector<char> &tabvy, std::vector<char> &tabvz, std::vector<int> &tabv, std::vector<std::vector<int> > &taballv){

    file.open("african_head.obj");
    file2.open("african_head.obj");
    getLineNumberv(file, tabvx, tabvy, tabvz, tabv, taballv);


    std::vector<char> sommet1;
    std::vector<char> sommet2;
    std::vector<char> sommet3;
    //int count = 0;
    while (!file2.eof())
    {
       // std::cout << "ligne fini";
        std::string myString;
        getline(file2, myString);

        //int taille = myString.size() & INT_MAX;
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
        int Ax = taballv[nombre1-1][0];
        int Ay = taballv[nombre1-1][1];
        int Az = taballv[nombre1-1][2];
        int Bx = taballv[nombre2-1][0];
        int By = taballv[nombre2-1][1];
        int Bz = taballv[nombre2-1][2];
        int Cx = taballv[nombre3-1][0];
        int Cy = taballv[nombre3-1][1];
        int Cz = taballv[nombre3-1][2];

        barymetricFullMethod(Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz);
        //lineSweepingMethod(Ax,Ay,Bx,By,Cx,Cy);
       // std::cout << "Triangle dessiné";
        drawTriangle(Ax,Ay,Bx,By,Cx,Cy,white);

        sommet1.clear();
        sommet2.clear();
        sommet3.clear();

	  }

  }

}



int main(int argc, char** argv) {
    std::vector<char> tabvx;
    std::vector<char> tabvy;
    std::vector<char> tabvz;
    std::vector<int> tabv;
    std::vector<std::vector<int> > taballv;
    TGAImage image(600, 600, TGAImage::RGB);
    Img = &image;
    ifstream file;
    ifstream file2;
    parsefile(file, file2, tabvx, tabvy, tabvz, tabv, taballv);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
