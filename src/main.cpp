#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <spdlog/spdlog.h>


#include <stb_image.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>
#include <cmath>
#include <data.h>
#include <shader.h>

#include <todo.h>
#include <camera.h>

constexpr int AppWindowWidth  = 1920;
constexpr int AppWindowHeight = 1080;
int gameWindowWidth  = 800;
int gameWindowHeight = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
void processExit(GLFWwindow* window);
void processInput(GLFWwindow* window, int key, std::function<void(void)> func);
void renderMainImGui(unsigned int texColorBuffer);

// clang-format off
std::vector<float> vertices = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

std::vector<unsigned int> indices {
    // 注意索引从0开始! 
    // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
    // 这样可以由下标代表顶点组合成矩形
    0, 1, 3,
    1, 2, 3,
};

// 立方体位置
std::vector<glm::vec3> cubePositions = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), 
    glm::vec3(-3.8f, -2.0f, -12.3f), 
    glm::vec3( 2.4f, -0.4f, -3.5f), 
    glm::vec3(-1.7f,  3.0f, -7.5f), 
    glm::vec3( 1.3f, -2.0f, -2.5f), 
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

std::vector<float> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions           // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

// clang-format on

bool hiddenCursor = true;

float alpha = 0.0;

float     deltaTime     = 0.0f; // 当前帧和上一帧的时间差
float     lastFrameTime = 0.0f; // 上一帧的时间

float lastX = (float)gameWindowWidth / 2.f, lastY = (float)gameWindowHeight / 2.f;

Utils::Camera camera{glm::vec3{0.f, 0.f, 3.f}};

