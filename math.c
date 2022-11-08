#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#define v2(x, y) (vec2){x, y}
#define v3(x, y, z) (vec3){x, y, z}
#define iv3(x, y, z) (ivec3){x, y, z}
#define r3(pos, dir) (ray3){pos, dir}
#define m3x3(xx, xy, xz, yx, yy, yz, zx, zy, zz) (mat3x3){xx, xy, xz, yx, yy, yz, zx, zy, zz}

#define max(v1, v2) (((v1) > (v2)) ? (v1) : (v2))
#define max_3(v1, v2, v3) max(max(max(v1, v2), v3)
#define max_4(v1, v2, v3, v4) max(max(max(v1, v2), v3), v4)

#define min(v1, v2) (((v1) < (v2)) ? (v1) : (v2))
#define min_3(v1, v2, v3) min(min(min(v1, v2), v3)
#define min_4(v1, v2, v3, v4) min(min(min(v1, v2), v3), v4)


#define m3x3_identity m3x3( 	\
	1, 0, 0,					\
	0, 1, 0,					\
	0, 0, 1						\
)


#ifndef M_TAU
	#define M_TAU (M_PI + M_PI)
#endif

typedef struct pixel_format {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} pixel_format;

typedef struct vec2 {
	float x;
	float y;
} vec2;

typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3;

typedef struct ivec3 {
	int x;
	int y;
	int z;
} ivec3;

typedef struct mat3x3 {
	float xx;
	float xy;
	float xz;

	float yx;
	float yy;
	float yz;

	float zx;
	float zy;
	float zz;

} mat3x3;

typedef struct ray3 {
	vec3 pos;
	vec3 dir;
} ray3;

typedef struct cuboid_type {
	vec3 pos;
	mat3x3 matrix;
	vec3 size;
} cuboid_type;


typedef struct camera_type {
	vec3 pos;
	vec2 size;
	float depth;
	mat3x3 matrix;

} camera_type;





float magn_v3(vec3 v) {
	return sqrt(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2));
}

float magn_v2(vec2 v) {
	return sqrt(powf(v.x, 2) + powf(v.y, 2));
}

