#include <stdio.h>;
#include <SDL.h>;
#include <stdbool.h>;
#include <stdint.h>;
#include <stdlib.h>;//similar to <malloc.h> which takes care of memory allocation.
#include <SDL_video.h>;
#include "vector.h";//create function implemenations for vector operations.
#include "mesh.h";
//Used to create a void pointer^^^. A void pointer is a pointer that is untyped, meaning it can handle any type that can be type casted.

//Preprocessor directives ^^

//Global variables

SDL_Window* window; //pointer to a window struct
SDL_Renderer* renderer; //pointer to an SDL_Renderer struct	
SDL_Texture* texture; //pointer to an SDL_Texture struct
bool is_running = false;
uint32_t* color_buffer; //pointer to an array of unsigned 32 bit integers. We need 32 bits since that translates to 4 bytes for r, g, b, and alpha.
int window_width = 1920;
int window_height = 1080;
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };
vec3_t camera_position = { 0, 0, -5};
//possibly make a global variable for the camera, make is a vec3_t at 0,0,-5
int previous_frame_time = 0;//assigned to SDL_GetTicks();, time from last frame in milliseconds.
#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS) //how long we want to wait.
int t_cnt = 0;
triangle_t triangles_to_render[1000];






bool initialize_windowing_system()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf(stderr, "SDL_Init failed");
		return false;
	}
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;//changed
	window_height = display_mode.h;
	//SDL_SetWindowFullScreen(window, SDL_WINDOW_FULLSCREEN);

	window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS); //Null is the title, since we don't care we are naming it as null, SDL_WINDOWPOS_CENTERED automatically centers the window in the display.

	if (!window)//When window fails it creates a null value. ! implies if window is null.
	{
		printf(stderr, "SDL_WINDOW failed");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);//Returns the address of the renderer struct.
	if (!renderer)
	{
		printf(stderr, "SDL_Renderer failed");
		return false;
	}
	return true;//Once it passes all the checks we return true.
}

void set_up_memory_buffers()
{

	color_buffer = (uint32_t*)malloc(window_width * window_height * sizeof(uint32_t));
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
}
//MIDTERM QUESTION: HOW TO GET COLOR BUFFER AND 6X6 PIXEL GRID (0-5, 0-5) (to solve a pixel grid, multiply width by y and then add the x column.)



void clear_color_buffer(uint32_t color)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			color_buffer[(y * window_width) + x] = color;
		}
	}
}



void process_keyboard_input()
{
	SDL_Event event; // The data type for this is a UNION, and  NOT a Struct
	SDL_PollEvent(&event); //passing a reference and fills out all the data that involves that Union.
	switch (event.type)
	{
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			is_running = false;
			break;
		}
	}
}

void set_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || x >= window_width) return;
	if (y < 0 || y >= window_height) return;
	color_buffer[x + y * window_width] = color;
}

void DrawDots()
{

	for (int y = 0; y < window_height; y += 25)
	{
		for (int x = 0; x < window_width; x += 25)
		{
			color_buffer[(y * window_width) + x] = 0x00FF00;
		}
	}



}

void DrawRect(int center_x, int center_y, int width, int height, uint32_t color)
{
	int x, y;
	for (x = center_x; x < center_x + width; ++x)
	{
		for (y = center_y; y < center_y + height; ++y)
		{
			set_pixel(x, y, color);
		}
	}
}

void DrawGrid(int center_x, int center_y, int spacing, uint32_t color)
{
	int x, y;
	//vertical lines
	for (x = center_x; x < window_width; x += spacing)
	{
		for (y = 0; y < window_height; y++)
		{
			set_pixel(x, y, color);
		}
	}
	for (x = center_x - spacing; x >= 0; x -= spacing)
	{
		for (y = 0; y < window_height; y++)
		{
			set_pixel(x, y, color);
		}
	}

	// Draw horizontal lines
	for (y = center_y; y < window_height; y += spacing)
	{
		for (x = 0; x < window_width; x++)
		{
			set_pixel(x, y, color);
		}
	}
	for (y = center_y - spacing; y >= 0; y -= spacing)
	{
		for (x = 0; x < window_width; x++)
		{
			set_pixel(x, y, color);
		}
	}
}




