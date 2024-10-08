#include <cstdio>
#include <vector>

#include "SDL.h"
#undef main

#define GLEW_STATIC
#include "GL/glew.h"

#define HBMATH_IMPLEMENTATION
#include "hbmath.h"
using namespace hbmath;

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct Vertex
{
	Vec3 pos;
	Vec3 norm;
	Vec2 uv;
};

struct RenderObj
{
	GLuint vbo;
	GLuint vao;
	GLuint texture;
	bool textured;

	size_t vertex_count;
};

static_assert(sizeof(Vertex) == 8 * sizeof(float), "Vertex does not have expected memory layout");

GLuint LoadShader(const char* shader_src, GLenum shader_type)
{
	char info_log[256];

	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_src, nullptr);
	glCompileShader(shader);

	GLsizei info_log_length;
	glGetShaderInfoLog(shader, sizeof(info_log), &info_log_length, info_log);
	if (info_log_length >= sizeof(info_log))
	{
		info_log[sizeof(info_log) - 1] = 0;
	}
	else
	{
		info_log[info_log_length] = 0;
	}

	if (shader_type == GL_VERTEX_SHADER)
	{
		printf("Vertex shader info log: ");
	}
	else if (shader_type == GL_FRAGMENT_SHADER)
	{
		printf("Fragment shader info log: ");
	}
	printf("%s\n", info_log);

	return shader;
}

GLuint LinkProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	return shader_program;
}

Vertex* ReadObj(fastObjMesh *mesh, size_t group, size_t *vertex_count, char **texture_path)
{
	*vertex_count = 0;
	Vertex* vertices;

	size_t face_vertices = 3;

	vertices = new Vertex[face_vertices * mesh->face_count];

	if (mesh->group_count <= group)
	{
		printf("group does not exist\n");
		exit(1);
	}

	size_t material_index = mesh->face_materials[mesh->groups[group].face_offset];

	for (size_t i = 0; i < mesh->groups[group].face_count; ++i)
	{
		if (mesh->face_vertices[i + mesh->groups[group].face_offset] != face_vertices)
		{
			printf("only triangulated meshes are supported\n");
			assert(false);
		}

		if (mesh->face_materials[i + mesh->groups[group].face_offset] != material_index)
		{
			printf("only one material per group is supported\n");
			exit(1);
		}

		// Push each corner of face
		for (size_t v = 0; v < face_vertices; ++v)
		{
			size_t index_offset = mesh->groups[group].index_offset + i * face_vertices + v;

			size_t pos_index = mesh->indices[index_offset].p;
			size_t norm_index = mesh->indices[index_offset].n;
			size_t uv_index = mesh->indices[index_offset].t;
			memcpy(&vertices[*vertex_count].pos, mesh->positions + 3 * pos_index, sizeof(Vec3));
			memcpy(&vertices[*vertex_count].norm, mesh->normals + 3 * norm_index, sizeof(Vec3));
			memcpy(&vertices[*vertex_count].uv, mesh->texcoords + 2 * uv_index, sizeof(Vec2));
			*vertex_count += 1;
		}
	}

	size_t texture_index = mesh->materials[material_index].map_Kd;

	if (texture_index)
	{
        char* texture_path_src = mesh->textures[texture_index].path;
        *texture_path = new char[strlen(texture_path_src) + 1];
        memcpy(*texture_path, texture_path_src, strlen(texture_path_src) + 1);
	}
	else
	{
		*texture_path = nullptr;
	}

	assert(face_vertices * mesh->groups[group].face_count == *vertex_count);

	printf("Loaded mesh with %zu vertices\n", *vertex_count);

	return vertices;
}

