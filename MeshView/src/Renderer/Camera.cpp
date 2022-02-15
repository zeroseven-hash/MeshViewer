#include "pch.h"
#include "Camera.h"

#include <CoreModel/Input.h>
#include<CoreModel/Macro.h>
#include<CoreModel/Log.h>



namespace MV {

	Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
		m_Projection(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}

	void Camera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void Camera::UpdateView()
	{
		//glm::quat m_quaternion = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));  Lock the camera's rotation
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> Camera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float Camera::RotationSpeed() const
	{
		return 0.8f;
	}

	float Camera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		if (distance < 1.0f)
		{
			speed = distance;
		}
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void Camera::OnUpdate(TimeStep ts)
	{
	
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		if (Input::IsMouseButtonPressed(IGAME_MOUSE_BUTTON_MIDDLE))
			MousePan(delta);
		else if (Input::IsMouseButtonPressed(IGAME_MOUSE_BUTTON_LEFT))
			MouseRotate(delta);
		else if (Input::IsMouseButtonPressed(IGAME_MOUSE_BUTTON_RIGHT))
			MouseZoom(delta.y);
		

		UpdateView();
	}

	void Camera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&Camera::OnMouseScroll,this,std::placeholders::_1));
	}

	bool Camera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetOffsetY() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void Camera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void Camera::MouseRotate(const glm::vec2& delta)
	{
		//四元数计算公式：
		//x = RotationAxis.x * sin(RotationAngle / 2)
		//y = RotationAxis.y * sin(RotationAngle / 2)
		//z = RotationAxis.z * sin(RotationAngle / 2)
		//w = cos(RotationAngle / 2)
		
		glm::vec3 upDirection = GetUpDirection();
		glm::vec3 rightDirection = GetRightDirection();
		float rotationAngle = glm::radians(( - delta.x * 60.0f)/2);
		float x = upDirection.x * sin(rotationAngle);
		float y = upDirection.y * sin(rotationAngle);
		float z = upDirection.z * sin(rotationAngle);
		float w = cos(rotationAngle);
		
	
		
		rotationAngle = glm::radians(( - delta.y * 60.0f)/2);
		float x1 = rightDirection.x * sin(rotationAngle);
		float y1 = rightDirection.y * sin(rotationAngle);
		float z1 = rightDirection.z * sin(rotationAngle);
		float w1 = cos(rotationAngle);
		
		//quad must be normalized
		m_quaternion = glm::normalize(glm::quat(w1, x1, y1, z1)* glm::quat(w, x, y, z) * m_quaternion);
		
		
	}

	void Camera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 0.1f)
		{
			m_Distance = 0.1f;
			m_FocalPoint += GetForwardDirection()*0.1f;
			
		}
	}

	glm::vec3 Camera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::GetForwardDirection() const
	{
		//将朝前方向沿四元数旋转。
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 Camera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat Camera::GetOrientation() const
	{
		//quat返回一个四元数，此处输入欧拉角返回去四元数表示。表示一个旋转。
	
		return m_quaternion;
	}

}