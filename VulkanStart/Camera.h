#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "UserInput.h"

const glm::vec3 UP3 = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec4 UP4 = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

class Camera {
private:
	glm::vec4 m_position;
	glm::vec4 m_forward;
	glm::vec4 m_localFwd;

public:
	Camera();
	Camera(const glm::vec3& a_Position, const glm::vec3& a_Target);

	void Rotate(const UserInput& a_input, const float a_mouseSpeed);

	void Move(const UserInput& a_input, const float a_deltaTime, const float a_speed); 

	glm::vec3 GetPosition3() {return glm::vec3(m_position.x, m_position.y, m_position.z);}
	glm::vec4 GetPosition4() {return m_position;}

	glm::vec3 GetForward3() {return glm::vec3(m_forward.x, m_forward.y, m_forward.z);}
	glm::vec4 GetForward4() {return m_forward;}

	glm::vec3 GetUp3() { return glm::vec3(m_position.x, m_position.y, m_position.z) + UP3;}
	glm::vec4 GetUp4() { return m_position + UP4; }

	glm::vec3 GetRight3();
	glm::vec4 GetRight4();

private:

	glm::vec3 GetUVec3();
	glm::vec4 GetUVec4();
};


#endif // !CAMERA_H

