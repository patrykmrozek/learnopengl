#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

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
  std::cout << "hello" << "\n";

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

  // OPENGL GRAPHICS PIPELINE:
  // Vertex Shader (3D Coordinates)
  // Geometry Shader (generates other shapes by omitting new vertices to form
  // new primitives) Shape Assembly (assembles points in the primitive shape)
  // Rasterization (maps primitives to corresponding pixels on the screen)
  // Fragment Shader (calculate final color of a pixel - where all advanced
  // effects occur [lights, shadows, color..]) Tests and Blending (checks
  // corresponding depth value and checks if resulting fragment is in front or
  // behind other objects)

  // Vertex
  // Coordinates are normalized to a specific range between -1.0f and 1.0f on
  // all 3 axes (0, 0) is the center of the screen
  float vertices[]{-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

  // Vertex Buffer Object VBO
  // Can store a large amount of vertices on the GPU's memory
  // Used to send large batches of data to the GPU all at once without having to
  // send them one at a time Sending data from GPU to CPU is relatively slow -
  // better to send as much as possible at once

  unsigned int VBO, VAO; // VAO also created here
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(
      VAO); // bind VAO first, then bind VBOs, then configure Vertex attribs
  glBindBuffer(
      GL_ARRAY_BUFFER,
      VBO); // binds newly created buffer with the GL_ARRAY_BUFFER target
  // from now on, any buffer calls we make on this target will be used to
  // configure the currently bound buffer (VBO) Then we can make a call to
  // glBufferData that copies previously defined vertex data into buffers memory

  // glBufferData(); copies user-defined data into currently bound buffer.
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // usage parameter:
  //- GL_STREAM_DRAW: data is set only once and used by the GPU a few times
  //- GL_STATIC_DRAW: data is set only once and used many times
  //- GL_DYNAMIC_DRAW: data is changed a lot and used many times

  // Shader
  // GLSL - GL Shader Language
  /*
   #version 330 core
   layout (location = 0) in vec3 aPos;

   void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   }
   */

  // Compiling a Shader
  // 1) VERTEX SHADER
  const char *vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main() {\n"
      " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

  // in order for OpenGL to use a shader, it has to dynamically compile it at
  // run-time
  unsigned int vertexShader;
  vertexShader =
      glCreateShader(GL_VERTEX_SHADER); // assigns an id to this shader object
  // now attach the source code to the shader object and compile the shader
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // Error checking the shader
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << "\n";
  }

  // 2) FRAGMENT SHADER
  // all about calculating the color output of pixels
  // FragColor = vec4(r, g, b, a);

  const char *fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "void main() {\n"
      "FragColor = vec4(1.0f, 0.1f, 0.9f, 1.0f);\n"
      "}//0";

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // Shader Program
  // shader program object is the final linked version of multiple combined
  // shaders
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram(); // creates a program and returns an id
                                     // reference to new program object
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
  }
  glUseProgram(shaderProgram); // every shader and rendering call after this
                               // will now use this object

  // once the shaders are linked to the program object they can be deleted
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Linking Vertex Attributes
  // vertex shader allows us to specify any input we want in the form of vertex
  // attributes because of this we need to specify what part of the input data
  // goes to which vertex attribute in the shader we need to specify how openGL
  // should interpret the vertex data before render vertex format:
  //- pos data stored as 32bit floats
  //- each pos has 3 values
  //- no space between each set of 3 values - tight packed
  //- first value is at the beginning of the buffer

  // glVertexAttribPointer(
  //   index: which vertex attribute we want to configure
  //   size: size of the vertex attribute (vec3 so 3 values)
  //   type: type of data (GL_FLOAT = vec* in GLSL consists of fp values)
  //   normalized: if we want the data to be normalized
  //   stride: the space between consecutive vertex attributes (3 floats so
  //   3*sizeof(float)) offset: where the position data begins in the buffer.
  //   Since at the start, the value is 0 - but its expecting a pointer to cast
  //   to nullptr
  //)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // how drawing an object in OpenGL should look:
  // 1) copy vertices array in buffer (binfBuffer, bufferData..)
  // 2) set vertex attribute pointers (vertexAttribPointer..)
  // 3) use shader program to render an object (useProgram)
  // 4) draw object (?..)

  // can get quite bothersome if there were many objects to render
  // How can we store all these state configs into an object and bind the object
  // to restore its state? Vertex Array Object VAO bound just like VBO and any
  // subsequent calls from that point will be stored inside the VAO whenever we
  // want to draw the object, we can bind the corresponding VAO.

  // VAO stores the following:
  //   calls to glEnableVertexAttribArray or glDisableVetexAttribArray
  //   vertex attribute configs via glVertexAttribPointer
  //   VBOs associated with vertex attributes by calls to glVertexAttribPointer

  // Generating a VAO - done at the beginning of the program and in the while
  // loop

  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.6f, 0.1f, 0.9f, 0.8f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
