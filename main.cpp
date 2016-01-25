#include "tgaimage.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <sstream>


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


void parsefile(ifstream &file, ifstream &file2, std::vector<char> &tabvx, std::vector<char> &tabvy, std::vector<char> &tabvz, std::vector<int> &tabv, std::vector<std::vector<int> > &taballv){

    file.open("african_head.obj");
    file2.open("african_head.obj");
    getLineNumberv(file, tabvx, tabvy, tabvz, tabv, taballv);


    std::vector<char> sommet1;
    std::vector<char> sommet2;
    std::vector<char> sommet3;
    int count = 0;
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
          int Bx = taballv[nombre2-1][0];
          int By = taballv[nombre2-1][1];
          int Cx = taballv[nombre3-1][0];
          int Cy = taballv[nombre3-1][1];

            if(count < 15){


          float denominateur = (((Bx-Ax)*(Cy-Ay))-((Cx-Ax)*(By-Ay)));

          float coeff = 1./denominateur;
            for(int i = 0; i<=600; i++){
            for(int j = 0; j<=600;j++){
                int Px = j;
                int Py = i;
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
         // line(taballv[nombre1-1][0],taballv[nombre1-1][1],taballv[nombre2-1][0],taballv[nombre2-1][1],*Img,white);
        //  line(taballv[nombre2-1][0],taballv[nombre2-1][1],taballv[nombre3-1][0],taballv[nombre3-1][1],*Img,white);
         // line(taballv[nombre1-1][0],taballv[nombre1-1][1],taballv[nombre3-1][0],taballv[nombre3-1][1],*Img,white);
         sommet1.clear();
         sommet2.clear();
         sommet3.clear();
         count++;
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