/*
TODO this function converts a 3d vector to a 2d vector
by dropping the z paramater, helper function called by
project_model
*/

vec2_t orthographic_project_point(vec3_t point_3d)
{
	vec2_t projected_point = { .x = point_3d.x, .y = point_3d.y };
	return projected_point;
}



vec2_t perspective_project_point(vec3_t pt3d, float scale2d) {
	// Apply camera position
	pt3d = vec3_sub(pt3d, camera_position);

	vec2_t pt2d = { .x = pt3d.x / pt3d.z, .y = pt3d.y / pt3d.z };

	// Scale for screen
	pt2d.x = pt2d.x * scale2d;
	pt2d.y = pt2d.y * scale2d;

	return pt2d;
}




void project_model(void)
{
	triangle_t projected_triangle;
	//face_t triangle;
	for (int i = 0; i < N_MESH_FACES; i++)
	{
		face_t mesh_face = mesh_faces[i];
		vec3_t face_vertices[3];
		face_vertices[0] = mesh_vertices[mesh_face.a - 1];
		face_vertices[1] = mesh_vertices[mesh_face.b - 1];
		face_vertices[2] = mesh_vertices[mesh_face.c - 1];

		vec3_t transformed_vertices[3];
		

		for (int j = 0; j < 3; j++)
		{
			vec3_t transformed_vertex = face_vertices[j];
			transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);
			//transformed_vertex.z -= camera_position.z;

			//translate the vertex away from the camera
			transformed_vertex.z = 5;
			transformed_vertices[j] = transformed_vertex;
			/*vec2_t projected_point = perspective_project_point(transformed_vertex, 1200);
			projected_triangle.points[j] = projected_point;*/
		}

		//Backface culling
		vec3_t vertex_a = transformed_vertices[0];
		vec3_t vertex_b = transformed_vertices[1];
		vec3_t vertex_c = transformed_vertices[2];
		
		//Get the vector subtraction of B - A and C - A
		vec3_t vector_ab = vec3_sub(vertex_b, vertex_a);
		vec3_t vector_ac = vec3_sub(vertex_c, vertex_a);

		//compute the face normal using cross product
		vec3_t normal = vec3_cross(vector_ab, vector_ac);

		//Find the vector between a point in the triangle and the camera origin
		vec3_t camera_ray = vec3_sub(camera_position, normal);

		//Calculate how aligned the camera ray is with the face normal using dot product
		float dot_normal_camera = vec3_dot(camera_ray, normal);

		//Bypass triangles that are looking away from the camera by continuing to next face in main loop
		if (dot_normal_camera <= 0)
		{
			continue;
		}

		triangle_t projected_triangle;

		//Sub loop, project the vertices of current face
		for (int j = 0; j < 3; j++)
		{
			vec2_t projected_point = perspective_project_point(transformed_vertices[j],1200);

			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);

			projected_triangle.points[j] = projected_point;
		}
		triangles_to_render[t_cnt++] = projected_triangle;
	}


	//Rendering 2D, Iterates thru global array of triangles to render and draws them.

	//for (int i = 0; i < t_cnt; i++)
	//{
	//	triangle_t triangle = triangles_to_render[i];
	//	Draw_Rect(triangle.points[0].x, triangle.points[0].y, 5, 5, 0xFF00FF00);
	//	Draw_Rect(triangle.points[1].x, triangle.points[1].y, 5, 5, 0xFF00FF00);
	//	Draw_Rect(triangle.points[2].x, triangle.points[2].y, 5, 5, 0xFF00FF00);
	//	draw_triangle(triangle.points[0].x,triangle.points[0].y,triangle.points[1].x,triangle.points[1].y,triangle.points[2].x,triangle.points[2].y,0xFF00FF00);
	//}
	//t_cnt = 0;
}


