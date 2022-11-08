#include "math.c"
#include "colors.c"

typedef struct rendering_point {
	vec2 position;
	float weight;
} rendering_point;

typedef struct rendering_settings {
	int num_points;
	rendering_point* points;
} rendering_settings;


void render_line(pixel_format frame[HEIGHT][WIDTH], int y, rendering_settings* rs) {

	pixel_format* row = frame[y];
	for (int x=0; x<WIDTH; x++) {

		float closest_distance = 0;
		int closest_point = -1;
		vec2 this_point = v2(
			(float)x / (float)(WIDTH-1),
			(float)y / (float)(HEIGHT-1)
		);

		for (int i=0; i<rs->num_points; i++) {
			float this_distance = magn_v2(sub_v2_v2(this_point, rs->points[i].position));

			if ((closest_point == -1) || (this_distance < closest_distance)) {
				closest_point = i;
				closest_distance = this_distance;
			}

		}

		row[x] = frgb(1-closest_distance*50, 1-closest_distance*10, 1-closest_distance*5);

	}


}
