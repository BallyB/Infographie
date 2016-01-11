#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "model.h"

int Model::nverts() {
	return (int)_verts.size();
}

int Model::nfaces() {
	return (int)_faces.size();
}

Model::Model(const char *filename) : _verts(), _norms(), _uv(), _faces() {
	std::ifstream in;
	in.open (filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;
	while(!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.find("v ")) {
			iss >> trash;
			Vec3f v;
			for (int i=0;i<3;i++) iss >> v.raw[i];
			_verts.push_back(v);
		} else if (!line.find("vn ")) {
			iss >> trash >> trash;
			Vec3f n;
			for (int i=0;i<3;i++) iss >> n.raw[i];
			_norms.push_back(n);
		} else if (!line.find("vt ")) {
			iss >> trash >> trash;
			Vec2f uv;
			for (int i=0;i<2;i++) iss >> uv.raw[i];
			_uv.push_back(uv);
		} else if (!line.find("f ")) {
			Vec3i fe;
			std::vector<Vec3i> f;
			iss >> trash;
			while (iss >> fe.ivert >> trash >> fe.iuv >> trash >> fe.inorm) {
				for (int i=0; i<3; i++) fe.raw[i]--; // in wavefront obj all indices start at 1, not zero
				f.push_back(fe);
			}
			_faces.push_back(f);
		}
	}
	std::cerr << "v# " << _verts.size() << " n# " << _norms.size() << ", uv# " << _uv.size() << " f# "  << _faces.size() << std::endl;
	resize();
	load_texture(filename, "_nm.tga", _normalmap);
	load_texture(filename, "_diffuse.tga", _diffusemap);
	load_texture(filename, "_spec.tga", _specularmap);
	TGAImage &max = max_texture();
	TGAImage *imgs[] = {&_normalmap, &_diffusemap, &_specularmap};
	for (int i=0; i<3; i++) {
		imgs[i]->scale(max.get_width(), max.get_height());
	}
}

TGAImage & Model::max_texture() {
	TGAImage *imgs[] = {&_normalmap, &_diffusemap, &_specularmap};
	TGAImage &max = _normalmap;
	for (int i=0; i<3; i++) {
		if (max.get_width()<imgs[i]->get_width()) max = *(imgs[i]);
	}
	return max;
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot!=std::string::npos) {
		texfile = texfile.substr(0,dot) + std::string(suffix);
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

Model::~Model() {
}

std::vector<Vec3i> Model::face(int idx) {
	return _faces[idx];
}

void Model::resize() {
	Vec3f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
	Vec3f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	for (int i=0; i<nverts(); i++) {
		Vec3f v = vert(i);
		for (int j=0; j<3; j++) {
			bboxmin.raw[j] = std::min(bboxmin.raw[j], v.raw[j]);
			bboxmax.raw[j] = std::max(bboxmax.raw[j], v.raw[j]);
		}
	}
	float maxside = -std::numeric_limits<float>::max();
	for (int j=0; j<3; j++) maxside = std::max(maxside, bboxmax.raw[j]-bboxmin.raw[j]);
	Vec3f center = (bboxmax+bboxmin)*.5f;
	for (int i=0; i<nverts(); i++) {
		_verts[i] = (vert(i)-center)*(2.f/maxside);
	}
}

Vec2i Model::uv(int i) {
	return Vec2i(_uv[i].u*_normalmap.get_width(), _uv[i].v*_normalmap.get_height());
}

Vec3f Model::vert(int i) {
	return _verts[i];
}

Vec3f Model::norm(int i) {
	return _norms[i];
}

float Model::specular(Vec2i uv) {
	return _specularmap.get(uv.u, uv.v).b/1.f;
}

TGAColor Model::diffuse(Vec2i uv) {
	return _diffusemap.get(uv.u, uv.v);
}

Vec3f Model::normal(Vec2i uv) {
	TGAColor c = _normalmap.get(uv.u, uv.v);
	Vec3f res;
	for (int i=0; i<3; i++)
		res.raw[2-i] = (float)c.raw[i]/255.f*2.f - 1.f;
	return res;
}