RenderObj LoadRenderObj(fastObjMesh* mesh, size_t index)
{
	RenderObj obj;
	char *texture_path;
	Vertex* vertices = ReadObj(mesh, index, &obj.vertex_count, &texture_path);
	assert(vertices);

    glGenBuffers(1, &obj.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBufferData(GL_ARRAY_BUFFER, obj.vertex_count * sizeof(*vertices), vertices, GL_STATIC_DRAW);

	obj.textured = texture_path != nullptr;

    glGenVertexArrays(1, &obj.vao);
    glBindVertexArray(obj.vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, pos)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, norm)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, uv)));

	if (obj.textured)
	{
		printf("loading texture %s\n", texture_path);
        int w, h, n;
		uint8_t *texture_data = stbi_load(texture_path, &w, &h, &n, 3);
        n = 3;

        glGenTextures(1, &obj.texture);
        glBindTexture(GL_TEXTURE_2D, obj.texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(texture_data);
        delete[] texture_path;
	}
    delete[] vertices;
	return obj;
}

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"virtual texturing demo",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL);

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

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.5f, 0.6f, 0.7f, 1.0f);

	std::vector<RenderObj> render_objects;
	{
		fastObjMesh* mesh = fast_obj_read("assets/terrain.obj");
		printf("%u groups in mesh\n", mesh->group_count);

		render_objects.push_back(LoadRenderObj(mesh, 0));
		render_objects.push_back(LoadRenderObj(mesh, 1));

		fast_obj_destroy(mesh);
	}

	const char *vertex_shader_src =
		"#version 430\n"
		"in layout(location = 0) vec3 vpos;\n"
		"in layout(location = 1) vec3 vnorm;\n"
		"in layout(location = 2) vec2 uv;\n"
		"layout (location = 0) uniform vec3 position;\n"
		"layout (location = 1) uniform mat3 rotation;\n"
		"layout (location = 2) uniform mat4 perspective;\n"
		"layout (location = 3) uniform mat3 camera;\n"
		"out vec3 norm;\n"
		"out vec2 uv_out;\n"
		"void main() {\n"
		"    vec3 pos = rotation * vpos;\n"
		"    pos += position;\n"
		"    pos = camera * pos;\n"
		"    gl_Position = perspective * vec4(pos.x, pos.y, pos.z, 1.0f);\n"
		"    norm = vnorm;\n"
		"    uv_out = uv;\n"
		"}\n";

	const char* fragment_shader_src =
		"#version 430\n"
		"in vec3 norm;\n"
		"in vec2 uv_out;"
		"out vec4 color;\n"
		"uniform sampler2D color_texture;\n"
		"layout (location = 8) uniform float uv_scale_factor;\n"
		"void main() {\n"
		"    float brightness = 0.5f + 0.5f * clamp(dot(norm, vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f);\n"
		"    vec3 texture_color = texture(color_texture, uv_out * uv_scale_factor).rgb;\n"
		"    color = brightness * vec4(texture_color.r, texture_color.g, texture_color.b, 1.0f);\n"
		"}\n";

	GLuint vertex_shader = LoadShader(vertex_shader_src, GL_VERTEX_SHADER);
	GLuint fragment_shader = LoadShader(fragment_shader_src, GL_FRAGMENT_SHADER);
	GLuint shader_program = LinkProgram(vertex_shader, fragment_shader);

	glUseProgram(shader_program);

	Mat4 perspective_mat = Mat4::Perspective(0.1f, 100.0f, 0.5f * (float)M_PI, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	Vec3 position;

	uint64_t perf_freq = SDL_GetPerformanceFrequency();
	uint64_t perf_cnt = SDL_GetPerformanceCounter();

	float camera_pitch = 0.0f;
	float camera_yaw = 0.0f;

	Vec3 camera_pos(0.0f, 5.0f, 0.0f);

	bool w_down = false;;
	bool a_down = false;
	bool s_down = false;
	bool d_down = false;
	bool q_down = false;
	bool e_down = false;
	bool shift_down = false;

	bool running = true;
	while (running)
	{
		int32_t mouse_dx = 0;
		int32_t mouse_dy = 0;

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
				if (event.key.keysym.sym == SDLK_LSHIFT) shift_down = true;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_w) w_down = false;
				if (event.key.keysym.sym == SDLK_a) a_down = false;
				if (event.key.keysym.sym == SDLK_s) s_down = false;
				if (event.key.keysym.sym == SDLK_d) d_down = false;
				if (event.key.keysym.sym == SDLK_q) q_down = false;
				if (event.key.keysym.sym == SDLK_e) e_down = false;
				if (event.key.keysym.sym == SDLK_LSHIFT) shift_down = false;
				break;
			}
		}

		camera_yaw += (float)mouse_dx * 0.01f;
		camera_pitch += (float)mouse_dy * 0.01f;

		if (camera_yaw > M_PI) camera_yaw -= 2.0f * (float)M_PI;
		if (camera_yaw < -M_PI) camera_yaw += 2.0f * (float)M_PI;
		if (camera_pitch > 0.5f * M_PI) camera_pitch = 0.5f * (float)M_PI;
		if (camera_pitch < -0.5f * M_PI) camera_pitch = -0.5f * (float)M_PI;

		Quaternion camera_rotation = Quaternion::RotateX(camera_pitch) * Quaternion::RotateY(camera_yaw);

		Vec3 camera_delta;
		if (w_down) camera_delta.z -= 0.02f;
		if (s_down) camera_delta.z += 0.02f;
		if (a_down) camera_delta.x -= 0.02f;
		if (d_down) camera_delta.x += 0.02f;
		if (q_down) camera_delta.y -= 0.02f;
		if (e_down) camera_delta.y += 0.02f;

		if (shift_down) camera_delta *= 3;

		Mat3 camera_rotation_inverse_mat;
		camera_rotation.inverse().to_matrix(camera_rotation_inverse_mat.data);
		camera_delta = camera_rotation_inverse_mat * camera_delta;

		camera_pos += camera_delta;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(2, 1, GL_TRUE, perspective_mat.data);

		Mat3 camera_rotation_mat;
		camera_rotation.to_matrix(camera_rotation_mat.data);
		glUniformMatrix3fv(3, 1, GL_TRUE, camera_rotation_mat.data);

		for (const RenderObj &render_object : render_objects)
		{
			glBindVertexArray(render_object.vao);
			if (render_object.textured)
			{
				glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, render_object.texture);
			}

            glUniform3fv(0, 1, (position - camera_pos).array());

            Mat3 rotation_mat;
            glUniformMatrix3fv(1, 1, GL_TRUE, rotation_mat.data);

            glUniform1f(8, 1.0f);

            glDrawArrays(GL_TRIANGLES, 0, render_object.vertex_count);
		}

		SDL_GL_SwapWindow(window);

		uint64_t perf_cnt_now = SDL_GetPerformanceCounter();
		uint64_t perf_cnt_delta = perf_cnt_now - perf_cnt;
		double fps = (double)perf_freq / (double)perf_cnt_delta;
		perf_cnt = perf_cnt_now;
		printf("%.3f FPS      \r", fps);
	}

	return 0;
}
