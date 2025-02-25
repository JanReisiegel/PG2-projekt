#include <iostream>

void key_callback(GLFWwindow* , int , int , int , int );
void error_callback(int , const char* );
void fbsize_callback(GLFWwindow* , int , int );
void mouse_button_callback(GLFWwindow* , int , int , int );
void cursor_position_callback(GLFWwindow* , double , double );
void scroll_callback(GLFWwindow* , double , double );
extern bool vsync;