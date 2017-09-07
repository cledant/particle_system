/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Glfw_manager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cledant <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/03 11:30:26 by cledant           #+#    #+#             */
/*   Updated: 2017/09/07 14:31:27 by cledant          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Glfw_manager.hpp"

Glfw_manager::Glfw_manager(void) : _input(), _window(), _mouse_exclusive(true),
	_delta_time(0.0f), _last_time(0.0f)
{
}

Glfw_manager::~Glfw_manager(void)
{
}

Glfw_manager::Glfw_manager(Glfw_manager const &src) : _input(), _window(),
	_mouse_exclusive(true), _delta_time(0.0f), _last_time(0.0f)
{
	static_cast<void>(src);
}

Glfw_manager		&Glfw_manager::operator=(Glfw_manager const &rhs)
{
	static_cast<void>(rhs);
	return (*this);
}

void				Glfw_manager::run_manager(void)
{
	auto	error_callback = [](int error, char const *what)
	{
		std::cout << "GLFW error code : " << error << std::endl;
		std::cout << what << std::endl;
	};

	glfwSetErrorCallback(error_callback);
	if (glfwInit() != GLFW_TRUE)
		throw Glfw_manager::InitFailException();
}

void				Glfw_manager::close_manager(void)
{
	glfwTerminate();
}

size_t				Glfw_manager::getActiveWindowNumber(void)
{
	return (_nb_active_win);
}

float				Glfw_manager::getTime(void)
{
	return (static_cast<float>(glfwGetTime()));
}

Input const			&Glfw_manager::getInput(void) const
{
	return (this->_input);
}

Window const		&Glfw_manager::getWindow(void) const
{
	return (this->_window);
}

bool				Glfw_manager::getMouseMode(void) const
{
	return (this->_mouse_exclusive);
}

float				Glfw_manager::getDeltaTime(void) const
{
	return (this->_delta_time);
}

void				Glfw_manager::create_resizable_window(std::string const &name,
						int const major, int const minor, int const w,
						int const h)
{
	auto	close_callback = [](GLFWwindow *win)
	{
		glfwSetWindowShouldClose(win, GLFW_TRUE);
	};

	auto	window_size_callback = [](GLFWwindow *win, int w, int h)
	{
		static_cast<void>(win);
		THIS_GLFW->_window.cur_win_h = h;
		THIS_GLFW->_window.cur_win_w = w;
		THIS_GLFW->_window.resized = true;
	};

	auto	framebuffer_size_callback = [](GLFWwindow *win, int w, int h)
	{
		static_cast<void>(win);
		oGL_module::oGL_update_framebuffer(w, h);
	};

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if ((this->_window.win = glfwCreateWindow(w, h, name.c_str(), NULL,
			NULL)) == NULL)
	{
		this->_window.win = nullptr;
		throw Glfw_manager::WindowFailException();
	}
	this->_win_name = name;
	this->_window.cur_win_h = h;
	this->_window.cur_win_w = w;
	glfwSetWindowCloseCallback(this->_window.win, close_callback);
	glfwSetWindowSizeCallback(this->_window.win, window_size_callback);
	glfwSetFramebufferSizeCallback(this->_window.win, framebuffer_size_callback);
	glfwSetInputMode(this->_window.win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowSizeLimits(this->_window.win, this->_window.min_win_w,
		this->_window.min_win_h, this->_window.max_win_w, this->_window.max_win_h);
	glfwSetWindowUserPointer(this->_window.win, this);
	glfwSetInputMode(this->_window.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(this->_window.win);
	_nb_active_win++;
}

void				Glfw_manager::destroy_window(void)
{
	if (this->_window.win != nullptr)
	{
		glfwDestroyWindow(this->_window.win);
		this->_window.win = nullptr;
		_nb_active_win--;
	}
}

void				Glfw_manager::init_input_callback(void)
{
	auto	keyboard_callback = [](GLFWwindow *win, int key, int scancode,
				int action, int mods)
	{
		static_cast<void>(scancode);
		static_cast<void>(mods);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(win, GLFW_TRUE);
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
				THIS_GLFW->_input.p_key[key] = PRESSED;
			else if (action == GLFW_RELEASE)
				THIS_GLFW->_input.p_key[key] = RELEASED;
		}
	};

	auto	cursor_position_callback = [](GLFWwindow *win, double xpos, double ypos)
	{
		static bool		first_time = true;

		static_cast<void>(win);
		if (first_time == true)
		{
			THIS_GLFW->_input.last_pos_x = xpos;
			THIS_GLFW->_input.last_pos_y = ypos;
			first_time = false;
		}
		THIS_GLFW->_input.pos_x = static_cast<GLfloat>(xpos);
		THIS_GLFW->_input.pos_y = static_cast<GLfloat>(ypos);
	};

	auto	mouse_button_callback = [](GLFWwindow *win, int button, int action,
				int mods)
	{
		static_cast<void>(win);
		static_cast<void>(mods);
		if (button >= 0 && button < 9)
		{
			if (action == GLFW_PRESS)
				THIS_GLFW->_input.p_mouse[button] = PRESSED;
			else if (action == GLFW_RELEASE)
				THIS_GLFW->_input.p_mouse[button] = RELEASED;
		}
	};

	glfwSetKeyCallback(this->_window.win, keyboard_callback);
	glfwSetMouseButtonCallback(this->_window.win, mouse_button_callback);
	glfwSetCursorPosCallback(this->_window.win, cursor_position_callback);
}

void				Glfw_manager::update_events(void)
{
	float		time;

	glfwPollEvents();
	time = glfwGetTime();
	this->_delta_time = time - this->_last_time;
	this->_last_time = time;
	if (this->_input.timer > 0.5f && this->_input.p_key[GLFW_KEY_SPACE] == PRESSED)
		this->toogle_mouse_exclusive();
	this->_input.timer += this->_delta_time;
}

void				Glfw_manager::swap_buffers(void)
{
	this->_window.resized = false;
	glfwSwapBuffers(this->_window.win);
}

bool				Glfw_manager::should_window_be_closed(void)
{
	if (!glfwWindowShouldClose(this->_window.win))
		return (false);
	return (true);
}

void				Glfw_manager::update_title_fps(void)
{
	std::stringstream	ss;
	float				fps;

	std::feclearexcept(FE_ALL_EXCEPT);
	fps = 1 / this->_delta_time;
	if (std::fetestexcept(FE_ALL_EXCEPT))
		fps = 0.0f;
	ss << this->_win_name << " | " << std::fixed << std::setprecision(1) << fps
		<< "fps";
	glfwSetWindowTitle(this->_window.win, ss.str().c_str());
}

void				Glfw_manager::update_title(std::string const &name)
{
	this->_win_name = name;
	glfwSetWindowTitle(this->_window.win, name.c_str());
}

void				Glfw_manager::toogle_mouse_exclusive(void)
{
	this->_mouse_exclusive = (this->_mouse_exclusive == true) ? false : true;
	(this->_mouse_exclusive == true) ? glfwSetInputMode(this->_window.win,
		GLFW_CURSOR, GLFW_CURSOR_DISABLED) : glfwSetInputMode(this->_window.win,
		GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	this->_input.timer = 0.0f;
}

Glfw_manager::InitFailException::InitFailException(void)
{
	this->_msg = "GLFW : Initilization failed !";
}

Glfw_manager::InitFailException::~InitFailException(void) throw()
{
}

Glfw_manager::WindowFailException::WindowFailException(void)
{
	this->_msg = "GLFW : Window creation failed !";
}


Glfw_manager::WindowFailException::~WindowFailException(void) throw()
{
}

size_t		Glfw_manager::_nb_active_win = 0;
