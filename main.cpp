#include "color.h"
#include "ray.h"
#include "vec3.h"

// cmake --build build
// .\build\Debug\CMakeTarget.exe | set-content image.ppm -encoding String

double hit_sphere(const point3& center, double radius, const ray& r) {
	/*
		Q = origin of ray
		C = center of sphere
		d = direction of ray
		r = radius of sphere
	*/
	// C-Q
	vec3 oc = center - r.origin();
	// a=d⋅d
	//auto a = dot(r.direction(), r.direction());
	auto a = r.direction().length_squared();

	// b=−2d⋅(C−Q)
	//auto b = -2.0 * dot(r.direction(), oc);

	// h=b/−2=d⋅(C−Q)
	auto h = dot(r.direction(), oc);

	// c=(C−Q)⋅(C−Q)−r^2
	//auto c = dot(oc, oc) - radius * radius;
	auto c = oc.length_squared() - radius * radius;
	//auto discriminant = b * b - 4 * a * c;

	auto discriminant = h * h - a * c;

	if (discriminant < 0) {
		return -1.0;
	}
	else {
		//return (-b - std::sqrt(discriminant) ) / (2.0*a);
		return (h - std::sqrt(discriminant)) / a;
	}
}

// blendedValue=(1−a)⋅startValue+a⋅endValue,
color ray_color(const ray& r) {
	auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
	if (t > 0.0) {
		vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
		return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto a = 0.5 * (unit_direction.y() + 1.0);

	return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
	// image

	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 400;

	// calculate the image height, and ensure that it's at least 1
	int image_height = int(image_width / aspect_ratio);
	image_height = (image_height < 1) ? 1 : image_height;

	// camera

	auto focal_length = 1.0;
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (double(image_width) / image_height);
	auto camera_center = point3(0, 0, 0);

	// calculate the vectors across the horizontal and down the vertical viewport edges
	auto viewport_u = vec3(viewport_width, 0, 0);
	auto viewport_v = vec3(0, -viewport_height, 0);

	// calculate the horizontal and vertical delta vectors from pixel to pixel
	auto pixel_delta_u = viewport_u / image_width;
	auto pixel_delta_v = viewport_v / image_height;

	// calculate the location of the upper left pixel
	auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
	auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	// render

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < image_height; j++) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		for (int i = 0; i < image_width; i++) {
			auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
			auto ray_direction = pixel_center - camera_center;
			ray r(camera_center, ray_direction);

			color pixel_color = ray_color(r);
			write_color(std::cout, pixel_color);
		}
	}

	std::clog << "\rDone.                  \n";


	return 0;
}
