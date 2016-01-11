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
std::vector<char> tabvx;
std::vector<char> tabvy;
std::vector<char> tabvz;
std::vector<int> tabv;
std::vector<vector<int> > taballv;

void getLineNumberv(ifstream &myFile){
  std::string myString;
  
  int counterligne = 1;
  while(!myFile.eof())
    {
      getline(myFile, myString);
      //	for(size_t i = 0; i<myString.size(); ++i){
      int taille = myString.size() & INT_MAX;
      if(myString[1] == 'v'&& myString[2] == ' '){
	int i = 2;
	while(myString[i]==' '){
	  i++;
	}
	while(myString[i]!=' '){
	  char c = myString[i];
	  
	  tabvx.push_back(c);
	  i++;
	}
	while(myString[i]==' '){
	  i++;
	}
	while(myString[i]!=' '){
	  char c = myString[i];
	  
	  tabvy.push_back(c);
	  i++;
	}
	while(myString[i]==' '){
	  i++;
	}
	while(i<(taille+1)){
	  char c = myString[i];
	  
	  tabvz.push_back(c);
	  i++;
	}
	
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
      tabv.push_back(nombrex);
      tabv.push_back(nombrey);
      tabv.push_back(nombrez);
      taballv.push_back(tabv);

      // ICI A OCNTINUER BABY
      counter++;
      
    }
  myFile.close();
}


void parsefile(ifstream &file){
  std::string line;
  std::string line2;
  
  // int nbline = getLineNumberv(ifstream file);
 
  file.open("african_head.obj");
  getLineNumberv(file);
  //getline(file, line);

  /*  for(size_t i; i<line.size(); i++){
    char c = line[i];
    printf("carac %lo : %c",i,c);
    }*/
  /* while(!file.eof()){
    
 
    getline(file, line);
    if(line.at(0) == 'v'){
      
      for(size_t i; i<line.size(); i++){
	
	getline(file, line2);
	printf("%s \n",line1.c_str());
	printf("%s",line2.c_str());

      }
      }*/
}

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

int main(int argc, char** argv) {
  ifstream file;
  parsefile(file);
  TGAImage image(800, 800, TGAImage::RGB);
  //image.set(52, 41, red);
  line(100,200,700,400,image,red);
  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  image.write_tga_file("output.tga");
  return 0;
}
