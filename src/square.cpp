#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int success;
char infoLog[512];

//get shader from a file and place it in shaderSource
static std::string parseShaderSource( const std::string &filePath) {
  std::ifstream stream(filePath);
  std::string line;
  std::string shaderSource;
  while (std::getline(stream, line)) {
    shaderSource += line + "\n"; 
  }
  return shaderSource;
}

// callback function GLFW will call every time we resize the window
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

int main() {
  std::cout << "square" << "\n";

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // create window
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "GLFW Window error" << "\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLAD
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    std::cout << "GLAD Init Failed!" << "\n";
    return -1;
  }
  // tell OpenGL the size of the rendering window
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, // top left
      0.0f,  0.0f,  0.0f  // middle
  };
  unsigned int indices[] = {
      // note that we start from 0!
      2, 3, 4, // first triangle
      0, 1, 4, // second triangle
      0, 3, 4, // third triangle
      1, 2, 4  // fourth triangle
  };

  //get shaderSources from files
  const char *vertexShaderSource;
  const char *fragmentShaderSource;
  
  std::string vertexShaderCode = parseShaderSource("../res/shaders/basic.vert");
  vertexShaderSource = vertexShaderCode.c_str();

  std::string fragmentShaderCode = parseShaderSource("../res/shaders/basic.frag");
  fragmentShaderSource = fragmentShaderCode.c_str();

  std::cout << "VERTEX CODE: " << vertexShaderCode << "\n";
  std::cout << "FRAGMENT CODE: " << fragmentShaderCode << "\n";


  unsigned int VAO, VBO, EBO; // Element Buffer Object
  // VAO
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  // VBO
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // EBO - bind the EBO and copy indices into with glBufferData
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0); // unbind vertex array

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << "\n";
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << "\n";
  }

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << GL_LINK_STATUS << "\n";
  }

  glUseProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws in a 'wireframe' mode
  bool wireframe = true;

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        wireframe = !(wireframe);
      } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wireframe = !(wireframe);
      }
    }

//    std::cout << "WIREFRAME: " << wireframe << "\n";

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    // glDrawElements: takes indices currently bound to GL_ELEMENT_ARRAY_BUFFER
    // We have to bind the corresponding EBO each time we want to render with
    // the indices The vertex array also keeps track of EBO bindings The last
    // EBO object that is bound while the VAO is bound, is stored as a VAOs
    // element buffer object
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();

  return 0;
}
