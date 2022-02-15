
#pragma once

#include<CoreModel/TimeStep.h>
#include<Event/MouseEvent.h>
#include <glm/glm.hpp>


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include<glm/gtc/matrix_transform.hpp>

namespace MV {


	
	class Camera
	{
		friend class MeshLayer;
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; UpdateView();}

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }
		inline void SetFocalPoint(const glm::vec3& focalpoint) { m_FocalPoint = focalpoint, UpdateView(); }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjection()const { return m_Projection; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		//float GetPitch() const { return m_Pitch; }
		//float GetYaw() const { return m_Yaw; }
		float GetFov()const { return m_FOV; }

		
	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 60.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_Projection;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		

		glm::quat m_quaternion = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;

	
	};

}