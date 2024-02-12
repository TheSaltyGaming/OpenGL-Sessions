#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <windows.h>

#include "Camera.h"
#include "Kube.h"

struct Vertex {
    float x, y, z, r, g, b;
};

#pragma region Public Variables

Camera MainCamera;
Kube k(1.0f);

bool firstMouse = true; // Used in mouse_callback


float lastX = 400, lastY = 300; //Used in mouse_callback

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

#pragma endregion

#pragma region Function Declarations

void setup(GLFWwindow*& window, unsigned& shaderProgram, unsigned& VBO, unsigned& VAO, unsigned& EBO,
               int& vertexColorLocation, int& value1, std::vector<float> floats);
void render(GLFWwindow* window, unsigned shaderProgram, unsigned VAO, int vertexColorLocation, std::vector<Vertex> points);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

std::string readFile(const std::string& filename);
std::vector<float> convertPointsToFloats(const std::vector<Vertex>& points, float scale);
std::vector<Vertex> readPointsFromFile(const std::string& filename);

#pragma endregion

#pragma region Settings

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string vertexShaderSourceString = readFile("NewVertShader.vert");
std::string fragmentShaderSourceString = readFile("FragmentShader.frag");

const char *vertexShaderSource = vertexShaderSourceString.c_str();
const char *fragmentShaderSource = fragmentShaderSourceString.c_str();

#pragma endregion


int main()
{
    std::vector<Vertex> points = readPointsFromFile("spiralpunkter2.txt");
    std::vector<float> floats = convertPointsToFloats(points, 1/9.9f);
    
    GLFWwindow* window;
    unsigned shaderProgram, VBO, VAO, EBO;
    int vertexColorLocation, value1;
    
    setup(window, shaderProgram, VBO, VAO, EBO, vertexColorLocation, value1, floats);
    
    render(window, shaderProgram, VAO, vertexColorLocation, points);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void setup(GLFWwindow*& window, unsigned& shaderProgram, unsigned& VBO, unsigned& VAO, unsigned& EBO,
               int& vertexColorLocation, int& value1, std::vector<float> floats)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Oppgave1", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        value1 = -1;
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        value1 = -1;
        return;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    vertexColorLocation = glGetUniformLocation(shaderProgram,"Color");

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, floats.size()*sizeof(float), floats.data() , GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);  

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return;
}

void render(GLFWwindow* window, unsigned shaderProgram, unsigned VAO, int vertexColorLocation, std::vector<Vertex> points)
{
    glm::mat4 trans = glm::mat4(1.0f);

    glm::mat4 projection;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        
        // input
        // -----
        processInput(window);

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        

        MainCamera.tick();

        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        view = glm::lookAt(MainCamera.cameraPos, MainCamera.cameraPos + MainCamera.cameraFront, MainCamera.cameraUp);
        
        // glm::mat4 view = glm::mat4(1.0f);
        // // note that we're translating the scene in the reverse direction of where we want to move
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Update the transformation matrix
        //trans *= glm::translate(glm::mat4(1.0f), glm::vec3(0.01f, -0.01f, 0.0f));
        
        //trans *= glm::rotate(glm::mat4(1.0f), glm::radians(1.0f)/100.f, glm::vec3(0.0f, 0.0f, 1.0f));
        
        // Pass the transformation matrix to the vertex shader
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        
        // render
        // ------
        
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glLineWidth(12);
        glDrawArrays(GL_LINE_STRIP, 0, points.size());
        
        
        // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        //
        // glUniform4f(vertexColorLocation, 1.0f, 1.0f, 0.0f, 1.0f);
        // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(unsigned int)));
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainCamera.cameraPos += cameraSpeed * MainCamera.cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainCamera.cameraPos -= cameraSpeed * MainCamera.cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainCamera.cameraPos -= glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainCamera.cameraPos += glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        MainCamera.cameraPos += cameraSpeed * MainCamera.cameraUp; // Move camera up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        MainCamera.cameraPos -= cameraSpeed * MainCamera.cameraUp; // Move camera down

}

/// \brief reads the content of a file and returns a stringstream.
/// \param filename the exact filename
/// \return the content of the file as stringstream
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/// \brief Reads points from a file and returns a vector of vertices
/// \param filename name of the file
/// \return 
std::vector<Vertex> readPointsFromFile(const std::string& filename) {
    std::vector<Vertex> points;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cout << "Unable to open file: " << filename << std::endl;
        return points;
    }

    // Skip the first line
    std::getline(file, line);

    while (std::getline(file, line)) {
        Vertex point;
        // Assuming the Point struct has members x, y, z, r, g, b
        int ret = sscanf_s(line.c_str(), "X: %f, Y: %f, Z: %f, r: %f, g: %f, b: %f", &point.x, &point.y, &point.z, &point.r, &point.g, &point.b);
        if (ret == 6) { // if all six values are successfully read
            points.push_back(point);
        } else {
            std::cout << "Failed to read line: " << line << std::endl;
        }
    }

    file.close();
    return points;
}

/// \brief Converts a vector of points to a vector of floats
/// \param points list of verftices
/// \param scale 
/// \return vector of floats
std::vector<float> convertPointsToFloats(const std::vector<Vertex>& points, float scale) {
    std::vector<float> floats;
    for (const auto& point : points) {
        floats.push_back(point.x*scale);
        floats.push_back(point.y*scale);
        floats.push_back(point.z*scale);
        floats.push_back(point.r);
        floats.push_back(point.g);
        floats.push_back(point.b);
    }
    return floats;
}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
 {
     if (firstMouse)
     {
         lastX = xpos;
         lastY = ypos;
         firstMouse = false;
     }
   
     float xoffset = xpos - lastX;
     float yoffset = lastY - ypos; 
     lastX = xpos;
     lastY = ypos;
 
     float sensitivity = 0.1f;
     xoffset *= sensitivity;
     yoffset *= sensitivity;
 
     MainCamera.yaw   += xoffset;
     MainCamera.pitch += yoffset;
 
     if(MainCamera.pitch > 89.0f)
         MainCamera.pitch = 89.0f;
     if(MainCamera.pitch < -89.0f)
         MainCamera.pitch = -89.0f;
 
     glm::vec3 direction;
     direction.x = cos(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
     direction.y = sin(glm::radians(MainCamera.pitch));
     direction.z = sin(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
     MainCamera.cameraFront = glm::normalize(direction);
 }  