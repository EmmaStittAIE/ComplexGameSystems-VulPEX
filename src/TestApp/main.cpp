#include <iostream>

#include <glm.hpp>
#include <GLFW/glfw3.h>

int main(void)
{
    glm::vec3 test(1, 2, 3);
    
    std::cout << "Hello Premake World!" << std::endl;
    
    std::cout << "Vec3(" << test.x << ", " << test.y << ", " << test.z << ")" << std::endl;

    if(!glfwInit()) {
        std::cout << "GLFW failed to initialise" << std::endl;
		return 1;
	}

    std::cout << "GLFW initialised successfully" << std::endl;
	glfwTerminate();
	return 0;
}
