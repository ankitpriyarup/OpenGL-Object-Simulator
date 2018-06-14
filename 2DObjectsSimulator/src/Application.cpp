#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"
#include "Object.h"
#include "CSVWriter.h"

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;


void showObjects(std::vector<Object> &airplanes, std::vector<Object> &helicopters, std::vector<Object> &cars, std::vector<Object> &boats,
	int index, int &quantity, float &velocityRange1, float &velocityRange2, float &accelerationRange1, float &accelerationRange2)
{
	if (index == 0)
		glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
	else if (index == 1)
		glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
	else if (index == 2)
		glClearColor(0.5f, 0.7f, 0.4f, 1.0f);
	else if (index == 3)
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);

	airplanes.clear();
	helicopters.clear();
	cars.clear();
	boats.clear();
	for (int i = 0; i < quantity; i++)
	{
		glm::vec3 position = glm::vec3(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 0);

		float minVelocity, maxVelocity;
		if (velocityRange1 > velocityRange2)
		{
			minVelocity = velocityRange2;
			maxVelocity = velocityRange1;
		}
		else
		{
			maxVelocity = velocityRange2;
			minVelocity = velocityRange1;
		}

		float minAcceleration, maxAcceleration;
		if (accelerationRange1 > accelerationRange2)
		{
			minAcceleration = accelerationRange2;
			maxAcceleration = accelerationRange1;
		}
		else
		{
			maxAcceleration = accelerationRange2;
			minAcceleration = accelerationRange1;
		}

		float velocityVal = (maxVelocity == minVelocity) ? maxVelocity : (rand() % (int)(maxVelocity - minVelocity)) + minVelocity;
		float accelerationVal = (maxAcceleration == minAcceleration) ? maxAcceleration : (rand() % (int)(maxAcceleration - minAcceleration)) + minAcceleration;
		glm::vec3 velocity = glm::vec3(velocityVal, 0, 0);

		if (index == 0)
			airplanes.emplace_back(position, velocity, glm::vec3(accelerationVal, -9.8f, 0), glm::vec3(0, 0, 0));
		else if (index == 1)
			helicopters.emplace_back(position, velocity, glm::vec3(accelerationVal, -9.8f, 0), glm::vec3(0, 0, 0));
		else if (index == 2)
			cars.emplace_back(position, velocity, glm::vec3(accelerationVal, 0, 0), glm::vec3(5, 0, 0));
		else if (index == 3)
			boats.emplace_back(position, velocity, glm::vec3(accelerationVal, 0, 0), glm::vec3(10, 0, 0));
	}
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scene", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] =
	{
		-96.0f, -41.0f, 0.0f, 0.0f,
		 96.0f, -41.0f, 1.0f, 0.0f,
		 96.0f,  41.0f, 1.0f, 1.0f,
		-96.0f,  41.0f, 0.0f, 1.0f
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND));

	VertexArray va;
	VertexBuffer vb(positions, 4 * 4 * sizeof(float));

	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 6);

	Shader shader("res/Diffuse.shader");
	shader.Bind();

	glm::mat4 proj = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 pro = proj * view;
	shader.SetUniformMat4f("u_MVP", proj * view);

	Renderer renderer;

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	Texture texture[]{ "res/plane.png", "res/helicopter.png", "res/car.png", "res/boat.png" };
	std::vector<Object> airplanes;
	std::vector<Object> helicopters;
	std::vector<Object> cars;
	std::vector<Object> boats;
	float velocityRange[] { 0.0f, 20.0f };
	float acceleartionRange[]{ 0.0f, 1.0f };
	int quantity = 5;

	CSVWriter writer("data.csv");

	std::vector<std::string> dataListHeader = { "Object", "positionX", "positionY", "velocityX", "velocityY" };
	writer.addDatainRow(dataListHeader.begin(), dataListHeader.end());

	while (!glfwWindowShouldClose(window))
	{
		renderer.Clear();
		ImGui_ImplGlfwGL3_NewFrame();

		texture[0].Bind(0);
		shader.SetUniform1i("u_Texture", 0);
		for (int i = 0; i < airplanes.size(); i++)
		{
			airplanes[i].Update();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), airplanes[i].position);
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", proj * view * model);
			renderer.Draw(va, ib, shader);

			std::vector<std::string> data = { "Airplane" + std::to_string(i),
				std::to_string(airplanes[i].position.x), std::to_string(airplanes[i].position.y),
				std::to_string(airplanes[i].velocity.x), std::to_string(airplanes[i].velocity.y) };
			writer.addDatainRow(data.begin(), data.end());
		}

		texture[1].Bind(0);
		shader.SetUniform1i("u_Texture", 0);
		for (int i = 0; i < helicopters.size(); i++)
		{
			helicopters[i].Update();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), helicopters[i].position);
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", proj * view * model);
			renderer.Draw(va, ib, shader);

			std::vector<std::string> data = { "Helicopter" + std::to_string(i),
				std::to_string(helicopters[i].position.x), std::to_string(helicopters[i].position.y),
				std::to_string(helicopters[i].velocity.x), std::to_string(helicopters[i].velocity.y) };
			writer.addDatainRow(data.begin(), data.end());
		}

		texture[2].Bind(0);
		shader.SetUniform1i("u_Texture", 0);
		for (int i = 0; i < cars.size(); i++)
		{
			cars[i].Update();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), cars[i].position);
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", proj * view * model);
			renderer.Draw(va, ib, shader);

			std::vector<std::string> data = { "Car" + std::to_string(i),
				std::to_string(cars[i].position.x), std::to_string(cars[i].position.y),
				std::to_string(cars[i].velocity.x), std::to_string(cars[i].velocity.y) };
			writer.addDatainRow(data.begin(), data.end());
		}

		texture[3].Bind(0);
		shader.SetUniform1i("u_Texture", 0);
		for (int i = 0; i < boats.size(); i++)
		{
			boats[i].Update();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), boats[i].position);
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", proj * view * model);
			renderer.Draw(va, ib, shader);

			std::vector<std::string> data = { "Boat" + std::to_string(i),
				std::to_string(boats[i].position.x), std::to_string(boats[i].position.y),
				std::to_string(boats[i].velocity.x), std::to_string(boats[i].velocity.y) };
			writer.addDatainRow(data.begin(), data.end());
		}

		{
			ImGui::SliderFloat2("Velocity Range", velocityRange, 0.0f, 1000);
			ImGui::SliderFloat2("Acceleration Range", acceleartionRange, 0.0f, 100);
			ImGui::SliderInt("Quantity", &quantity, 0, 5000);

			if (ImGui::Button("Airplane") && quantity > 0 && (velocityRange[0] > 0 || velocityRange[1] > 0))
				showObjects(airplanes, helicopters, cars, boats, 0, quantity, velocityRange[0], velocityRange[1], acceleartionRange[0], acceleartionRange[1]);

			if (ImGui::Button("Helicopter") && quantity > 0 && (velocityRange[0] > 0 || velocityRange[1] > 0))
				showObjects(airplanes, helicopters, cars, boats, 1, quantity, velocityRange[0], velocityRange[1], acceleartionRange[0], acceleartionRange[1]);

			if (ImGui::Button("Car") && quantity > 0 && (velocityRange[0] > 0 || velocityRange[1] > 0))
				showObjects(airplanes, helicopters, cars, boats, 2, quantity, velocityRange[0], velocityRange[1], acceleartionRange[0], acceleartionRange[1]);

			if (ImGui::Button("Boat") && quantity > 0 && (velocityRange[0] > 0 || velocityRange[1] > 0))
				showObjects(airplanes, helicopters, cars, boats, 3, quantity, velocityRange[0], velocityRange[1], acceleartionRange[0], acceleartionRange[1]);

		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}