//void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
//{
//	float delta_x = x1 - x0;
//	float delta_y = y1 - y0;
//
//	int steps = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : (delta_y);//changed >= abs(delta_x) to delta_y removed the spotted line. Added abs to the last term.
//
//	float x_inc = delta_x / (float)steps;//x increment = delta x/steps
//	float y_inc = delta_y / (float)steps;//y increment = delta y/steps
//
//	float new_x = x0;
//	float new_y = y0;
//	for (int i = 0; i < steps; i++)
//	{
//		set_pixel(round(new_x) /* + (window_width / 2)*/, round(new_y) /* + (window_height / 2)*/, color);//professor calls it as drawpixles(x1,y1);
//		new_x += x_inc;//change to -= for cool effect works only with the old code (remove the declare for newx & newy)
//		new_y += y_inc;//change to -= for cool effect
//	}
//}


void bres_line(int x0, int y0, int x1, int y1, uint32_t color)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	for (;;) {  /* loop */
		set_pixel(x0, y0, color);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	bres_line(x0, y0, x1, y1, color);
	bres_line(x1, y1, x2, y2, color);
	bres_line(x2, y2, x0, y0, color);
}







void update_state()
{
		
		int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
		if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
		{
			SDL_Delay(time_to_wait);
		}

		clear_color_buffer(0x000000);
		//build_cube_model();
		cube_rotation.x += .01;
		cube_rotation.y += .01;
		cube_rotation.z += .01;

		//draw_grid(0xFF777777);
		project_model();
		

		for (int i = 0; i < t_cnt; i++)
		{
			triangle_t triangle = triangles_to_render[i];
			
			/*DrawRect(triangle.points[0].x + (window_width / 2), triangle.points[0].y + (window_height / 2), 5, 5, 0x00FFFFFF);
			DrawRect(triangle.points[1].x + (window_width / 2), triangle.points[1].y + (window_height / 2), 5, 5, 0x00FFFFFF);
			DrawRect(triangle.points[2].x + (window_width / 2), triangle.points[2].y + (window_height / 2), 5, 5, 0x00FFFFFF);*/
			DrawRect(triangle.points[0].x, triangle.points[0].y, 5, 5, 0x00FFFFFF);
			DrawRect(triangle.points[1].x, triangle.points[1].y, 5, 5, 0x00FFFFFF);
			DrawRect(triangle.points[2].x, triangle.points[2].y, 5, 5, 0x00FFFFFF);
			draw_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				0xFF00FF00
			);
			
		}

		t_cnt = 0;

		//to make it a perspective view, take the x value and divide it by the z value to get the top view.
		//to get the side view it has a similar concept, take the y value and divide it by the z value.
		//^^^^This is called the perspective divide^^^^

		previous_frame_time = SDL_GetTicks();
	


}

void run_render_pipeline()
{
	//SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);//background color
	//SDL_RenderClear(renderer);//paint background back buffer

	//Copy color_buffer to texture and render it
	SDL_UpdateTexture(texture, NULL, color_buffer, (int)(window_width * sizeof(uint32_t)));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	//Paint the color buffer here:

	//clear_color_buffer(0xFFFFFFFF);

	SDL_RenderPresent(renderer);//Presents to the screen

}



//SDL_PIXELFORMAT_ABGR8888 is a constant for the alpha blue green red and states that each value is 8 bits.

void clean_up_windowing_system()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

int main(void)
{
	is_running = initialize_windowing_system();
	set_up_memory_buffers();

	while (is_running) {
		//game loop
		/*The three main functions of the game loop is
		Process
		keyboard
		input
		*/
		process_keyboard_input();
		update_state();
		run_render_pipeline();
	}
	clean_up_windowing_system();
	return 0;
}