vec3 sub_v3_v3(vec3 a, vec3 b) {
	return v3(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec2 sub_v2_v2(vec2 a, vec2 b) {
	return v2(a.x - b.x, a.y - b.y);
}

vec3 add_v3_v3(vec3 a, vec3 b) {
	return v3(a.x + b.x, a.y + b.y, a.z + b.z);
}

float dot_v3(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 mul_v3_s(vec3 v, float s) {
	return v3(v.x * s, v.y * s, v.z * s);
}

ivec3 div_iv3_s(ivec3 v, int s) {
	return iv3(v.x / s, v.y / s, v.z / s);
}

ivec3 iv3_mod(ivec3 v, int s) {
	return iv3(v.x % s, v.y % s, v.z % s);
}

vec3 norm_v3(vec3 v) {
	return mul_v3_s(v, 1.0 / magn_v3(v));
}


vec3 v3_sum3(vec3 a, vec3 b, vec3 c) {
	return v3(a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z);
}

ray3 calc_camera_ray(camera_type camera, vec2 canvas_pos) {

	vec2 cp = v2(
		(canvas_pos.x - camera.size.x * .5) / camera.depth,
		(canvas_pos.y - camera.size.y * .5) / camera.depth
	);

	vec3 x_axis = v3(camera.matrix.xx, camera.matrix.xy, camera.matrix.xz);
	vec3 y_axis = v3(camera.matrix.yx, camera.matrix.yy, camera.matrix.yz);
	vec3 z_axis = v3(camera.matrix.zx, camera.matrix.zy, camera.matrix.zz);

	return r3(
		camera.pos,
		norm_v3(
			v3_sum3(
				mul_v3_s(x_axis , cp.x),
				mul_v3_s(y_axis, cp.y),
				z_axis
			)
		)
	);
}




mat3x3 m3x3_roll(float a) {
	return m3x3(
		1, 			0, 			0,
		0, 			cos(a),		-sin(a),
		0,			sin(a),		cos(a)
	);
}

mat3x3 m3x3_pitch(float a) {
	return m3x3(
		cos(a),		0,			sin(a),
		0,			1,			0,
		-sin(a),	0,			cos(a)
	);
}

mat3x3 m3x3_yaw(float a) {
	return m3x3(
		cos(a),		-sin(a),	0,
		sin(a),		cos(a),		0,
		0,			0,			1
	);
}


mat3x3 mul_m3x3_m3x3(mat3x3 a, mat3x3 b) {
	return m3x3(
		a.xx * b.xx + a.xy * b.yx + a.xz * b.zx, a.xx * b.xy + a.xy * b.yy + a.xz * b.zy, a.xx * b.xz + a.xy * b.yz + a.xz * b.zz,
		a.yx * b.xx + a.yy * b.yx + a.yz * b.zx, a.yx * b.xy + a.yy * b.yy + a.yz * b.zy, a.yx * b.xz + a.yy * b.yz + a.yz * b.zz,
		a.zx * b.xx + a.zy * b.yx + a.zz * b.zx, a.zx * b.xy + a.zy * b.yy + a.zz * b.zy, a.zx * b.xz + a.zy * b.yz + a.zz * b.zz
	);
}

mat3x3 m3x3_product_3(mat3x3 m1, mat3x3 m2, mat3x3 m3) {
	return mul_m3x3_m3x3(mul_m3x3_m3x3(m1, m2), m3);
}





typedef enum cuboid_side {
	CS_NONE=0,
	CS_XN,
	CS_XP,
	CS_YN,
	CS_YP,
	CS_ZN,
	CS_ZP,
} cuboid_side;


typedef struct cuboid_intersect_result_type {
	cuboid_side side;
	vec2 side_position;
	vec3 intersect_position;
} cuboid_intersect_result_type;


typedef struct plane_intersect_result_type {
	bool hit;
	vec2 local_position;
	vec3 intersect_position;
} plane_intersect_result_type;


typedef struct plane_type {
	vec3 pos;
	mat3x3 matrix;
} plane_type;


cuboid_side vec3_cardinal(vec3 v) {
	float mv;
	cuboid_side ms = CS_NONE;
	if (v.x >= 0) {
		mv = v.x;
		ms = CS_XP;
	} else {
		mv = -v.x;
		ms = CS_XN;
	}

	if (v.y >= 0) {
		if (v.y > mv) {
			mv = v.y;
			ms = CS_YP;
		}
	} else {
		if (-v.y > mv) {
			mv = -v.y;
			ms = CS_YN;
		}
	}

	if (v.z >= 0) {
		if (v.z > mv) {
			mv = v.z;
			ms = CS_ZP;
		}
	} else {
		if (-v.z > mv) {
			mv = -v.z;
			ms = CS_ZN;
		}
	}

	return ms;

}

plane_intersect_result_type ray_plane_intersect(ray3 ray, plane_type plane) {

	//Get plane axises
	vec3 x_axis = v3(plane.matrix.xx, plane.matrix.xy, plane.matrix.xz);
	vec3 y_axis = v3(plane.matrix.yx, plane.matrix.yy, plane.matrix.yz);
	vec3 z_axis = v3(plane.matrix.zx, plane.matrix.zy, plane.matrix.zz);

	float z_plane_dot = dot_v3(z_axis, ray.dir);

	if (z_plane_dot > 0) {

		vec3 ray_plane = sub_v3_v3(plane.pos, ray.pos);

		float ray_plane_distance = magn_v3(ray_plane) * dot_v3(norm_v3(ray_plane), z_axis);
		float ray_intersection_distance = ray_plane_distance / dot_v3(ray.dir, z_axis);


		vec3 hit_pos = add_v3_v3(ray.pos, mul_v3_s(ray.dir, ray_intersection_distance));
		vec3 local_pos = sub_v3_v3(hit_pos, plane.pos);
		vec2 plane_pos = v2(dot_v3(local_pos, x_axis), dot_v3(local_pos, y_axis));

		return (plane_intersect_result_type) {
			.hit = true,
			.intersect_position = hit_pos,
			.local_position = plane_pos,
		};
	} else {
		return (plane_intersect_result_type) {
			.hit = false,
		};
	}

}

cuboid_intersect_result_type ray_cuboid_intersect(ray3 ray, cuboid_type cuboid) {


	// Get cuboid axises
	vec3 x_axis = v3(cuboid.matrix.xx, cuboid.matrix.xy, cuboid.matrix.xz);
	vec3 y_axis = v3(cuboid.matrix.yx, cuboid.matrix.yy, cuboid.matrix.yz);
	vec3 z_axis = v3(cuboid.matrix.zx, cuboid.matrix.zy, cuboid.matrix.zz);

	plane_intersect_result_type plane_intersection;

	// X+
	plane_intersection = ray_plane_intersect(ray, (plane_type) {
		.pos = add_v3_v3(cuboid.pos, mul_v3_s(x_axis, cuboid.size.x * -.5)),
		.matrix = m3x3(
			z_axis.x, z_axis.y, z_axis.z,
			y_axis.x, y_axis.y, y_axis.z,
			x_axis.x, x_axis.y, x_axis.z
		),
	});

	if (plane_intersection.hit) {
		if ((fabs(plane_intersection.local_position.x) <= cuboid.size.z * .5) && (fabs(plane_intersection.local_position.y) <= cuboid.size.y * .5)) {
			return (cuboid_intersect_result_type) {
				.side = CS_XP,
				.intersect_position = plane_intersection.intersect_position,
				.side_position = plane_intersection.local_position,
			};
		}

	}


	// X-
	plane_intersection = ray_plane_intersect(ray, (plane_type) {
		.pos = add_v3_v3(cuboid.pos, mul_v3_s(x_axis, cuboid.size.x * .5)),
		.matrix = m3x3(
			-z_axis.x, -z_axis.y, -z_axis.z,
			-y_axis.x, -y_axis.y, -y_axis.z,
			-x_axis.x, -x_axis.y, -x_axis.z
		),
	});

	if (plane_intersection.hit) {
		if ((fabs(plane_intersection.local_position.x) <= cuboid.size.z * .5) && (fabs(plane_intersection.local_position.y) <= cuboid.size.y * .5)) {
			return (cuboid_intersect_result_type) {
				.side = CS_XN,
				.intersect_position = plane_intersection.intersect_position,
				.side_position = plane_intersection.local_position,
			};
		}

	}

	// Y+
	plane_intersection = ray_plane_intersect(ray, (plane_type) {
		.pos = add_v3_v3(cuboid.pos, mul_v3_s(y_axis, cuboid.size.y * -.5)),
		.matrix = m3x3(
			x_axis.x, x_axis.y, x_axis.z,
			z_axis.x, z_axis.y, z_axis.z,
			y_axis.x, y_axis.y, y_axis.z
		),
	});

	if (plane_intersection.hit) {
		if ((fabs(plane_intersection.local_position.x) <= cuboid.size.x * .5) && (fabs(plane_intersection.local_position.y) <= cuboid.size.z * .5)) {
			return (cuboid_intersect_result_type) {
				.side = CS_YP,
				.intersect_position = plane_intersection.intersect_position,
				.side_position = plane_intersection.local_position,
			};
		}

	}

	// Y-
	plane_intersection = ray_plane_intersect(ray, (plane_type) {
		.pos = add_v3_v3(cuboid.pos, mul_v3_s(y_axis, cuboid.size.y * .5)),
		.matrix = m3x3(
			-x_axis.x, -x_axis.y, -x_axis.z,
			-z_axis.x, -z_axis.y, -z_axis.z,
			-y_axis.x, -y_axis.y, -y_axis.z
		),
	});

	if (plane_intersection.hit) {
		if ((fabs(plane_intersection.local_position.x) <= cuboid.size.x * .5) && (fabs(plane_intersection.local_position.y) <= cuboid.size.z * .5)) {
			return (cuboid_intersect_result_type) {
				.side = CS_YN,
				.intersect_position = plane_intersection.intersect_position,
				.side_position = plane_intersection.local_position,
			};
		}

	}


	// Z+
	plane_intersection = ray_plane_intersect(ray, (plane_type) {
		.pos = add_v3_v3(cuboid.pos, mul_v3_s(z_axis, cuboid.size.z * -.5)),
		.matrix = m3x3(
			x_axis.x, x_axis.y, x_axis.z,
			y_axis.x, y_axis.y, y_axis.z,
			z_axis.x, z_axis.y, z_axis.z
		),
	});

	if (plane_intersection.hit) {
		if ((fabs(plane_intersection.local_position.x) <= cuboid.size.x * .5) && (fabs(plane_intersection.local_position.y) <= cuboid.size.y * .5)) {
			return (cuboid_intersect_result_type) {
				.side = CS_ZP,
				.intersect_position = plane_intersection.intersect_position,
				.side_position = plane_intersection.local_position,
			};
		}

	}

	// Z-
	plane_intersection = ray_plane_intersect(ray, (plane_type) {
		.pos = add_v3_v3(cuboid.pos, mul_v3_s(z_axis, cuboid.size.z * .5)),
		.matrix = m3x3(
			-x_axis.x, -x_axis.y, -x_axis.z,
			-y_axis.x, -y_axis.y, -y_axis.z,
			-z_axis.x, -z_axis.y, -z_axis.z
		),
	});

	if (plane_intersection.hit) {
		if ((fabs(plane_intersection.local_position.x) <= cuboid.size.x * .5) && (fabs(plane_intersection.local_position.y) <= cuboid.size.y * .5)) {
			return (cuboid_intersect_result_type) {
				.side = CS_ZN,
				.intersect_position = plane_intersection.intersect_position,
				.side_position = plane_intersection.local_position,
			};
		}

	}

	return (cuboid_intersect_result_type) {
		.side = CS_NONE,
	};

}





ivec3 ivec3_clamped_v3(vec3 v, ivec3 max) {
	return iv3(
		min((int)v.x, max.x),
		min((int)v.y, max.y),
		min((int)v.z, max.z)
	);
}




ivec3 iv3_v3(vec3 v) {
	return iv3((int)v.x, (int)v.y, (int)v.z);
}

ivec3 iv3_v3_floor(vec3 v) {
	return iv3(floor(v.x), floor(v.y), floor(v.z));
}


ivec3 ivec3_clamped_v3_floor(vec3 v, ivec3 max) {
	ivec3 result = iv3_v3_floor(v);
	result.x = min(result.x, max.x);
	result.y = min(result.y, max.y);
	result.z = min(result.z, max.z);

}


vec3 get_cuboid_side_normal(cuboid_side s) {
	switch (s) {
		case CS_XP:		return v3(1, 0, 0);
		case CS_XN:		return v3(-1, 0, 0);
		case CS_YP:		return v3(0, 1, 0);
		case CS_YN:		return v3(0, -1, 0);
		case CS_ZP:		return v3(0, 0, 1);
		case CS_ZN:		return v3(0, 0, -1);
		default:		return v3(0, 0, 0);
	}
}

