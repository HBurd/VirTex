#include <cstdio>

#include "SDL.h"
#undef main

#define GLEW_STATIC
#include "GL/glew.h"

#define HBMATH_IMPLEMENTATION
#include "hbmath.h"
using namespace hbmath;

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct Vertex
{
	Vec3 pos;
	Vec3 norm;
};

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("virtual texturing demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		printf("glewInit() failed\n");
		exit(1);
	}

	printf("Using OpenGL version %s\n", glGetString(GL_VERSION));

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	GLuint vbo;
	GLuint vao;
	size_t vertex_count = 0;
	Vertex* vertices;
	{
		fastObjMesh* mesh = fast_obj_read("assets/terrain.obj");


		size_t face_vertices = 3;

		vertices = new Vertex[face_vertices * mesh->face_count];

		if (mesh->group_count == 0)
		{
			printf("mesh has no groups\n");
			exit(1);
		}

		size_t group = 0;

		if (mesh->index_count != face_vertices * mesh->groups[0].face_count)
		{
			printf("only triangulated meshes are supported\n");
			exit(1);
		}

		for (size_t i = 0; i < mesh->groups[group].face_count; ++i)
		{
			if (mesh->face_vertices[i + mesh->groups[group].face_offset] != face_vertices)
			{
				printf("only triangulated meshes are supported\n");
				exit(1);
			}

			// Push each corner of face
			for (size_t v = 0; v < face_vertices; ++v)
			{
				size_t index_offset = mesh->groups[group].index_offset + i * face_vertices + v;

				size_t pos_index = mesh->indices[index_offset].p;
				size_t norm_index = mesh->indices[index_offset].n;
				memcpy(&vertices[vertex_count].pos, mesh->positions + 3 * pos_index, sizeof(Vec3));
				memcpy(&vertices[vertex_count].norm, mesh->normals + 3 * norm_index, sizeof(Vec3));
				vertex_count += 1;
			}
		}

		assert(face_vertices * mesh->face_count == vertex_count);

		fast_obj_destroy(mesh);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(*vertices), vertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));
	}

	printf("Loaded mesh with %zu vertices\n", vertex_count);

	const char *vertex_shader_src =
		"#version 430\n"
		"in layout(location = 0) vec3 vpos;\n"
		"in layout(location = 1) vec3 vnorm;\n"
		"layout (location = 0) uniform vec3 position;\n"
		"layout (location = 1) uniform mat3 rotation;\n"
		"layout (location = 2) uniform mat4 perspective;\n"
		"layout (location = 3) uniform mat3 camera;\n"
		"out vec3 norm;\n"
		"void main() {\n"
		"    vec3 pos = rotation * vpos;"
		"    pos += position;\n"
		"    pos = camera * pos;\n"
		"    gl_Position = perspective * vec4(pos.x, pos.y, pos.z, 1.0f);\n"
		"    norm = vnorm;\n"
		"}\n";

	const char* fragment_shader_src =
		"#version 430\n"
		"in vec3 norm;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"    float brightness = dot(norm, vec3(0.0f, 1.0f, 0.0f));\n"
		"    color = brightness * vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
		"}\n";

	char info_log[64];

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_src, nullptr);
	glCompileShader(vertex_shader);

	GLsizei info_log_length;
	glGetShaderInfoLog(vertex_shader, sizeof(info_log), &info_log_length, info_log);
	if (info_log_length >= sizeof(info_log))
	{
		info_log[sizeof(info_log) - 1] = 0;
	}
	else
	{
		info_log[info_log_length] = 0;
	}
	printf("%s\n", info_log);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_src, nullptr);
	glCompileShader(fragment_shader);

	glGetShaderInfoLog(fragment_shader, sizeof(info_log), &info_log_length, info_log);
	if (info_log_length >= sizeof(info_log))
	{
		info_log[sizeof(info_log) - 1] = 0;
	}
	else
	{
		info_log[info_log_length] = 0;
	}
	printf("%s\n", info_log);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glUseProgram(shader_program);


	glClearColor(0.5f, 0.6f, 0.7f, 1.0f);

	Mat4 perspective_mat = Mat4::Perspective(0.1f, 100.0f, 0.5f * (float)M_PI, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	Vec3 position;

	uint64_t perf_freq = SDL_GetPerformanceFrequency();
	uint64_t perf_cnt = SDL_GetPerformanceCounter();

	float camera_pitch = 0.0f;
	float camera_yaw = 0.0f;

	Vec3 camera_pos(0.0f, 0.4f, 1.0f);

	bool w_down = false;;
	bool a_down = false;
	bool s_down = false;
	bool d_down = false;
	bool q_down = false;
	bool e_down = false;

	bool running = true;
	while (running)
	{
		int32_t mouse_dx = 0.0f;
		int32_t mouse_dy = 0.0f;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEMOTION:
				mouse_dx = event.motion.xrel;
				mouse_dy = event.motion.yrel;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_w) w_down = true;
				if (event.key.keysym.sym == SDLK_a) a_down = true;
				if (event.key.keysym.sym == SDLK_s) s_down = true;
				if (event.key.keysym.sym == SDLK_d) d_down = true;
				if (event.key.keysym.sym == SDLK_q) q_down = true;
				if (event.key.keysym.sym == SDLK_e) e_down = true;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_w) w_down = false;
				if (event.key.keysym.sym == SDLK_a) a_down = false;
				if (event.key.keysym.sym == SDLK_s) s_down = false;
				if (event.key.keysym.sym == SDLK_d) d_down = false;
				if (event.key.keysym.sym == SDLK_q) q_down = false;
				if (event.key.keysym.sym == SDLK_e) e_down = false;
				break;
			}
		}

		camera_yaw += (float)mouse_dx * 0.01f;
		camera_pitch += (float)mouse_dy * 0.01f;

		if (camera_yaw > M_PI) camera_yaw -= 2.0f * M_PI;
		if (camera_yaw < -M_PI) camera_yaw += 2.0f * M_PI;
		if (camera_pitch > 0.5f * M_PI) camera_pitch = 0.5f * M_PI;
		if (camera_pitch < -0.5f * M_PI) camera_pitch = -0.5f * M_PI;

		Quaternion camera_rotation = Quaternion::RotateX(camera_pitch) * Quaternion::RotateY(camera_yaw);

		Vec3 camera_delta;
		if (w_down) camera_delta.z -= 0.02f;
		if (s_down) camera_delta.z += 0.02f;
		if (a_down) camera_delta.x -= 0.02f;
		if (d_down) camera_delta.x += 0.02f;
		if (q_down) camera_delta.y -= 0.02f;
		if (e_down) camera_delta.y += 0.02f;

		Mat3 camera_rotation_inverse_mat;
		camera_rotation.inverse().to_matrix(camera_rotation_inverse_mat.data);
		camera_delta = camera_rotation_inverse_mat * camera_delta;

		camera_pos += camera_delta;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform3fv(0, 1, (position - camera_pos).array());

		Mat3 rotation_mat;
		glUniformMatrix3fv(1, 1, GL_TRUE, rotation_mat.data);

		glUniformMatrix4fv(2, 1, GL_TRUE, perspective_mat.data);

		Mat3 camera_rotation_mat;
		camera_rotation.to_matrix(camera_rotation_mat.data);
		glUniformMatrix3fv(3, 1, GL_TRUE, camera_rotation_mat.data);

		glDrawArrays(GL_TRIANGLES, 0, vertex_count);
		SDL_GL_SwapWindow(window);

		uint64_t perf_cnt_now = SDL_GetPerformanceCounter();
		uint64_t perf_cnt_delta = perf_cnt_now - perf_cnt;
		double fps = (double)perf_freq / (double)perf_cnt_delta;
		perf_cnt = perf_cnt_now;
		printf("%.3f FPS\r", fps);
	}

	return 0;
}
