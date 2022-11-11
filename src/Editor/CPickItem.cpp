// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CWindow.h"
#include "CPickItem.h"

std::vector<CPickItem::stIntersect> CPickItem::g_ClickedObject;

CPickItem g_PickItem;

CPickItem::CPickItem() : CEvent()
{
    m_LastEditorType = -1;
}

void CPickItem::ProcessMouseDragEvent(GLFWwindow* window, float xoffset, float yoffset)
{
    if (g_ClickedObject.size() > 0)
    {
        xoffset *= 0.01f;
        yoffset *= 0.01f;

        if (CUtil::m_EditorType == 3)
        { // Move Objects.
            for (stIntersect& it : g_ClickedObject)
            {
                glm::vec3* pos = it.m_Model->GetPosition();

                pos->x += xoffset;
                pos->y += yoffset;
            }
        }
        else
        {
            for (stIntersect& it : g_ClickedObject)
            {
                it.m_Vertex->Position.x += xoffset;
                it.m_Vertex->Position.y += yoffset;
                it.m_Mesh->AllocBuffer();
            }
        }
    }

}

void CPickItem::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            if (m_LastEditorType == -1)
                m_LastEditorType = CUtil::m_EditorType;

            if (m_LastEditorType != CUtil::m_EditorType)
            {
                g_ClickedObject.clear();
                m_LastEditorType = CUtil::m_EditorType;
            }

            stIntersect in;

            if (CUtil::m_EditorType == 0)
            { // Move Vertices and Faces.
                if (IntersectSphere(in))
                    g_ClickedObject.push_back(in);
                else if (IntersectSurface(in))
                {
                    Vertex* A = &in.m_Mesh->m_Vertex[in.m_Indices[0]];
                    Vertex* B = &in.m_Mesh->m_Vertex[in.m_Indices[1]];
                    Vertex* C = &in.m_Mesh->m_Vertex[in.m_Indices[2]];

                    auto addVertexInClickedObj = [](stIntersect& in, Vertex* v) {
                        bool finded = false;

                        for (const stIntersect& it : g_ClickedObject)
                        {
                            if (v == it.m_Vertex)
                            {
                                finded = true;
                                break;
                            }
                        }

                        if (!finded)
                        {
                            in.m_Vertex = v;
                            g_ClickedObject.push_back(in);
                        }
                    };

                    addVertexInClickedObj(in, A);
                    addVertexInClickedObj(in, B);
                    addVertexInClickedObj(in, C);
                }
                else
                {
                    g_ClickedObject.clear();

                    if (IntersectSphere(in, false) || IntersectSurface(in, false))
                        CModel::g_SelectedModel = in.m_Model;
                }
            }
            else if (CUtil::m_EditorType == 1)
            { // Create Vertices.
                if (IntersectSurface(in))
                    CreateVertice(in);
                else if (IntersectSurface(in, false))
                    CModel::g_SelectedModel = in.m_Model;
            }
            else if (CUtil::m_EditorType == 2)
            { // Remove Vertices.
                if (IntersectSphere(in))
                {
                    const auto itr = std::find(in.m_Mesh->m_Vertex.begin(), in.m_Mesh->m_Vertex.end(), *in.m_Vertex);
                    int vertexId = std::distance(in.m_Mesh->m_Vertex.begin(), itr);

                    std::vector<unsigned int> newIndices;

                    for (unsigned int id = 0; id < in.m_Mesh->m_Indices.size(); id += 3)
                    {
                        if (in.m_Mesh->m_Indices[id] != vertexId && in.m_Mesh->m_Indices[id + 1] != vertexId && in.m_Mesh->m_Indices[id + 2] != vertexId)
                        {
                            newIndices.push_back(in.m_Mesh->m_Indices[id]);
                            newIndices.push_back(in.m_Mesh->m_Indices[id + 1]);
                            newIndices.push_back(in.m_Mesh->m_Indices[id + 2]);
                        }
                    }

                    in.m_Mesh->m_Indices = newIndices;
                    in.m_Mesh->AllocBuffer();
                }
                else if (IntersectSurface(in, false))
                    CModel::g_SelectedModel = in.m_Model;
            }
            else if (CUtil::m_EditorType == 3)
            { // Move Objects.
                g_ClickedObject.clear();

                if (IntersectSphere(in) || IntersectSurface(in))
                    g_ClickedObject.push_back(in);
                else if (IntersectSurface(in, false))
                    CModel::g_SelectedModel = in.m_Model;
            }
            else if (CUtil::m_EditorType == 4)
            { // Criar Curva.
                if (IntersectSurface(in))
                {
                    stIntersect in2 = in;
                    bool finded = false;

                    const glm::vec3& A = in.m_Mesh->m_Vertex[in.m_Indices[0]].Position;
                    const glm::vec3& B = in.m_Mesh->m_Vertex[in.m_Indices[1]].Position;
                    const glm::vec3& C = in.m_Mesh->m_Vertex[in.m_Indices[2]].Position;

                    for (unsigned int id = 0; id < in.m_Mesh->m_Indices.size(); id += 3)
                    {
                        const glm::vec3& A2 = in.m_Mesh->m_Vertex[in.m_Mesh->m_Indices[id]].Position;
                        const glm::vec3& B2 = in.m_Mesh->m_Vertex[in.m_Mesh->m_Indices[id + 1]].Position;
                        const glm::vec3& C2 = in.m_Mesh->m_Vertex[in.m_Mesh->m_Indices[id + 2]].Position;

                        auto checkPosition = [&in](const glm::vec3& v) {
                            for (int i = 0; i < 3; ++i)
                                if (in.m_Mesh->m_Vertex[in.m_Mesh->m_Indices[i]].Position == v)
                                    return true;

                            return false;
                        };

                        if ((checkPosition(A) && checkPosition(B)) ^ (checkPosition(A) && checkPosition(C)) ^ (checkPosition(B) && checkPosition(C)))
                        {
                            in2.m_Indices = &in.m_Mesh->m_Indices[id];

                            finded = true;
                            break;
                        }
                    }

                    printf("%d %d %d - ", in.m_Indices[0], in.m_Indices[1], in.m_Indices[2]);
                    printf("%d %d %d\n", in2.m_Indices[0], in2.m_Indices[1], in2.m_Indices[2]);

                    if (finded)
                    {
                        CreateVertice(in);
                        CreateVertice(in2);
                    }
                }
                else if (IntersectSurface(in, false))
                    CModel::g_SelectedModel = in.m_Model;
            }
        }
        else if (action == GLFW_RELEASE)
        {
        }
    }
}

