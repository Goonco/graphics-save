#pragma once

#include <glm/gtc/matrix_transform.hpp>

enum Camera_Projection_TYPE {
	CAMERA_PROJECTION_PERSPECTIVE = 0, CAMERA_PROJECTION_ORTHOGRAPHIC
};

enum Camera_ID {
	CAMERA_MAIN = 0, CAMERA_FRONT_SIDE, CAMERA_TOP, CAMERA_SIDE,
	CAMERA_CC_1, CAMERA_CC_2, CAMERA_CC_3
};

enum Camera_Move {
	CAMERA_LEFT = 0, CAMERA_RIGHT, CAMERA_FRONT, CAMERA_BACK, CAMERA_UP, CAMERA_DOWN
};

enum Camera_Tilt {
	U_C = 0, U_RC, N_C, N_RC, V_C, V_RC
};

enum Camera_Zoom {
	ZOOM_IN = 0, ZOOM_OUT
};

struct Camera_View {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
};
 
struct Camera_Projection {
	Camera_Projection_TYPE projection_type;
	union {
		struct {
			float fovy, aspect, n, f;
		} pers;
		struct {
			float left, right, bottom, top, n, f;
		} ortho;
	} params;
};

struct View_Port {
	float x, y, w, h;
};

struct Camera {
	Camera_ID camera_id;
	Camera_View cam_view;
	glm::mat4 ViewMatrix;
	Camera_Projection cam_proj;
	glm::mat4 ProjectionMatrix;
	View_Port view_port;

	Camera(Camera_ID _camera_id) : camera_id(_camera_id) {}

	void define_axis_and_pos();
	void look_at();
	void move_camera(Camera_Move move);
	void tilt_camera(Camera_Tilt tilt);
};

struct Perspective_Camera : public Camera {
	Perspective_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
	void zoom_camera(Camera_Zoom cam_zoom);
};

struct Orthographic_Camera : public Camera {
	Orthographic_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
};

struct Camera_Data {
	Perspective_Camera cam_main { CAMERA_MAIN };
	Perspective_Camera cam_cc_1{ CAMERA_CC_1 };
	Perspective_Camera cam_cc_2{ CAMERA_CC_2 };
	Perspective_Camera cam_cc_3{ CAMERA_CC_3 };

	Orthographic_Camera cam_front_side{ CAMERA_FRONT_SIDE };
	Orthographic_Camera cam_side{ CAMERA_SIDE };
	Orthographic_Camera cam_top{ CAMERA_TOP };
};