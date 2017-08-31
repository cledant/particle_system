/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cledant <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/30 13:58:09 by cledant           #+#    #+#             */
/*   Updated: 2017/08/31 12:01:12 by cledant          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer.hpp"

Renderer::Renderer(void)
{
}

Renderer::~Renderer(void)
{
}

Renderer::Renderer(Renderer const &src)
{
	static_cast<void>(src);
}

Renderer		&Renderer::operator=(Renderer const &rhs)
{
	static_cast<void>(rhs);
	return (*this);
}

void			Renderer::oCL_check_error(cl_int const err, cl_int const ref)
{
	if (err != ref)
		throw Renderer::oCLFailException();
}

void			Renderer::oCL_init(void)
{
	this->oCL_get_platform_list();
	if (this->oCL_select_platform_from_name("NVIDIA") == false &&
		this->oCL_select_platform_from_name("AMD") == false &&
		this->oCL_select_platform_from_name("Apple") == false)
	{
		std::cout << "No Platform recognized" << std::endl;
		throw Renderer::oCLFailException();
	}
	this->oCL_get_device_list(CL_DEVICE_TYPE_GPU);
	this->oCL_select_first_oGL_sharing_device();
	std::cout << "OpenCL device : " << this->_cl_device.getInfo<CL_DEVICE_NAME>()
		<< std::endl;
	this->oCL_create_context();
	this->oCL_create_command_queue();
}

void			Renderer::oCL_get_platform_list(void)
{
	cl_int		err;

	err = cl::Platform::get(&(this->_cl_platform_list));
	Renderer::oCL_check_error(err, CL_SUCCESS);
	if (this->_cl_platform_list.size() == 0)
	{
		std::cout << "No Platform" << std::endl;
		throw Renderer::oCLFailException();
	}
}

void			Renderer::oCL_get_device_list(cl_device_type type)
{
	std::vector<cl::Device>::iterator		it;
	cl_int									err;

	err = this->_cl_platform.getDevices(type, &(this->_cl_device_list));
	Renderer::oCL_check_error(err, CL_SUCCESS);
	if (this->_cl_device_list.size() == 0)
	{
		std::cout << "No Device" << std::endl;
		throw Renderer::oCLFailException();
	}
}

bool			Renderer::oCL_select_platform_from_name(std::string const name)
{
	std::vector<cl::Platform>::iterator		it;
	std::string								value;

	for (it = this->_cl_platform_list.begin();
		it != this->_cl_platform_list.end(); ++it)
	{
		if (it->getInfo(CL_PLATFORM_NAME, &value) == CL_SUCCESS)
		{
			if (value.find(name) == 0)
			{
				this->_cl_platform = *it;
				return (true);
			}
		}
	}
	return (false);
}

void			Renderer::oCL_select_first_oGL_sharing_device(void)
{
	std::vector<cl::Device>::iterator		it;
	std::string								value;

	for (it = this->_cl_device_list.begin(); it != this->_cl_device_list.end(); ++it)
	{
		if (it->getInfo(CL_DEVICE_EXTENSIONS, &value) == CL_SUCCESS &&
			(value.find("cl_khr_gl_sharing") != std::string::npos |
			value.find("cl_APPLE_gl_sharing") != std::string::npos))
		{
			this->_cl_device = *it;
			return ;
		}
	}
	std::cout << "No Device with OpenGL sharing capability" << std::endl;
	throw Renderer::oCLFailException();
}

void			Renderer::oCL_create_context(void)
{
	cl_int				err;
	auto	oCL_error_callback = [](const char *err_info, const void *priv_info_size,
				size_t cb, void *user_data)
	{
		static_cast<void>(priv_info_size);
		static_cast<void>(cb);
		static_cast<void>(user_data);
		std::cout << "Context error :" << err_info << std::endl;
	};
#ifdef __APPLE__
	CGLContextObj		kCGLContext = CGLGetCurrentContext();
	CGLShareGroupObj	kCGLShareGroup = CGLGetShareGroup(kCGLContext);	
	cl_context_properties prop[] =
	{
	  CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
	  (cl_context_properties) kCGLShareGroup,
	  0
	};
#endif

	this->_cl_context = cl::Context({this->_cl_device}, prop, oCL_error_callback,
		NULL, &err);
	Renderer::oCL_check_error(err, CL_SUCCESS);
}

void			Renderer::oCL_create_command_queue(void)
{
	cl_int		err;

	this->_cl_cc = cl::CommandQueue(this->_cl_context, this->_cl_device, 0,
		&err);
	Renderer::oCL_check_error(err, CL_SUCCESS);
}

void			Renderer::oCL_add_kernel_from_file(std::string const file)
{
	std::string		kernel;

	Renderer::read_file(file, kernel);
	this->_cl_sources.push_back({kernel.c_str(), kernel.length()});
}

void			Renderer::oCL_compile_program(void)
{
	cl_int		err;

	this->_cl_program = cl::Program(this->_cl_context, this->_cl_sources, &err);
	Renderer::oCL_check_error(err, CL_SUCCESS);
	if ((err = this->_cl_program.build({this->_cl_device})) != CL_SUCCESS)
	{
		std::cout << "OpenCL : Error Building : " <<
			this->_cl_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->_cl_device) <<
			std::endl;
		throw Renderer::oCLFailException();
	}
}

void			Renderer::read_file(std::string const path, std::string &content)
{
	std::fstream	fs;

	fs.open(path, std::fstream::in);
	content.assign((std::istreambuf_iterator<char>(fs)),
		std::istreambuf_iterator<char>());
	fs.close();
}

Renderer::oCLFailException::oCLFailException(void)
{
	this->_msg = "OpenCL : Something failed !";
}

Renderer::oCLFailException::~oCLFailException(void) throw()
{
}
