#include "application.hpp"
#include "mth/linalg.hpp"
#include "modelloader.hpp"
#include "image.hpp"
#include "vpdloader.hpp"
#include <iostream>

namespace democollection
{
	void Application::Update()
	{
		const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

		vk::SceneBufferVs& sceneBufferVs = *m_sceneBufferVs->Data<vk::SceneBufferVs>();
		sceneBufferVs.cameraMatrix = mth::Transpose(m_camera.CameraMatrix());

		vk::SceneBufferFs& sceneBufferFs = *m_sceneBufferFs->Data<vk::SceneBufferFs>();
		sceneBufferFs.lightColor = mth::float4(1.0f);
		sceneBufferFs.lightPosition = mth::float4(m_camera.position(0), m_camera.position(1), m_camera.position(2), 1.0f);

		if (m_model)
			m_model->Update();

		//std::cout << 1.0f / std::chrono::duration<float>(now - m_prevFrameTime).count() << std::endl;
		m_prevFrameTime = now;
	}

	void Application::Render()
	{
		if (m_graphics->BeginRender())
		{
			if (m_model)
				m_model->Render();
			m_graphics->EndRender();
		}
	}

	void Application::Resize(int width, int height)
	{
		m_camera.UpdateScreenResolution(width, height);
		m_graphics->RequestResize();
	}

	void Application::MouseMove(double x, double y)
	{
		m_camController.MouseMove(static_cast<float>(x), static_cast<float>(y));
	}

	void Application::MouseButton(int button, int action, int modifier)
	{
		m_camController.MouseButton(button, action);
	}

	void Application::Scroll(double x, double y)
	{
		m_camController.Scroll(static_cast<float>(y));
	}

	Application::Application()
		: m_window{}
		, m_camController(m_camera)
	{}

	Application::~Application()
	{
		m_model.reset();
		m_sceneBufferVs.reset();
		m_sceneBufferFs.reset();
		m_graphics.reset();

		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Application::Init(int argc, char* argv[])
	{
		const char title[] = "demo-collection";
		const int width = 1280;
		const int height = 800;
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, static_cast<void*>(this));
		glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)->void {
			reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->Resize(width, height);
		});
		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y)->void{
			reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->MouseMove(x, y);
		});
		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int modifier)->void{
			reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->MouseButton(button, action, modifier);
		});
		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y)->void{
			reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->Scroll(x, y);
		});

		m_graphics = std::make_unique<vk::Graphics>(title, m_window);
		m_sceneBufferVs = std::make_unique<vk::UniformBuffer>(*m_graphics, sizeof(vk::SceneBufferVs));
		m_sceneBufferFs = std::make_unique<vk::UniformBuffer>(*m_graphics, sizeof(vk::SceneBufferFs));

		if (argc > 1)
		{
			ModelLoader ml;
			if (ml.LoadPmx(argv[1]))
				m_model = std::make_unique<vk::Model>(*m_graphics, *m_sceneBufferVs, *m_sceneBufferFs, ml);
		}

		if (argc > 2 && m_model)
		{
			VpdLoader pose(argv[2]);
			m_model->ClearBoneTransforms();
			for (const VpdLoader::Bone& b : pose.Bones())
				m_model->SetBoneTransform(b.boneName, b.translation, b.rotation);
			m_model->UpdateInheritTransforms();
		}

		m_camera.UpdateScreenResolution(width, height);
		m_camController.SetCenter(mth::float3(0.0f, -10.0f, 0.0f));
		m_camController.SetDistance(27.0f);

		m_prevFrameTime = m_startTime = std::chrono::steady_clock::now();
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_window))
		{
			glfwPollEvents();
			Update();
			Render();
		}
		m_graphics->Flush();
	}
}
