#include <iostream>
#include "../shaders/loadAndCompileShaders.hpp"
#include "renderSphere.hpp"

RenderSphere::RenderSphere(int indexSize, short int * index, int vertexSize, glm::vec3 * vertex[6]) {
  this->indexSize = indexSize;
  this->index = index;
  this->vertexSize = vertexSize;
  this->vertex = vertex;

  // Création de l'IBO
  glGenBuffers(1, &elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexSize * sizeof(short int), this->index, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // On Charge les vertex dans chaque VBOs et on y attache le VAO qui correspond
  for(int i=0; i<ACTIVE_FACES; i++) {
    glGenBuffers(1, &this->VBO[i]);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO[i]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vertexSize, 0, GL_STATIC_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * this->vertexSize, this->vertex[i]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &this->VAO[i]);
    glBindVertexArray(this->VAO[i]);
      glBindBuffer(GL_ARRAY_BUFFER, this->VBO[i]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  this->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  this->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  loadShader(&this->vertexShader, "shaders/vertex.shader");
  loadShader(&this->fragmentShader, "shaders/fragment.shader");

  compileShader(this->vertexShaderID, this->vertexShader);
  compileShader(this->fragmentShaderID, this->fragmentShader);

  this->programID = glCreateProgram();

  glAttachShader(this->programID, this->vertexShaderID);
  glAttachShader(this->programID, this->fragmentShaderID);

  glLinkProgram(this->programID);

  GLint Result = GL_FALSE;
  int InfoLogLength = 1024;
  char ProgramErrorMessage[1024] = {0};

  glGetProgramiv(this->programID, GL_LINK_STATUS, &Result);
  glGetProgramiv(this->programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  glGetProgramInfoLog(this->programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  if (strlen(ProgramErrorMessage) != 0) 
    std::cout << ProgramErrorMessage << "\n";

}

void RenderSphere::Render(int window_width, int window_height) {
  glUseProgram(this->programID);

    this->model = glm::rotate(this->model, (glm::mediump_float) 0.01, glm::vec3(1.0,1.0,1.0));
    this->view = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.0f, -2.00f));
    this->projection = glm::perspective(45.0, (double) window_width/window_height, 0.1, 10000.0);
          
    glEnable(GL_CULL_FACE);  
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(int i=0; i< ACTIVE_FACES;i++) {

    if (i%2 == 0) {
      glFrontFace(GL_CCW);
    }
    else {
      glFrontFace(GL_CW);
    }

    glBindVertexArray(this->VAO[i]);
      glUniformMatrix4fv(glGetUniformLocation(this->programID, "model"), 1, GL_FALSE, glm::value_ptr(this->model));
      glUniformMatrix4fv(glGetUniformLocation(this->programID, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
      glUniformMatrix4fv(glGetUniformLocation(this->programID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
        glDrawElements(GL_TRIANGLE_STRIP, this->indexSize, GL_UNSIGNED_SHORT, (GLvoid *) 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  glUseProgram(0);
}
