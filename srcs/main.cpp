/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cledant <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/02 12:14:31 by cledant           #+#    #+#             */
/*   Updated: 2017/09/07 14:31:47 by cledant          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Glfw_manager.hpp"
#include "oGL_module.hpp"
#include "World.hpp"

int		main(int argc, char **argv)
{
	Glfw_manager	manager;
	oGL_module		oGL;
	World			*world = nullptr;

	static_cast<void>(argc);
	static_cast<void>(argv);
	try
	{
		Glfw_manager::run_manager();
		manager.create_resizable_window("Particle System", 4, 1, 800, 600);
		manager.init_input_callback();
		oGL_module::oGL_enable_depth();
		oGL.add_shader("simple_box", "./shaders/simple_box/simple_box.vs",
			"./shaders/simple_box/simple_box.fs");
		world = new World(manager.getInput(), manager.getWindow(),
				glm::vec3(0.0f, 3.0f, 0.0f));
		world->add_Simple_box(&(oGL.getShader("simple_box")),
				glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 100.0f, 100.0f));
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		delete world;
		return (0);
	}
	while (Glfw_manager::getActiveWindowNumber())
	{
		if (manager.getWindow().win != nullptr)
		{
			manager.update_events();
			oGL_module::oGL_clear_buffer();
			world->update(Glfw_manager::getTime(), manager.getMouseMode());
			world->render();
			manager.update_title_fps();
			manager.swap_buffers();
			if (manager.should_window_be_closed() == true)
				manager.destroy_window();
		}
	}
	delete world;
	oGL.delete_all_shaders();
	Glfw_manager::close_manager();
	return (0);
}
