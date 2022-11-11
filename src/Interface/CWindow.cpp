// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

glm::mat4 CWindow::m_VP;
std::list<CModel*> CWindow::m_DrawModel;

CWindow::CWindow()
{
    g_Window = NULL;
    m_ProgramId = 0;
    m_PickingProgramId = 0;
}

CWindow::~CWindow()
{
}

const glm::mat4& CWindow::GetVP() { return m_VP; }
const std::list<CModel*>& CWindow::GetModels() { return m_DrawModel; }

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

    // Load Models.
    CreateModel(0, "Model/main.obj");

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
    glClearColor(0.25f, 0.25f, 0.25f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set Transforms.
    glm::mat4 projection = glm::perspective(glm::radians(m_Camera[CCamera::m_CameraId].m_Zoom), (float)g_WindowMaxY / (float)g_WindowMaxX, 0.1f, 100.0f);
    glm::mat4 view = m_Camera[CCamera::m_CameraId].GetViewMatrix();

    m_VP = (projection * view);
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramId, "u_vp"), 1, GL_FALSE, glm::value_ptr(m_VP));

    // Draw Lights.
    for (const auto& it : m_Light)
        it.Draw(m_ProgramId);
    
    // Draw objects.
    for (const auto& it : m_DrawModel)
        it->Draw(m_ProgramId, m_VP);

    // Start the Dear ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create ImGui Sliders.
    ImGui::Begin("Editor");
        ImGui::Text("Modelo:");
        ImGui::RadioButton("Arrastar Vertices/Triangulos", &CUtil::m_EditorType, 0);
        ImGui::RadioButton("Criar Vertices", &CUtil::m_EditorType, 1);
        ImGui::RadioButton("Remover Vertices", &CUtil::m_EditorType, 2);
        ImGui::RadioButton("Mover Objetos", &CUtil::m_EditorType, 3);
        ImGui::RadioButton("Criar Curva", &CUtil::m_EditorType, 4);
        if (ImGui::Button("Criar Modelo #1"))
            CreateModel(0, "Model/main.obj");
        if (ImGui::Button("Criar Modelo #2"))
            CreateModel(1, "Model2/main.obj");
        if (ImGui::Button("Criar Modelo #3"))
            CreateModel(2, "Model3/main.obj");
        if (CModel::g_SelectedModel->GetAnimation())
        {
            if (ImGui::Button("Pausar Animacao"))
                CModel::g_SelectedModel->SetAnimation(false);
        }
        else
        {
            if (ImGui::Button("Ativar Animacao"))
                CModel::g_SelectedModel->SetAnimation();
        }
        static bool s_AllAniAtived = false;
        if (s_AllAniAtived)
        {
            if (ImGui::Button("Desativar Animacoes"))
            {
                for (auto& it : CWindow::GetModels())
                    it->SetAnimation(false);

                s_AllAniAtived = false;
            }
        }
        else
        {
            if (ImGui::Button("Ativar todas Animacoes"))
            {
                for (auto& it : CWindow::GetModels())
                    it->SetAnimation();

                s_AllAniAtived = true;
            }
        }
        ImGui::Separator();
        ImGui::Text("Textura:");
        ImGui::RadioButton("Textura Padrao", &CModel::g_SelectedModel->m_SelectedTexture, 0);
        ImGui::RadioButton("Textura #02", &CModel::g_SelectedModel->m_SelectedTexture, 1);
        ImGui::RadioButton("Textura #03", &CModel::g_SelectedModel->m_SelectedTexture, 2);
        ImGui::Text("Mover a Textura:");
        ImGui::SliderFloat("+", &CModel::g_SelectedModel->m_TextCoord, 0.f, 2.f);
        ImGui::Separator();
        ImGui::Text("Camera:");
        ImGui::RadioButton("Camera Padrao", &CCamera::m_CameraId, 0);
        ImGui::RadioButton("Camera #02", &CCamera::m_CameraId, 1);
        ImGui::RadioButton("Camera #03", &CCamera::m_CameraId, 2);
        static bool s_AllCamAniAtived = false;
        if (s_AllCamAniAtived)
        {
            if (ImGui::Button("Desativar Animacoes [Cam]"))
            {
                for (auto& it : m_Camera)
                    it.SetAnimation(false);

                s_AllCamAniAtived = false;
            }
        }
        else
        {
            if (ImGui::Button("Ativar Animacoes [Cam]"))
            {
                for (auto& it : m_Camera)
                    it.SetAnimation();

                s_AllCamAniAtived = true;
            }
        }
        ImGui::Separator();
        ImGui::Text("Luz:");
        static bool s_AllLightAniAtived = false;
        if (s_AllLightAniAtived)
        {
            if (ImGui::Button("Desativar Animacoes [Light]"))
            {
                for (auto& it : m_Light)
                    it.SetAnimation(false);

                s_AllLightAniAtived = false;
            }
        }
        else
        {
            if (ImGui::Button("Ativar Animacoes [Light]"))
            {
                for (auto& it : m_Light)
                    it.SetAnimation();

                s_AllLightAniAtived = true;
            }
        }
        ImGui::Separator();
        ImGui::Text("Salvar:");
        if (ImGui::Button("Salvar Modelo"))
        {
            CWindow::SaveModel();

            ImGui::SameLine();
            ImGui::Text("Salvo!");
        }
        if (ImGui::Button("Salvar Cena"))
        {
            FILE* out = fopen("Scene.txt", "wt");
            if (out)
            {
                for (const auto& it : m_Camera)
                    fprintf(out, "%s\n", it.ToString());

                for (const auto& it : m_Light)
                    fprintf(out, "%s\n", it.ToString());

                for (const auto& it : m_DrawModel)
                    fprintf(out, "%s\n", it->ToString());

                fclose(out);
            }

            ImGui::SameLine();
            ImGui::Text("Salvo!");
        }
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

