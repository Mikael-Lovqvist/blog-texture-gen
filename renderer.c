#include "math.c"
#include "colors.c"

//For future reference: https://openexr.readthedocs.io/en/latest/OpenEXRCoreAPI.html

typedef struct rendering_point {
	vec2 position;
	float weight;
} rendering_point;

typedef struct rendering_settings {
	int num_points;
	rendering_point* points;
} rendering_settings;


void voronoi_render_line(pixel_format frame[HEIGHT][WIDTH], int y, rendering_settings* rs) {

	pixel_format* row = frame[y];
	for (int x=0; x<WIDTH; x++) {

		float closest_distance = 0;
		int closest_point = -1;

		vec2 check_points[4];
		vec2 main_point = v2(
			(float)x / (float)(WIDTH-1),
			(float)y / (float)(HEIGHT-1)
		);

		vec2 cp_offset;

		//Check point 1
		if ((main_point.x < 0.5) && (main_point.y < 0.5)) {	//Top left quadrant
			cp_offset = v2(1, 1);
		} else if ((main_point.x > 0.5) && (main_point.y < 0.5)) {	//Top right quadrant
			cp_offset = v2(-1, 1);
		} else if ((main_point.x < 0.5) && (main_point.y > 0.5)) {	//Bottom left quadrant
			cp_offset = v2(1, -1);
		} else {	//Bottom right quadrant
			cp_offset = v2(-1, -1);
		}

		check_points[0] = main_point;
		check_points[1] = v2(main_point.x + cp_offset.x, main_point.y);
		check_points[2] = v2(main_point.x, main_point.y + cp_offset.y);
		check_points[3] = v2(main_point.x + cp_offset.x, main_point.y + cp_offset.y);


		for (int i=0; i<rs->num_points; i++) {

			float this_distance = magn_v2(sub_v2_v2(check_points[0], rs->points[i].position));
			for (int cp=1; cp<4; cp++) {
				float distance_candidate = magn_v2(sub_v2_v2(check_points[cp], rs->points[i].position));
				this_distance = min(this_distance, distance_candidate);
			}

			if ((closest_point == -1) || (this_distance < closest_distance)) {
				closest_point = i;
				closest_distance = this_distance;
			}

		}

		row[x] = frgb(1-closest_distance*50, 1-closest_distance*10, 1-closest_distance*5);

	}


}






void render_line(pixel_format frame[HEIGHT][WIDTH], int y, rendering_settings* rs) {

	pixel_format* row = frame[y];
	for (int x=0; x<WIDTH; x++) {

		vec2 check_points[4];
		vec2 main_point = v2(
			(float)x / (float)(WIDTH-1),
			(float)y / (float)(HEIGHT-1)
		);

		vec2 cp_offset;

		//Check point 1
		if ((main_point.x < 0.5) && (main_point.y < 0.5)) {	//Top left quadrant
			cp_offset = v2(1, 1);
		} else if ((main_point.x > 0.5) && (main_point.y < 0.5)) {	//Top right quadrant
			cp_offset = v2(-1, 1);
		} else if ((main_point.x < 0.5) && (main_point.y > 0.5)) {	//Bottom left quadrant
			cp_offset = v2(1, -1);
		} else {	//Bottom right quadrant
			cp_offset = v2(-1, -1);
		}

		check_points[0] = main_point;
		check_points[1] = v2(main_point.x + cp_offset.x, main_point.y);
		check_points[2] = v2(main_point.x, main_point.y + cp_offset.y);
		check_points[3] = v2(main_point.x + cp_offset.x, main_point.y + cp_offset.y);

		double influence_radius = 0.25;	// 1.0 is entire texture (sqrt(2))
		double gain=100000;
		double bias=0.5;

		double cumulative_weight = 0;

		for (int i=0; i<rs->num_points; i++) {

			float this_distance = magn_v2(sub_v2_v2(check_points[0], rs->points[i].position));
			for (int cp=1; cp<4; cp++) {
				float distance_candidate = magn_v2(sub_v2_v2(check_points[cp], rs->points[i].position));
				this_distance = min(this_distance, distance_candidate);
			}

			cumulative_weight += pow(max(0, influence_radius - ((double)this_distance / sqrt(2))), 1) * rs->points[i].weight / (double) (WIDTH * HEIGHT) ;

		}



		//row[x] = frgb(bias+cumulative_weight*50*gain, bias+cumulative_weight*10*gain, bias+cumulative_weight*5*gain);
		row[x] = frgb(bias+cumulative_weight*gain, bias+cumulative_weight*gain, bias+cumulative_weight*gain);

	}


}
