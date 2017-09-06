/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cledant <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/04 13:11:10 by cledant           #+#    #+#             */
/*   Updated: 2017/09/06 10:59:04 by cledant          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_HPP
# define SHADER_HPP

# include "glfw3.h"
# include "GeneralException.hpp"
# include <iostream>

class Shader
{
	public :

		Shader(std::string const &name, std::string const &vs_path,
				std::string const &fs_path);
		virtual ~Shader(void);

		std::string const		&getName(void) const;
		GLuint					getShaderProgram(void) const;
		void					use(void);
		void					setMat4(std::string const &name,
									glm::mat4 const &mat4);

	class FileOpenFailException : public GeneralException
	{
		public :

			explicit FileOpenFailException(void);
			virtual ~FileOpenFailException(void) throw();
	};

	class AllocationException : public GeneralException
	{
		public :

			explicit AllocationErrorException(void);
			virtual ~AllocationErrorException(void) throw();
	};

	class CompileException : public GeneralException
	{
		public :

			explicit CompileException(void);
			virtual ~CompileException(void) throw();
	};

	class LinkException : public GeneralException
	{
		public :

			explicit LinkException(void);
			virtual ~LinkException(void) throw();
	};

	private :

		std::string			_name;
		GLuint				_shader_program;

		Shader(Shader const &src);
		Shader		&operator=(Shader const &rhs);

		static GLuint		load_shader(std::string const &path, GLenum type);
		static GLuint		compile_program(GLuint vs, GLuint fs);
		static void			get_shader_error(GLuint shader);
		static void			read_file(std::string const &path, std::string &content);
};

#endif
