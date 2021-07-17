#pragma once

#include "Renderer.h"

#include <iostream>

namespace Broccoli {

	Renderer::Renderer()
	{
		std::cout << "Renderer created!\n";

		// Initialise the shader library
		shaderLibrary = new ShaderLibrary();

		shaderLibrary->loadShader("Broccoli/resources/shaders/geometry.vert");
	}

	Renderer::~Renderer()
	{
		delete shaderLibrary;
	}

}