// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

GLFWwindow* g_Window = NULL;

float g_DeltaTime = 0;
float g_LastTime = 0;

int CUtil::m_EditorType = 5;

const char* CUtil::m_VertexShader = R"glsl(
    #version 430

    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec3 a_normals;
    layout(location = 2) in vec2 a_texcoords;
    layout(location = 3) in vec3 a_tangent;
    layout(location = 4) in vec3 a_bitangent;
    
    out vec2 TexCoords;
    out vec3 FragPos;  
    out vec3 Normal;
    
    uniform mat4 u_model;
    uniform mat4 u_vp;
    
    void main() {
        TexCoords = a_texcoords;
        FragPos = vec3(u_model * vec4(a_position, 1.0));
        Normal = a_normals;

        gl_Position = (u_vp * u_model * vec4(a_position, 1.0));
    }
)glsl";

const char* CUtil::m_FragmentShader = R"glsl(
    #version 430
    
    out vec4 FragColor;
    in vec2 TexCoords;
    in vec3 FragPos;
    in vec3 Normal;

    uniform sampler2D texture_diffuse1;
    uniform sampler2D texture_specular1;
    uniform sampler2D texture_normal1;
    uniform int u_wireframe;
    uniform int u_wireframeColor;
    uniform float u_textcoord;

    uniform vec3 lightPos;
    uniform vec3 lightPos2;
    uniform vec3 lightPos3;
    uniform vec3 lightColor;
    uniform vec3 lightColor2;
    uniform vec3 lightColor3;
    
    void main() {
        if (u_wireframe != 0) {
            if (u_wireframeColor == 0)
                FragColor = vec4(1, 0, 0, 0);
            else
                FragColor = vec4(0, 0, 1, 0);
        }
        else {
            // Object Color:
            vec2 newTexCoords;
            newTexCoords.x = (TexCoords.x + u_textcoord);
            newTexCoords.y = (TexCoords.y + u_textcoord);
            
            vec4 objectColor = mix(texture(texture_diffuse1, newTexCoords), texture(texture_specular1, newTexCoords), texture(texture_normal1, newTexCoords));
            
            // Light:
            vec3 norm = normalize(Normal);
            vec3 diffuse = max(dot(norm, normalize(lightPos - FragPos)), 0.0) * lightColor;
            vec3 diffuse2 = max(dot(norm, normalize(lightPos2 - FragPos)), 0.0) * lightColor2;
            vec3 diffuse3 = max(dot(norm, normalize(lightPos3 - FragPos)), 0.0) * lightColor3;
            
            float ambientStrength = 0.65f;
            vec3 ambient = ambientStrength * lightColor;
            
            // Ambient Color:
            FragColor = vec4(ambient + diffuse * 2 + diffuse2 + diffuse3, 1) * objectColor;
        }
    }
)glsl";

//https://stackoverflow.com/questions/20140711/picking-in-3d-with-ray-tracing-using-ninevehgl-or-opengl-i-phone
//ray at position p with direction d intersects sphere at (0,0,0) with radius r. returns intersection times along ray t1 and t2
bool CUtil::IntersectSphere(const glm::vec3& p, const glm::vec3& d, float r, float& t1, float& t2)
{
    float A = glm::dot(d, d);
    float B = 2.0f * glm::dot(d, p);
    float C = glm::dot(p, p) - r * r;
    float dis = B * B - 4.0f * A * C;

    if (dis < 0.0f)
        return false;

    float S = sqrt(dis);

    t1 = (-B - S) / (2.0f * A);
    t2 = (-B + S) / (2.0f * A);
    return true;
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool CUtil::RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayVector, glm::vec3* inTriangle, glm::vec3& outIntersectionPoint)
{
    const float EPSILON = 0.0000001f;

    glm::vec3 vertex0 = inTriangle[0];
    glm::vec3 vertex1 = inTriangle[1];
    glm::vec3 vertex2 = inTriangle[2];
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;

    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;

    h = glm::cross(rayVector, edge2);
    a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.

    f = 1.0f / a;
    s = rayOrigin - vertex0;
    u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f)
        return false;

    q = glm::cross(s, edge1);
    v = f * glm::dot(rayVector, q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) // ray intersection
    {
        outIntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}
