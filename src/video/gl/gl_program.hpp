//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_VIDEO_GL_GL_PROGRAM_HPP
#define HEADER_SUPERTUX_VIDEO_GL_GL_PROGRAM_HPP

#include <memory>

#include "video/glutil.hpp"
#include "video/gl/gl_shader.hpp"

class GLProgram
{
public:
  GLProgram();
  ~GLProgram();

  GLint get_attrib_location(const char* name)
  {
    return glGetAttribLocation(m_program, name);
  }

private:
  bool get_link_status() const;
  bool get_validate_status() const;
  std::string get_info_log() const;

private:
  GLuint m_program;

  std::unique_ptr<GLShader> m_frag_shader;
  std::unique_ptr<GLShader> m_vert_shader;

private:
  GLProgram(const GLProgram&) = delete;
  GLProgram& operator=(const GLProgram&) = delete;
};

#endif

/* EOF */
