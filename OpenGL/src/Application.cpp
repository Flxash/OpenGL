#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    //using this enum as an indexing method into the string stream array
    //the string stream array will store the associated shader type code contents

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        //alloca is a function that c gives that klets you allocate on the stack dynamically
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "ERROR!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
       -0.5f, -0.5f, //0
        0.5f, -0.5f, //1
        0.5f, 0.5f,  //2
       -0.5f, 0.5f   //3
    };

    unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    //generates one buffer at a time provide an address to an integer (unsigned)
    //so it can write it's ID to that vraible 
    //everything in openGL that is generated (contextially) gets assigned a unique identifier
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //selecting that buffer by its ID
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
    //given a target buffer and its intended size/ size of data going into buffer
    //you also have to provide a pointer to the data that will be coppied
    //and usage, go to docs.gl and you can find all function info

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    //now what is a shader: program that runs on the gpu -> parralell algorithms

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW);
   
    //the vertex shader basically takes in a set of vertecies and formats them in a way and prepares them as vertecies
    //this step is not redundant as it sets the vertecies properly
    //the fragment shader is basically what fills in individual pixels in a specieifeied vertex bound space
    //the space is bound by the vertecies provided by our vertex shader
    //the fragment shader rasterizes. which means the pixels get "shaded in" like a coloring book
    //the fragment shader gets called soooooo many times the vertex shader only gets callled a cpuple time in comparison
    //fragment shaders are a lot more expensive so try to do as many operations possible in the vertex shader
    //obv thats not always possible because some operations are needed to be done on a pixel basis (lighting)
    //our current pipeline: vertex shader -> fragment shader

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate(); 
    return 0;
}