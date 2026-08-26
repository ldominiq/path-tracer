#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rt.h"

class sphere : public hittable {
	public:
		// Stationary sphere
		sphere(const point3& static_center, double radius, shared_ptr<material> mat)
			: center(static_center, vec3(0,0,0)), radius(std::fmax(0, radius)), mat(mat) {}

		// Moving sphere
		sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat)
			: center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat) {}


		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			/*
				Q = origin of ray
				C = center of sphere
				d = direction of ray
				r = radius of sphere
			*/
			// C-Q
			point3 current_center = center.at(r.time());
			vec3 oc = current_center - r.origin();
			// a=d⋅d
			auto a = r.direction().length_squared();

			// h=b/−2=d⋅(C−Q)
			auto h = dot(r.direction(), oc);

			// c=(C−Q)⋅(C−Q)−r^2
			auto c = oc.length_squared() - radius * radius;

			auto discriminant = h * h - a * c;

			if (discriminant < 0)
				return false;

			auto sqrtd = std::sqrt(discriminant);

			// find the  nearest root that lies in the acceptable range
			auto root = (h - sqrtd) / a;
			if (!ray_t.surrounds(root)) {
				root = (h + sqrtd) / a;
				if (!ray_t.surrounds(root))
					return false;
			}

			rec.t = root;
			rec.p = r.at(rec.t);
			vec3 outward_normal = (rec.p - current_center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat = mat;

			return true;
		}

	private:
		ray center;
		double radius;
		shared_ptr<material> mat;
};


#endif