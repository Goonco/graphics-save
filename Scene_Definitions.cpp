#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
unsigned int camera_ID_mapper[N_MAX_CAMERAS];
unsigned int shader_ID_mapper[N_MAX_SHADERS];

void Axis_Object::define_axis() {
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Axis_Object::draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix) {
#define WC_AXIS_LENGTH		10.0f
	glm::mat4 ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(shader_simple->h_ShaderProgram);
	glUniformMatrix4fv(shader_simple->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(VAO);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Scene::clock(int clock_id) { // currently one clock
	time_stamp = ++time_stamp % UINT_MAX;
}

void Scene::build_static_world() {
	static_geometry_data.building.define_object();
	static_objects.push_back(static_geometry_data.building);

	static_geometry_data.dragon.define_object();
	static_objects.push_back(static_geometry_data.dragon);

	static_geometry_data.light.define_object();
	static_objects.push_back(static_geometry_data.light);

	static_geometry_data.table.define_object();
	static_objects.push_back(static_geometry_data.table);

	static_geometry_data.ironman.define_object();
	static_objects.push_back(static_geometry_data.ironman);

	static_geometry_data.bike.define_object();
	static_objects.push_back(static_geometry_data.bike);

	static_geometry_data.tableAndTea.define_object();
	static_objects.push_back(static_geometry_data.tableAndTea);
}

void Scene::build_dynamic_world() {
	dynamic_geometry_data.spider_d.define_object();
	dynamic_objects.push_back(dynamic_geometry_data.spider_d);

	dynamic_geometry_data.wolf_d.define_object();
	dynamic_objects.push_back(dynamic_geometry_data.wolf_d);
}

void Scene::create_camera_list(int win_width, int win_height, float win_aspect_ratio) {
	camera_list.clear();
	// main camera

	camera_data.cam_main.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_MAIN] = camera_list.size();
	camera_list.push_back(camera_data.cam_main);

	// fixed cameras
	camera_data.cam_cc_1.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_1] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc_1);

	camera_data.cam_cc_2.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_2] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc_2);

	camera_data.cam_cc_3.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_3] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc_3);
	

	// ortho cameras
	camera_data.cam_front_side.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_FRONT_SIDE] = camera_list.size();
	camera_list.push_back(camera_data.cam_front_side);

	camera_data.cam_side.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_SIDE] = camera_list.size();
	camera_list.push_back(camera_data.cam_side);

	camera_data.cam_top.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_TOP] = camera_list.size();
	camera_list.push_back(camera_data.cam_top);
}

void Scene::build_shader_list() {
	shader_data.shader_simple.prepare_shader();
	shader_ID_mapper[SHADER_SIMPLE] = shader_list.size();
	shader_list.push_back(shader_data.shader_simple);
}

void Scene::initialize() {
	axis_object.define_axis();
	build_static_world();
	build_dynamic_world();
	create_camera_list(window.width, window.height, window.aspect_ratio);
	build_shader_list();
}

void Scene::draw_static_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto static_object = static_objects.begin(); static_object != static_objects.end(); static_object++) {
		if (static_object->get().flag_valid == false) continue;
		static_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list);
	}
}

void Scene::draw_dynamic_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto dynamic_object = dynamic_objects.begin(); dynamic_object != dynamic_objects.end(); dynamic_object++) {
		if (dynamic_object->get().flag_valid == false) continue;
		dynamic_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list, time_stamp);
	}
}

void Scene::draw_axis() {
	axis_object.draw_axis(static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
		ViewMatrix, ProjectionMatrix);
}

void Scene::draw_world() {
	draw_axis();
	draw_static_world();
	draw_dynamic_world();
}