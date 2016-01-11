#include "renderer.h"

void Renderer::rasterize(ScreenVertex *t, Model &model, int *shadow_buffer) {
	if (t[0].p.y==t[1].p.y && t[0].p.y==t[2].p.y) return;
	if (t[0].p.y>t[1].p.y) std::swap(t[0], t[1]);
	if (t[0].p.y>t[2].p.y) std::swap(t[0], t[2]);
	if (t[1].p.y>t[2].p.y) std::swap(t[1], t[2]);
	int total_height = t[2].p.y-t[0].p.y;
	for (int i=0; i<total_height; i++) {
		bool second_seg = i>t[1].p.y-t[0].p.y || t[1].p.y==t[0].p.y;
		int seg_height = second_seg ? t[2].p.y-t[1].p.y : t[1].p.y-t[0].p.y;
		float alpha = (float)i/(float)total_height;
		float beta  = (float)(i-(second_seg ? t[1].p.y-t[0].p.y : 0))/seg_height; // TODO check for division by zero
		ScreenVertex A = t[0] + (t[2]-t[0])*alpha;
		ScreenVertex B = t[0+second_seg] + (t[1+second_seg]-t[0+second_seg])*beta;
		if (A.p.x>B.p.x) std::swap(A, B);
		for (int j=A.p.x; j<=B.p.x; j++) {
			float phi = B.p.x==A.p.x ? 1.f : (float)(j-A.p.x)/(float)(B.p.x-A.p.x);
			ScreenVertex F = A + (B-A)*phi;
			int idx = j+(t[0].p.y+i)*get_width();
			if (j>=0 && j<get_width() && t[0].p.y+i>=0 && t[0].p.y+i<get_height() && _zbuffer[idx]<F.p.z) {
				_zbuffer[idx] = F.p.z;
				if (!shadow_buffer) continue;
				F.p.x = j; F.p.y = t[0].p.y+i; // a hack to fill holes (due to int cast precision problems)
				F.fragment_shader(model, shadow_buffer, this);
			}
		}
	}
}

void Renderer::ScreenVertex::fragment_shader(Model &model, int *shadow_buffer, Renderer *r) {
	TGAColor color = model.diffuse(uv);

	fmatrix nm = fmatrix(model.normal(uv), 0.)*r->NormalTransformMatrix;
	Vec3f normal = Vec3f(nm[0][0], nm[0][1], nm[0][2]).normalize(); // no need to divide by normal[0][3], normalizing anyway
	float diffuse_coeff = std::max(normal*r->light_dir, 0.0f);

	Vec3f reflected_light = normal*(normal*r->light_dir*2.f) - r->light_dir;

	float shadow = 0.4;
	fmatrix P = fmatrix(Vec3f(p.x, p.y, p.z), 1.) * r->ShadowMappingMatrix;
	Vec3i iP = Vec3i(P[0][0]/P[0][3], P[0][1]/P[0][3], P[0][2]/P[0][3]);
	if (iP.x>=0 && iP.y>=0 && iP.x<r->get_width() && iP.y<r->get_height()) {
		int shadow_depth = shadow_buffer[iP.x + r->get_width()*iP.y];
		if (shadow_depth<iP.z+10) shadow = 1.; // +10 to avoid z-fighting, especially awful due to mutliple int casts
	}

	float specularLightWeighting =  pow(std::max(reflected_light.z/reflected_light.norm(), 0.0f), model.specular(uv));
	for (int c=0; c<3; c++) color.raw[c] = std::min(5 + shadow*color.raw[c]*(diffuse_coeff+ .9f*specularLightWeighting), 255.f);

	r->set(p.x, p.y, color);
}

Vec3i Renderer::vertex_shader(Model &model, int ivert) {
	fmatrix v = fmatrix(model.vert(ivert), 1.) * ModelViewProjectionViewportMatrix;
	return Vec3i(v[0][0]/v[0][3], v[0][1]/v[0][3], v[0][2]/v[0][3]);
}

void Renderer::set_light(Vec3f light) {
	fmatrix lm = fmatrix(light, 0)*_Projection * _Viewport;
	light_dir = Vec3f(lm[0][0], lm[0][1], lm[0][2]).normalize(); //no need to divide by lm[0][3], normalizing anyway
}

void Renderer::set_matrices(fmatrix ModelViewMatrix, fmatrix ShadowModelViewMatrix) {
	ModelViewProjectionViewportMatrix = ModelViewMatrix * _Projection * _Viewport;
	NormalTransformMatrix = ModelViewProjectionViewportMatrix.inverse().transpose();
	ShadowMappingMatrix = NormalTransformMatrix.transpose()*ShadowModelViewMatrix*_Projection*_Viewport;
}

void Renderer::render(Model &model, int *shadow_buffer) {
	std::vector<Vec3i> verts(model.nverts());
	for (int i=0; i<model.nverts(); i++) {
		verts[i] = vertex_shader(model, i);
	}
	clear();
	_zbuffer = std::vector<int>(get_width()*get_height(), -1);
	for (int i=0; i<model.nfaces(); i++) {
		std::vector<Vec3i> f = model.face(i);
		ScreenVertex triangle[3];
		for (int t=0; t<3; t++) {
			triangle[t] = ScreenVertex(verts[f[t].ivert], model.uv(f[t].iuv));
		}
		rasterize(triangle, model, shadow_buffer);
	}
}

Renderer::Renderer(int w, int h, int bpp) : TGAImage(w, h, bpp), ZBUF_RESOLUTION(1024), _zbuffer(w*h, -1), light_dir(0,0,1) {
	_Viewport = _Projection = fmatrix::identity(4);
	_Projection[2][3] = -1./16.; // if set to -1 will create non-invertible modelviewprojectionviewport matrix
	_Viewport[0][0] = _Viewport[3][0] = w/2;
	_Viewport[1][1] = _Viewport[3][1] = h/2;
	_Viewport[2][2] = _Viewport[3][2] = ZBUF_RESOLUTION/2;
	ShadowMappingMatrix = NormalTransformMatrix = ModelViewProjectionViewportMatrix = fmatrix::identity(4);
}

