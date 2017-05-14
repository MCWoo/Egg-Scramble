#include "Utils.h"

#include <Windows.h>
#include <fstream>
#include <GLFW/glfw3.h>

bool Utils::ReadFile(const std::string& filename, std::string& outString)
{
    std::ifstream f(filename);
    if (f.is_open())
    {
        std::string line;
        while (getline(f, line))
            outString.append(line).append("\n");
        f.close();
    }
    else
    {
        fprintf(stderr, "Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", filename.c_str());
        return false;
    }

    return true;
}

double Utils::CurrentTime()
{
#ifdef EGGS_CLIENT
    return glfwGetTime();
#elif defined(EGGS_SERVER)
    return GetTickCount();
#endif
}