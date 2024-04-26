#include <glad/glad.h> // always include this before glfw3.h
#include <GLFW/glfw3.h>
#include <stdio.h>

void framebuffer_size_callback (GLFWwindow *window, int width, int height);
void process_input (GLFWwindow *window);
int glfw_init_and_configure (void);
GLFWwindow *glfw_window_creation (void);
int glfw_load_opengl_function_pointers (void);
void glfw_render_loop (GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int
main (int argc, char **argv)
{
  int ret = 1;

  GLFWwindow *window = glfw_do_init ();
  if (window)
    {
      ret = 0;
      glfw_render_loop (window);
    }

  glfwTerminate ();
  return ret;
}

int
glfw_init_and_configure (void)
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
}

GLFWwindow *
glfw_window_creation (void)
{
  GLFWwindow *window
      = glfwCreateWindow (SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
  if (!window)
    {
      fprintf (stderr, "Failed to create GLFW window\n");
      return window;
    }
  glfwMakeContextCurrent (window);
  glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);
}

int
glfw_load_opengl_function_pointers (void)
{
  if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress))
    {
      fprintf (stderr, "Failed to initialize GLAD\n");
      return 1;
    }
  return 0;
}

GLFWwindow *
glfw_do_init (void)
{
  if (glfw_init_and_configure ())
    return NULL;

  GLFWwindow *window = glfw_window_creation ();
  if (!window)
    {
      glfwTerminate ();
      return NULL;
    }

  if (glfw_load_opengl_function_pointers ())
    {
      glfwTerminate ();
      return NULL;
    }

  return window;
}

void
glfw_render_loop (GLFWwindow *window)
{
  while (!glfwWindowShouldClose (window))
    {
      process_input (window);

      glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
      glClear (GL_COLOR_BUFFER_BIT);

      glfwSwapBuffers (window);
      glfwPollEvents ();
    }
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
