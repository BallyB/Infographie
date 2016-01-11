#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <math.h>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	TGAImage _normalmap;
	TGAImage _diffusemap;
	TGAImage _specularmap;
	std::vector<Vec3f > _verts;
	std::vector<Vec3f > _norms;
	std::vector<Vec2f > _uv;
	std::vector<std::vector<Vec3i > > _faces;
	void resize();
	void load_texture(std::string filename, const char *suffix, TGAImage &img);
	TGAImage &max_texture();
public:
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f norm(int i);
	Vec2i uv(int i);
	std::vector<Vec3i > face(int idx);
	float specular(Vec2i uv);
	TGAColor diffuse(Vec2i uv);
	Vec3f normal(Vec2i uv);
	Model(const char *filename);
	~Model();
};
#endif //__MODEL_H__
