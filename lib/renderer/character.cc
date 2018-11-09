//
// Created by jesse on 11/8/18.
//

#include "lib/renderer/character.h"

namespace pie {

float Character::vertex_data_[] = {
  0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
  0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
  0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,

  0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
  0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
  0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f
};

Character::Character(FontFace &face, const char c, const VFShader &sp)
  : shader_(sp)
{
  face.LoadChar(c);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // generate texture
  glGenTextures(1, &tex_id_);
  glBindTexture(GL_TEXTURE_2D, tex_id_);
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face.face()->glyph->bitmap.width,
      face.face()->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face.face()->glyph->bitmap.buffer
  );
  // set texture options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  size_ = std::make_pair(
      face.face()->glyph->bitmap.width,
      face.face()->glyph->bitmap.rows
  );
  bearing_ = std::make_pair(
      static_cast<int>(face.face()->glyph->bitmap_left),
      static_cast<int>(face.face()->glyph->bitmap_top)
  );
  advance_ = static_cast<GLuint>(face.face()->glyph->advance.x);
  // build vertex data
  this->BuildVertices();
}

void Character::Draw(GLFWwindow *window) const {
  // Bind generated glyph texture
  glBindTexture(GL_TEXTURE_2D, tex_id_);
  // define mvp matrix
  mat4x4 model, project, mvp;
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  float ratio = width/static_cast<float>(height);
  mat4x4_identity(model);
  mat4x4_scale_aniso(model, model, 1.0f, 1.0f, 1.0f);
  mat4x4_ortho(project, -1.0f, 1.0f, 0.0f, width, 0.0f, height);
  mat4x4_mul(mvp, project, model);
  // draw text
  shader_.Use((const float *) mvp);
  auto text_col = glGetUniformLocation(shader_.program(), "textColor");
  if (text_col >= 0) {
    glUniform3f(text_col, 0.5, 0.8, 0.2);
  }
  //glActiveTexture(GL_TEXTURE0);
  // update content of VBO
  //glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_data_), vertex_data_);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindVertexArray(vao_);
  // render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisableVertexAttribArray(0);
  //glBindVertexArray(0);
  //glBindTexture(GL_TEXTURE_2D, 0);
}

void Character::MoveTo(double x, double y) {

}

void Character::Rotate(double angle) {

}

void Character::BuildVertices() {
  float xpos = bearing_.first;
  float ypos = bearing_.second - size_.second;
  float w = size_.first;
  float h = size_.second;
  constexpr auto stride = stride_/sizeof(float);
  // set vertex positions
  vertex_data_[0*stride + 0] = xpos;
  vertex_data_[0*stride + 1] = ypos + h;
  vertex_data_[1*stride + 0] = xpos;
  vertex_data_[1*stride + 1] = ypos;
  vertex_data_[2*stride + 0] = xpos + w;
  vertex_data_[2*stride + 1] = ypos;
  vertex_data_[3*stride + 0] = xpos;
  vertex_data_[3*stride + 1] = ypos + h;
  vertex_data_[4*stride + 0] = xpos + w;
  vertex_data_[4*stride + 1] = ypos;
  vertex_data_[5*stride + 0] = xpos + w;
  vertex_data_[5*stride + 1] = ypos + h;
  // generate vertex array
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  // generate buffer object
  glGenBuffers(1, &vbo_);
  // bind buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data_), vertex_data_, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_data_), vertex_data_);
  // bind shader
  shader_.Bind(*this);
}

} // namespace pie