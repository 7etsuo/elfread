#include <glad/glad.h> // always include this before glfw3.h
#include <GLFW/glfw3.h>
#include <stdio.h>

void framebuffer_size_callback (GLFWwindow *window, int width, int height);
void process_input (GLFWwindow *window);

int
main (int argc, char **argv)
{
  if (!glfwInit ())
    {
      fprintf (stderr, "Failed to initialize GLFW\n");
      return 1;
    }

  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // glfwWindowHint (GLFW_DOUBLEBUFFER, GL_TRUE);

#if defined(__APPLE__)
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow (800, 600, "LearnOpenGL", NULL, NULL);
  if (!window)
    {
      fprintf (stderr, "Failed to create GLFW window\n");
      glfwTerminate ();
      return 1;
    }
  glfwMakeContextCurrent (window);

  if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress))
    {
      fprintf (stderr, "Failed to initialize GLAD\n");
      return 1;
    }

  glViewport (0, 0, 800, 600);
  glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);

  while (!glfwWindowShouldClose (window))
    {
      glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
      glClear (GL_COLOR_BUFFER_BIT);

      process_input (window);

      glfwPollEvents ();
      glfwSwapBuffers (window);
    }

  return 0;
}

void
framebuffer_size_callback (GLFWwindow *window, int width, int height)
{
  glViewport (0, 0, width, height);
}

void
process_input (GLFWwindow *window)
{
  if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose (window, GLFW_TRUE);
}