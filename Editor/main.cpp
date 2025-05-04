#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

int main() {
	sf::RenderWindow window;
	window.create(sf::VideoMode({ 800, 600 }), "Luden Editor");

	if (!ImGui::SFML::Init(window)) {
		std::cerr << "Failed to initialize ImGui-SFML!" << std::endl;
		return 1;
	}

	sf::Clock deltaClock;

	while (window.isOpen()) {
		while (auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			ImGui::SFML::ProcessEvent(window, *event);
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Hello, Luden!");
		ImGui::Text("Welcome to the Editor");
		ImGui::End();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}