void CPickItem::CreateVertice(stIntersect& in)
{
    const Vertex& A = in.m_Mesh->m_Vertex[in.m_Indices[0]];
    const Vertex& B = in.m_Mesh->m_Vertex[in.m_Indices[1]];
    const Vertex& C = in.m_Mesh->m_Vertex[in.m_Indices[2]];

    Vertex centerOfTriangle; centerOfTriangle.Clear();
    centerOfTriangle.Position = (glm::vec3((A.Position + B.Position + C.Position) / 3.f));
    centerOfTriangle.TexCoords = (glm::vec2((A.TexCoords + B.TexCoords + C.TexCoords) / 3.f));

    int vertexId = in.m_Mesh->m_Vertex.size();
    in.m_Mesh->m_Vertex.push_back(centerOfTriangle);

    unsigned int prevA = in.m_Indices[0];
    unsigned int prevB = in.m_Indices[1];
    unsigned int prevC = in.m_Indices[2];

    // A, B, G
    in.m_Indices[2] = vertexId;

    // A, G, C
    in.m_Mesh->m_Indices.push_back(prevA);
    in.m_Mesh->m_Indices.push_back(vertexId);
    in.m_Mesh->m_Indices.push_back(prevC);

    // B, C, G
    in.m_Mesh->m_Indices.push_back(prevB);
    in.m_Mesh->m_Indices.push_back(prevC);
    in.m_Mesh->m_Indices.push_back(vertexId);

    in.m_Mesh->AllocBuffer();
}

