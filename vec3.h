#ifndef VEC3_H
#define VEC3_H

#include "rt.h"


class vec3 {
	public:
		double e[3];

		vec3() : e{0,0,0} {}
		vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

		double x() const { return e[0]; }
		double y() const { return e[1]; }
		double z() const { return e[2]; }

		vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
		double operator[](int i) const { return e[i]; }
		double& operator[](int i) { return e[i]; }

		vec3& operator+=(const vec3& v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			return *this;
		}

		vec3& operator*=(double t) {
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			return *this;
		}

		vec3& operator/=(double t) {
			return *this *= 1 / t;
		}

		double length() const {
			return std::sqrt(length_squared());
		}

		double length_squared() const {
			return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
		}

		bool near_zero() const {
			// return true if the vector is close to zero in all dimensions.
			auto s = 1e-8;
			return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
		}

		static vec3 random() {
			return vec3(random_double(), random_double(), random_double());
		}

		static vec3 random(double min, double max) {
			return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
		}
};

// useful for geometric clarity in the code
using point3 = vec3;


// vector utility functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
	return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0]
		 + u.e[1] * v.e[1]
		 + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// v / |v| -> |v| = sqrt(a^2 + b^2)
inline vec3 unit_vector(const vec3& v) {
	return v / v.length();
}

inline vec3 random_unit_vector() {
	while (true) {
		auto p = vec3::random(-1, 1);
		auto lensq = p.length_squared();
		if (1e-160 < lensq && lensq <= 1)
			return p / sqrt(lensq);
	}
}

inline vec3 random_on_hemisphere(const vec3& normal) {
	vec3 on_unit_sphere = random_unit_vector();
	if (dot(on_unit_sphere, normal) > 0.0) // in the same hemisphere as the normal
		return on_unit_sphere;
	else
		return -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

// snell's law: n * sin(theta) = n' * sin(theta')
// instead of messing with angles, we build the refracted direction from two pieces:
// r_out_perp (lies in the surface, perpendicular to n) and r_out_parallel (along n)
// uv must be a UNIT vector, etai_over_etat is the ratio n/n' (1.0/1.5 entering glass, 1.5 leaving)
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	// for unit vectors a.b = cos(angle between them)
	// uv points into the surface while n points back out, so dot(uv, n) is negative -> negate uv
	// fmin clamps float drift that could push this slightly above 1
	auto cos_theta = std::fmin(dot(-uv, n), 1.0);

	// (uv + cos_theta * n) cancels uv's normal component, leaving only the part lying in the
	// surface -> for a unit uv that leftover has length sin(theta)
	// scaling it by n/n' makes its length sin(theta'), which IS snell's law rewritten as vectors
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);

	// the result has to be a unit vector, so pythagoras gives the missing length:
	// |parallel|^2 = 1 - |perp|^2 -> negative because it points *through* the surface, opposite n
	// fabs only guards float drift; it also silently hides total internal reflection
	// (when etai_over_etat * sin(theta) > 1 there is no valid refraction)
	vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;

	// glue the two halves back together
	return r_out_perp + r_out_parallel;
}

#endif
