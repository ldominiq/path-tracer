#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
	public:
		sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

		bool hit(const ray& r, double ray_tmin, double ray_tmax, const hit_record& rec) const override {
			/*
				Q = origin of ray
				C = center of sphere
				d = direction of ray
				r = radius of sphere
			*/
			// C-Q
			vec3 oc = center - r.origin();
			// a=d⋅d
			auto a = r.direction().length_squared();

			// h=b/−2=d⋅(C−Q)
			auto h = dot(r.direction(), oc);

			// c=(C−Q)⋅(C−Q)−r^2
			auto c = oc.length_squared() - radius * radius;

			auto discriminant = h * h - a * c;

			if (discriminant < 0)
				return false

			auto sqrtd = std::sqrt(discriminant);

			// find the  nearest root that lies in the acceptable range
			auto root = (h - sqrtd) / a;
			if (root <= ray_tmin || ray_tmax <= root) {
				root = (h + sqrtd) / a;
				if (root <= ray_tmin || ray_tmax <= root)
					return false;
			}

			rec.t = root;
			rec.p = r.at(rec.t);
			rec.normal = (rec.p - center) / radius;

			return true;
		}

	private:
		point3 center;
		double radius;
};


#endif