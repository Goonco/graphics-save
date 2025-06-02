#define _CRT_SECURE_NO_WARNINGS
#include "Scene_Definitions.h"

void Spider_D::define_object() {
#define N_SPIDER_FRAMES 16
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_SPIDER_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(70.0f, -70.0f, 0.0f));
		*cur_MM = glm::scale(*cur_MM, glm::vec3(3.0f, -3.0f, 3.0f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Wolf_D::define_object() {
#define N_WOLF_FRAMES 16
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_WOLF_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/dynamic_objects/wolf/wolf_%d%d_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 12.0f, -25.0f));
		*cur_MM = glm::scale(*cur_MM, glm::vec3(20.0f));
		*cur_MM = glm::rotate(*cur_MM, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN,glm::vec3(1.0f, 0.0f, 0.0f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Dynamic_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp) {
	int cur_object_index = time_stamp % object_frames.size();
	Static_Object& cur_object = object_frames[cur_object_index];
	glFrontFace(cur_object.front_face_mode);

	float rotation_angle = 0.0f;
	glm::mat4 ModelMatrix = glm::mat4(1.0f);

	if (object_id == DYNAMIC_OBJECT_SPIDER) {
		Spider_D* spider = static_cast<Spider_D*>(this);
		cur_object_index = (cur_object_index * 3) % object_frames.size();

		if (spider->flag_up) {
			spider->pos += glm::vec3(0.0f, 0.0f, 0.1f);
			if (spider->pos[2] > 17.0f) spider->flag_up = false;
		}
		else {
			spider->pos -= glm::vec3(0.0f, 0.0f, 0.1f);
			if (spider->pos[2] < -17.0f) spider->flag_up = true;
		}

		ModelMatrix = glm::translate(ModelMatrix, spider->pos);
	}

	for (int i = 0; i < cur_object.instances.size(); i++) {
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix * cur_object.instances[i].ModelMatrix;
		switch (shader_kind) {
		case SHADER_SIMPLE:
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, cur_object.instances[i].material.diffuse.r,
				cur_object.instances[i].material.diffuse.g, cur_object.instances[i].material.diffuse.b);
			break;
		}
		glBindVertexArray(cur_object.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}