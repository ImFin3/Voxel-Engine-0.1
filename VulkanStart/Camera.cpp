#include "Camera.h"

Camera::Camera()
{
	m_position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_forward = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	m_localFwd = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
}

Camera::Camera(const glm::vec3& a_Position, const glm::vec3& a_Target)
{
	m_position = glm::vec4(a_Position, 0.0f);

	glm::vec4 lFwd = glm::vec4(a_Target - a_Position, 0.0f);
	lFwd = glm::normalize(lFwd);

	m_localFwd = lFwd;
	m_forward = m_localFwd + m_position;
}

void Camera::Rotate(const UserInput& a_input, const float a_mouseSpeed)
{
	float zRot = a_input.xPosDelta * a_mouseSpeed * -1;
	float xyRot = a_input.yPosDelta * a_mouseSpeed;

	if (m_localFwd.z <= -0.999f && xyRot > 0.0f) {
		xyRot = 0.0f;
	}
	if (m_localFwd.z >= 0.999f && xyRot < 0.0f) {
		xyRot = 0.0f;
	}

	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), zRot, UP3);

	glm::vec3 uVec = GetUVec3();
	glm::mat4 rotationXY = glm::rotate(glm::mat4(1.0f), xyRot, uVec);

	m_localFwd = rotationZ * m_localFwd;
	m_localFwd = rotationXY * m_localFwd;

	m_forward = m_position + m_localFwd;
}

void Camera::Move(const UserInput& a_input, const float a_deltaTime, const float a_speed)
{
	glm::vec3 localDirection = { 0.0f, 0.0f, 0.0f };

	switch (a_input.lengthways)
	{
	case InputState::NEGATIVE:
		localDirection.x = -1.0f * a_deltaTime * a_speed;
		break;
	case InputState::NEUTRAL:
		break;
	case InputState::POSITIVE:
		localDirection.x = 1.0f * a_deltaTime * a_speed;
		break;
	default:
		break;
	}
	switch (a_input.sideways)
	{
	case InputState::NEGATIVE:
		localDirection.y = -1.0f * a_deltaTime * a_speed;
		break;
	case InputState::NEUTRAL:
		break;
	case InputState::POSITIVE:
		localDirection.y = 1.0f * a_deltaTime * a_speed;
		break;
	default:
		break;
	}
	switch (a_input.vertical) 
	{
	case InputState::NEGATIVE:
		localDirection.z = -1.0f * a_deltaTime * a_speed;
		break;
	case InputState::NEUTRAL:
		break;
	case InputState::POSITIVE:
		localDirection.z = 1.0f * a_deltaTime * a_speed;
		break;
	default:
		break;
	}

	localDirection *= 1;

	m_position += m_localFwd * localDirection.x + GetUVec4() * localDirection.y + UP4 * localDirection.z;
	m_forward += m_localFwd * localDirection.x + GetUVec4() * localDirection.y + UP4 * localDirection.z;
}

glm::vec3 Camera::GetRight3()
{
	return GetUVec3();
}

glm::vec4 Camera::GetRight4()
{
	return GetUVec4();
}

glm::vec3 Camera::GetUVec3()
{
	glm::mat4 rotationFwd = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 2, UP3);

	glm::vec4 rotatedFwd = rotationFwd * m_localFwd;
	rotatedFwd.z = 0.0f;
	rotatedFwd = glm::normalize(rotatedFwd);

	return glm::vec3(rotatedFwd.x, rotatedFwd.y, rotatedFwd.z);
}

glm::vec4 Camera::GetUVec4()
{
	glm::mat4 rotationFwd = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 2, UP3);

	glm::vec4 rotatedFwd = rotationFwd * m_localFwd;
	rotatedFwd.z = 0.0f;
	rotatedFwd = glm::normalize(rotatedFwd);

	return rotatedFwd;
}
