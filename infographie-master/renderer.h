#include <iostream>
#include <vector>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

class Renderer : public TGAImage {
public:
	Renderer(int w, int h, int bpp);
	void render(Model &model, int *shadow_buffer);
	void set_light(Vec3f light);
	void set_matrices(fmatrix ModelViewMatrix, fmatrix ShadowModelViewMatrix=fmatrix::identity(4));

	const int ZBUF_RESOLUTION;
	std::vector<int> _zbuffer;
private:
	struct ScreenVertex {
		Vec3i p;
		Vec2i uv;

		ScreenVertex() {}
		ScreenVertex(Vec3i P, Vec2i UV) : p(P), uv(UV) {}
		ScreenVertex operator * (float f) const { return ScreenVertex(p*f, uv*f); }
		ScreenVertex operator + (const ScreenVertex &v) const { return ScreenVertex(p+v.p, uv+v.uv); }
		ScreenVertex operator - (const ScreenVertex &v) const { return ScreenVertex(p-v.p, uv-v.uv); }
		void fragment_shader(Model &model, int *shadow_buffer, Renderer *r);
	};
	void rasterize(ScreenVertex *triangle, Model &model, int *shadow_buffer);
	Vec3i vertex_shader(Model &model, int ivert);

	Vec3f light_dir;
	fmatrix ModelViewProjectionViewportMatrix;
	fmatrix NormalTransformMatrix;
	fmatrix ShadowMappingMatrix;
	fmatrix _Projection;
	fmatrix _Viewport;
};