void CWindow::CreateModel(int type, const char* fileModel)
{
    static int s_ModelCounter = 0;

    CModel* m = CModel::LoadModel(fileModel);
    glm::vec3* pos = m->GetPosition();

    int xmul = (s_ModelCounter / 7);
    int zmul = (s_ModelCounter % 7);

    *pos = glm::vec3(5.f * xmul, 0.f, -5.f * zmul);

    if (type == 1)
    {
        pos->y = -1.f;
        m->m_Scale = glm::vec3(5.f, 5.f, 5.f);
    }
    else if (type == 2)
    {
        pos->y = 0.75f;
        m->m_Scale = glm::vec3(0.5f, 0.5f, 0.5f);
    }

    s_ModelCounter++;
    m_DrawModel.push_back(m);
}

void CWindow::SaveModel()
{
    CModel* currentModel = CWindow::GetModels().front();

    FILE* out = fopen("Model/main_out.obj", "wt");
    if (out)
    {
        fprintf(out, "# Simple 3D Editor\n");
        fprintf(out, "mtllib Crate1.mtl\n");

        int objId = 1;

        for (CMesh& mesh : currentModel->m_Meshes)
        {
            fprintf(out, "o Object.%03d\n", objId++);

            for (Vertex& v : mesh.m_Vertex)
                fprintf(out, "v %.6f %.6f %.6f\n", v.Position.x, v.Position.y, v.Position.z);

            for (Vertex& v : mesh.m_Vertex)
                fprintf(out, "vt %.6f %.6f\n", v.TexCoords.x, (1.f - v.TexCoords.y));
        }

        fprintf(out, "usemtl Material.001\n");
        fprintf(out, "s off\n");

        for (CMesh& mesh : currentModel->m_Meshes)
            for (size_t i = 0; i < mesh.m_Indices.size(); i += 3)
                fprintf(out, "f %u/%u %u/%u %u/%u\n",
                    mesh.m_Indices[i] + 1, mesh.m_Indices[i] + 1,
                    mesh.m_Indices[i + 1] + 1, mesh.m_Indices[i + 1] + 1,
                    mesh.m_Indices[i + 2] + 1, mesh.m_Indices[i + 2] + 1);

        fclose(out);
    }
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