int main(void) {

    spdlog::info("Welcome to spdlog!");
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return EXIT_FAILURE;
    
    
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(AppWindowWidth, AppWindowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking Windows
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi Viewports


    // OpenGL 版本号输出
    std::cout << glGetString(GL_VERSION) << std::endl;
    // 设置视口大小
    glViewport(0, 0, gameWindowWidth, gameWindowHeight);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 隐藏鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 设置鼠标事件监听函数
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window,mouse_button_callback);

    // 创建 VAO
    unsigned int VAO = Utils::createVAO(vertices);
    // 创建 EBO
    Utils::createEBO(indices);

    // 如何从VBO解析顶点属性,并将状态保存到 VAO
    // '0' => Corresponding `location` in vertex shader Attribute value
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);                   // position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // texture_box
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(5 * sizeof(float))); // texture_smile

    // 以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

     // 屏幕 VAO
    unsigned int quadVAO = Utils::createVAO(quadVertices);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    // 生成着色器程序对象
    Utils::Shader gameShader("./src/shaders/vs/shader.vs", "./src/shaders/fs/shader.fs");
    Utils::Shader applicationShader("./src/shaders/vs/application.vs", "./src/shaders/fs/application.fs");

    // 生成纹理
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // stb 加载图片

    unsigned char* texture1_data =
        stbi_load("./src/assets/container.jpg", &width, &height, &nrChannels, 0);
    if (texture1_data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(texture1_data);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    // GL_REPEAT            重复纹理图像
    // GL_MIRRORED_REPEAT   和GL_REPEAT一样，但每次重复图片是镜像放置的
    // GL_CLAMP_TO_EDGE     纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果
    // GL_CLAMP_TO_BORDER   超出的坐标为用户指定的边缘颜色。
    //
    // # WARNING #
    //    根据 https://stackoverflow.com/questions/26589683/access-violation-when-calling-gltextureparameteri-with-opengl-and-devil
    //    这里原先使用 glTextureParameteri(), 为 OpenGL 4.5 中的新入口点，
    //    这个想法是您可以在不绑定对象的情况下修改对象的状态。这可以通过减少 API 调用次数和驱动程序开销来提高效率。
    //    与相应的旧入口点相比，这些新入口点将对象名称作为第一个参数，而旧入口点将目标作为第一个参数。
    //    原先第一个参数为目标
    //       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //    新的则可以使用对象名称
    //       glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // stb 加载图片
    unsigned char* texture2_data =
        stbi_load("./src/assets/awesomeface.png", &width, &height, &nrChannels, 0);
    if (texture2_data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture2_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(texture2_data);

    // Shader 配置
    gameShader.use();
    // 设置纹理坐标
    gameShader.setInt("texture_box", 0);
    gameShader.setInt("texture_smile", 1);

    applicationShader.use();
    applicationShader.setInt("screenTexture", 0);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 创建帧缓冲
    unsigned int gameWindowFbo;
    glGenFramebuffers(1, &gameWindowFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gameWindowFbo);

    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gameWindowWidth, gameWindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gameWindowWidth, gameWindowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        /* Poll for and process events */
        glfwPollEvents();

        processExit(window);
        //** HomeWork T4 **//
        processInput(window, GLFW_KEY_DOWN, [&]() {
            alpha -= 0.005f;
            if (alpha <= 1e-7)
                alpha = 0.0f;
            gameShader.setFloat("alpha", alpha);
        });
        processInput(window, GLFW_KEY_UP, [&]() {
            alpha += 0.005f;
            if (alpha > 1.0f)
                alpha = 1.0f;
            gameShader.setFloat("alpha", alpha);
        });

        glBindFramebuffer(GL_FRAMEBUFFER, gameWindowFbo); // 帧缓冲
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清理颜色和深度缓冲位
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        // 创建投影矩阵 (MVP - Projection)
        auto projection = glm::perspective(glm::radians(camera.m_zoom), (float)gameWindowWidth / (float)gameWindowHeight, 0.1f, 100.f);

        // 使用 ShaderProgram
        gameShader.use();
        // 绑定 VAO
        glBindVertexArray(VAO);
        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = camera.GetViewMatrix();

        gameShader.setMatrix4f("view", view);
        gameShader.setMatrix4f("projection", projection);

        for (std::size_t i = 0; i < 10; ++i) {
            // 创建模型矩阵 (MVP - Model)
            glm::mat4 model{1.0f};
            model = glm::translate(model, cubePositions[i]);
            float angle;
            if (i == 0 || i % 3 == 0)
                angle = 20.f * (i + 1);
            else
                angle = 0;
            model = glm::rotate(model, glm::radians(angle) * (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            gameShader.setMatrix4f("model", model);

            // //=========== Just Use VBO ===========//
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //============= Use EBO! =============//
            // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认帧缓冲
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        // applicationShader.use();
        // glDisable(GL_DEPTH_TEST);
        // glBindVertexArray(quadVAO);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        // // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawArrays(GL_TRIANGLES, 0, 6);  

        renderMainImGui(texColorBuffer);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        // 计算每帧用时和时间差
        float currentFrameTime = glfwGetTime();
        deltaTime              = currentFrameTime - lastFrameTime;
        lastFrameTime          = currentFrameTime;
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteFramebuffers(1, &gameWindowFbo);

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool first_in = true;
    
    if (first_in) {
        first_in = false;
        lastX    = xpos;
        lastY    = ypos;
    }
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX         = xpos;
    lastY         = ypos;
    
    camera.ProcessMouseMovement(xOffset, yOffset, true);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    camera.ProcessMouseScroll(yOffset);
}
void mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
    if (hiddenCursor == false) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && glfwGetWindowAttrib(window, GLFW_HOVERED)) {
            spdlog::info("Hidden Cursor due to click window");
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // hiddenCursor = true;
        }
    }
}
void processExit(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void processInput(GLFWwindow* window, int key, std::function<void(void)> func) {
    using Utils::Camera;
    
    if (glfwGetKey(window, key) == GLFW_PRESS)
        func();
    else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        spdlog::info("Show Cursor due to press F5");
        if (hiddenCursor) {
            hiddenCursor = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    // 摄像机移动控制
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
    }
    
}

void renderMainImGui(unsigned int colorBuffer)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    float menuHeight = 0;
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Button("test");
            ImGui::EndMenu();
        }

        float button_height = 20;
        float button_width = 60;
        ImGui::SameLine();
        ImGui::Button("Button", ImVec2(button_width, button_height));
        menuHeight = ImGui::GetFrameHeight();
    }
    ImGui::EndMainMenuBar();
    ImGui::DockSpaceOverViewport();
    ImGui::Begin("Object Lists");
    {
        ImGui::Text("Object Occupy");
    }
    ImGui::End();

    ImGui::Begin("Game Window");
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::BeginChild("GameRender");
        auto ret = ImGui::GetWindowSize();
        gameWindowWidth = ret.x;
        gameWindowHeight = ret.y;
        // spdlog::info("wize = ({}, {})", wsize.x, wsize.y);
        ImGui::Image((ImTextureID)(unsigned long)colorBuffer, ret, ImVec2(0, 1), ImVec2(1, 0));        //自定义GUI内容
        ImGui::EndChild();
    }
    ImGui::End();

    ImGui::Begin("Assets");
    {
        ImGui::Text("Assets Occupy");
    }
    ImGui::End();

    ImGui::Begin("Config");
    {
        ImGui::Text("Config Occupy");
    }
    ImGui::End();

    
    ImGui::Render();
}