bool CPickItem::IntersectSphere(stIntersect& out, bool onlySelected)
{
    bool finded = false;

    double xpos, ypos;
    glfwGetCursorPos(g_Window, &xpos, &ypos);

    glm::vec2 fC(xpos, ypos);
    glm::vec2 ndc = ((glm::vec2(fC.x / static_cast<float>(g_WindowMaxX), 1.0 - fC.y / static_cast<float>(g_WindowMaxY)) * 2.0f) - 1.0f);

    for (CModel* it : CWindow::GetModels())
    {
        if (onlySelected)
            it = CModel::g_SelectedModel;
        else if (it == CModel::g_SelectedModel)
            continue;

        glm::mat4 mvp = (CWindow::GetVP() * it->GetModelPos());
        glm::mat4 worldPosition = glm::inverse(mvp);

        glm::vec4 from = (worldPosition * glm::vec4(ndc, -1.0f, 1.0f));
        glm::vec4 to = (worldPosition * glm::vec4(ndc, 1.0f, 1.0f));

        from /= from.w; //perspective divide ("normalize" homogeneous coordinates)
        to /= to.w;

        glm::vec3 direction = (glm::vec3(to) - glm::vec3(from));
        float minDistOfSphere = 9999999.f;

        for (CMesh& mesh : it->m_Meshes)
        {
            for (Vertex& v : mesh.m_Vertex)
            {
                float t1, t2;

                if (CUtil::IntersectSphere((glm::vec3(from) - v.Position), direction, 0.10f, t1, t2))
                {
                    if (t1 < minDistOfSphere)
                    {
                        out.m_Model = it;
                        out.m_Mesh = &mesh;
                        out.m_Vertex = &v;
                        out.m_Indices = NULL;

                        finded = true;
                        minDistOfSphere = t1;
                    }
                }
            }
        }

        if (onlySelected)
            break;
    }

    return finded;
}

bool CPickItem::IntersectSurface(stIntersect& out, bool onlySelected)
{
    bool finded = false;

    double xpos, ypos;
    glfwGetCursorPos(g_Window, &xpos, &ypos);

    glm::vec2 fC(xpos, ypos);
    glm::vec2 ndc = ((glm::vec2(fC.x / static_cast<float>(g_WindowMaxX), 1.0 - fC.y / static_cast<float>(g_WindowMaxY)) * 2.0f) - 1.0f);

    glm::vec3 outIntersectionPoint;
    int minDistOfSurface = 9999999;

    for (CModel* it : CWindow::GetModels())
    {
        if (onlySelected)
            it = CModel::g_SelectedModel;
        else if (it == CModel::g_SelectedModel)
            continue;

        glm::mat4 mvp = (CWindow::GetVP() * it->GetModelPos());
        glm::mat4 worldPosition = glm::inverse(mvp);

        glm::vec4 from = (worldPosition * glm::vec4(ndc, -1.0f, 1.0f));
        glm::vec4 to = (worldPosition * glm::vec4(ndc, 1.0f, 1.0f));

        from /= from.w; //perspective divide ("normalize" homogeneous coordinates)
        to /= to.w;

        glm::vec3 direction = (glm::vec3(to) - glm::vec3(from));

        for (CMesh& itmesh : it->m_Meshes)
        {
            for (unsigned int id = 0; id < itmesh.m_Indices.size(); id += 3)
            {
                glm::vec3 triangule[3];
                triangule[0] = itmesh.m_Vertex[itmesh.m_Indices[id]].Position;
                triangule[1] = itmesh.m_Vertex[itmesh.m_Indices[id + 1]].Position;
                triangule[2] = itmesh.m_Vertex[itmesh.m_Indices[id + 2]].Position;

                if (CUtil::RayIntersectsTriangle(glm::vec3(from), glm::vec3(to), triangule, outIntersectionPoint))
                {
                    glm::vec3 diff = (glm::vec3(from) - outIntersectionPoint);

                    int dist = (int)sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                    if (dist < minDistOfSurface)
                    {
                        out.m_Model = it;
                        out.m_Mesh = &itmesh;
                        out.m_Vertex = NULL;
                        out.m_Indices = &itmesh.m_Indices[id];

                        finded = true;
                        minDistOfSurface = dist;
                    }
                }
            }
        }

        if (onlySelected)
            break;
    }

    return finded;
}

