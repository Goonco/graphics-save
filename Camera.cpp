#define _CRT_SECURE_NO_WARNINGS

#include "Camera.h"
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

float MOVE_FB_SPEED = 10;
float MOVE_LRUD_SPEED = 5;

float ROTATE_SPEED = 1.0f;

void Camera::look_at() {
	ViewMatrix = glm::lookAt(cam_view.pos, cam_view.pos - cam_view.naxis,
		cam_view.vaxis); // initial pose for main camera
}

void Camera::move_camera(Camera_Move move) {
	switch (move) {
	case CAMERA_FRONT:
		cam_view.pos += (-MOVE_FB_SPEED * cam_view.naxis);
		break;
	case CAMERA_BACK:
		cam_view.pos += (MOVE_FB_SPEED * cam_view.naxis);
		break;
	case CAMERA_RIGHT:
		cam_view.pos += (MOVE_LRUD_SPEED * cam_view.uaxis);
		break;
	case CAMERA_LEFT:
		cam_view.pos += (-MOVE_LRUD_SPEED * cam_view.uaxis);
		break;
	case CAMERA_UP:
		cam_view.pos += (MOVE_LRUD_SPEED * cam_view.vaxis);
		break;
	case CAMERA_DOWN:
		cam_view.pos += (-MOVE_LRUD_SPEED * cam_view.vaxis);
		break;
	};

	look_at();
}

void Camera::tilt_camera(Camera_Tilt tilt) {
	switch (tilt) {
	case U_C:
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(ROTATE_SPEED), cam_view.uaxis);
		break;
	case U_RC:
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(-ROTATE_SPEED), cam_view.uaxis);
		break;
	case V_C:
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(ROTATE_SPEED), cam_view.vaxis);
		break;
	case V_RC:
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(-ROTATE_SPEED), cam_view.vaxis);
		break;
	case N_C:
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(ROTATE_SPEED), cam_view.naxis);
		break;
	case N_RC:
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(-ROTATE_SPEED), cam_view.naxis);
		break;
	};
	cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
	cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
	cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
	look_at();
}

void Perspective_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
	float left_padding = win_width / 4;

	switch (camera_id) {
	case CAMERA_MAIN:
		ViewMatrix = glm::lookAt(glm::vec3(0.0f, -1.0f, 800.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
	
		define_axis_and_pos();

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;  
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		
		view_port.x = left_padding; view_port.y = 0; view_port.w = win_width - left_padding; view_port.h = win_height;
		break;
	case CAMERA_CC_0:
		break;
	}
}


void Camera::define_axis_and_pos() {
	glm::mat3 R33_t;
	glm::mat4 T;

	cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
	cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
	cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
	R33_t = glm::transpose(glm::mat3(ViewMatrix));
	T = glm::mat4(R33_t) * ViewMatrix;
	cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);
}

void Orthographic_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
	float left_padding = win_width / 4;
	float squash = win_height / 3;

	switch (camera_id) {
	case CAMERA_FRONT_SIDE:
		ViewMatrix = glm::lookAt(glm::vec3(600.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera 
		
		define_axis_and_pos();

		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
		cam_proj.params.ortho.left = -75.0f;
		cam_proj.params.ortho.bottom = -25.0f;
		cam_proj.params.ortho.right = -cam_proj.params.ortho.left;
		cam_proj.params.ortho.top = -cam_proj.params.ortho.bottom;
		cam_proj.params.ortho.n = 1.0f;
		cam_proj.params.ortho.f = 1000.0f;

		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right, cam_proj.params.ortho.bottom, cam_proj.params.ortho.top, cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		view_port.x = 0; view_port.y = squash * 2; view_port.w = left_padding; view_port.h = squash;
		break;
	case CAMERA_SIDE:
		ViewMatrix = glm::lookAt(glm::vec3(0.0f, 600.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera 
		
		define_axis_and_pos();

		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
		cam_proj.params.ortho.left = -110.0f;
		cam_proj.params.ortho.bottom = -25.0f;
		cam_proj.params.ortho.right = -cam_proj.params.ortho.left;
		cam_proj.params.ortho.top = -cam_proj.params.ortho.bottom;
		cam_proj.params.ortho.n = 1.0f;
		cam_proj.params.ortho.f = 1000.0f;

		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right, cam_proj.params.ortho.bottom, cam_proj.params.ortho.top, cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		view_port.x = 0; view_port.y = squash * 1; view_port.w = left_padding; view_port.h = squash;
		break;
	case CAMERA_TOP:
		ViewMatrix = glm::lookAt(glm::vec3(0.0f, -1.0f, 800.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		
		define_axis_and_pos();

		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
		cam_proj.params.ortho.left = -110.0f;
		cam_proj.params.ortho.bottom = -75.0f;
		cam_proj.params.ortho.right = -cam_proj.params.ortho.left;
		cam_proj.params.ortho.top = -cam_proj.params.ortho.bottom;
		cam_proj.params.ortho.n = 1.0f;
		cam_proj.params.ortho.f = 1000.0f;

		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right, cam_proj.params.ortho.bottom, cam_proj.params.ortho.top, cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		view_port.x = 0; view_port.y = 0; view_port.w = left_padding; view_port.h = squash;
		break;
	}
}



