// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

glm::mat4 CWindow::m_VP;

CWindow::CWindow()
{
    g_Window = NULL;
    m_ProgramId = 0;
}

CWindow::~CWindow()
{
}

const glm::mat4& CWindow::GetVP() { return m_VP; }

bool CWindow::Initialize()
{
    std::cout << "Iniciando glfw...\n";

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    g_Window = glfwCreateWindow(g_WindowMaxX, g_WindowMaxY, "cg", NULL, NULL);
    if (!g_Window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(g_Window);
    glfwSwapInterval(1); // Enable vsync

    ::CEvent::Initialize();

    std::cout << "Iniciando glew...\n";

    /* Initialize glew and OpenGL functions */
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;

        glfwTerminate();
        return false;
    }

    std::cout << "Iniciando imgui...\n";

    /* Initialize ImGui */
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    /* Compile and Link Shaders */
    std::cout << "Compilando o Vertex Shader...\n";
    GLuint vShaderId = CompileShader(CUtil::m_VertexShader, GL_VERTEX_SHADER);

    std::cout << "Compilando o Fragment Shader...\n";
    GLuint fShaderId = CompileShader(CUtil::m_FragmentShader, GL_FRAGMENT_SHADER);

    m_ProgramId = LinkProgram(vShaderId, fShaderId);

    // Use Shaders.
    glUseProgram(m_ProgramId);

    std::cout << "Carregando os modelos...\n";

    m_Game.Initialize();

    // Configure the Lines.
    glLineWidth(2.f);
    glEnable(GL_LINE_SMOOTH);

    std::cout << "Iniciando...\n";
	return true;
}

void CWindow::Cleanup()
{
    /* Cleanup ImGui */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    /* Cleanup GLFW */
    glfwDestroyWindow(g_Window);
    glfwTerminate();
}

bool CWindow::Render()
{
    // Clear OpenGl frame.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set Transforms.
    glm::mat4 projection = glm::perspective(glm::radians(m_Camera.m_Zoom), (float)g_WindowMaxY / (float)g_WindowMaxX, 0.1f, 100.0f);
    glm::mat4 view = m_Camera.GetViewMatrix();

    m_VP = (projection * view);
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramId, "u_vp"), 1, GL_FALSE, glm::value_ptr(m_VP));

    // Draw Lights.
    //for (auto& it : m_Light)
    m_Light.Draw(m_ProgramId);
    
    // Draw objects.
    for (unsigned int i = 0; i < MAX_OBJECT; ++i)
    {
        CModel* m = CModel::GetModel(i);
        if (m)
            m->Draw(m_ProgramId, m_VP);
    }

    // Start the Dear ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create ImGui Sliders.
    ImGui::Begin("Space Invaders");
        if (ImGui::Button("Iniciar Novo Jogo"))
        {
            CModel::DeleteAllModel();
            m_Game.Initialize();
        }

        //ImGui::RadioButton("Arrastar Vertices/Triangulos", &CUtil::m_EditorType, 0);
        //ImGui::RadioButton("Criar Vertices", &CUtil::m_EditorType, 1);
        //ImGui::RadioButton("Remover Vertices", &CUtil::m_EditorType, 2);
        //ImGui::RadioButton("Mover Objetos", &CUtil::m_EditorType, 3);
        //ImGui::RadioButton("Criar Curva", &CUtil::m_EditorType, 4);
        //ImGui::RadioButton("Visualizar", &CUtil::m_EditorType, 5);
        //if (ImGui::Button("Criar Modelo #1"))
        //    CreateModel(0, "Model/main.obj");
        //if (ImGui::Button("Criar Modelo #2"))
        //    CreateModel(1, "Model2/main.obj");
        //if (ImGui::Button("Criar Modelo #3"))
        //    CreateModel(2, "Model3/main.obj");
    ImGui::End();

    // Rendering the ImGui.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers.
    glfwSwapBuffers(g_Window);

    // Poll for and process events.
    glfwPollEvents();

    // Process Input events.
    for (const auto& it : *g_EventList)
        it->ProcessInput(g_Window);

    return glfwWindowShouldClose(g_Window);
}

CModel* CWindow::CreateModel(int type, const char* fileModel)
{
    glm::vec3 newPosition = glm::vec3((-0.75f + 1.8f * (5.f - (rand() % 10))), 0.f, -30.f);

    if (type == 4 || type == 5 || CGame::CheckMovement(newPosition) == NULL && CUtil::g_EnemyCount < 32)
    {
        CModel* m = CModel::LoadModel(fileModel);
        if (m)
        {
            if (type < 4)
            { // Enemys.
                m->m_Scale = glm::vec3(0.1f, 0.1f, 0.1f);
                
                if (type == 1 || type == 2 || type == 3)
                    *m->GetPosition() = newPosition;
            }
            else if (type == 4)
            { // Bullets.
                m->m_Scale = glm::vec3(0.05f, 0.f, 0.5f);

                newPosition = *CModel::GetModel(0)->GetPosition();
                newPosition.z -= 1.5f;

                *m->GetPosition() = newPosition;
            }
            else if (type == 5)
            { // Stars.
                float x = ((rand() % 20000) - 10000) / 1000.f;
                float z = ((rand() % 30000) - 1000) / 1000.f;

                m->m_Scale = glm::vec3(0.03f, 0.f, 0.05f);
                newPosition = glm::vec3(x, -5.f, -z);

                *m->GetPosition() = newPosition;
            }
            else if (type == 6)
            { // Enemy Bullets.
                m->m_Scale = glm::vec3(0.05f, 0.f, 0.5f);

                *m->GetPosition() = newPosition;
            }

            m->m_InitPosition = newPosition;
            m->m_SpawnTime = g_LastTime;
            m->m_ModelType = type;

            if (type >= 1 && type <= 3)
                CUtil::g_EnemyCount++;

            return m;
        }
    }

    return NULL;
}

GLuint CWindow::CompileShader(const char* shaderCode, GLenum type)
{
    GLuint shaderId = glCreateShader(type);

    if (shaderId == 0) { // Error: Cannot create shader object
        std::cout << "Error creating shaders";
        return 0;
    }

    // Attach source code to this object
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId); // compile the shader object

    GLint compileStatus;

    // check for compilation status
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus) { // If compilation was not successful
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* cMessage = new char[length];

        // Get additional information
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        std::cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

GLuint CWindow::LinkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint programId = glCreateProgram(); // create a program

    if (programId == 0) {
        std::cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(programId);

    GLint linkStatus;

    // Get the link status for this program
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus) { // If the linking failed
        std::cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}

