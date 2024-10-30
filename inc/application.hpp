#pragma once

#include "vk/model.hpp"
#include "camera.hpp"
#include "orbitcontroller.hpp"
#include <chrono>

namespace democollection
{
	class Application
	{
		GLFWwindow* m_window;
		std::unique_ptr<vk::Graphics> m_graphics;
		std::unique_ptr<vk::UniformBuffer> m_sceneBufferVs;
		std::unique_ptr<vk::UniformBuffer> m_sceneBufferFs;
		std::unique_ptr<vk::Model> m_model;
		Camera m_camera;
		OrbitController m_camController;
		std::chrono::steady_clock::time_point m_startTime;
		std::chrono::steady_clock::time_point m_prevFrameTime;

	private:
		void Update();
		void Render();

		void Resize(int width, int height);
		void MouseMove(double x, double y);
		void MouseButton(int button, int action, int modifier);
		void Scroll(double x, double y);

	public:
		Application();
		~Application();

		void Init(int argc, char* argv[]);
		void Run();
	};
}
