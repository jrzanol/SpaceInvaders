// by jrzanol
//

#pragma once
#include "CEvent.h"

class CPickItem : CEvent
{
public:
	CPickItem();

	void ProcessMouseDragEvent(GLFWwindow*, float, float);
	void ProcessMouseButtonEvent(GLFWwindow*, int, int, int);

private:
	int m_LastEditorType;

	struct stIntersect
	{
		CModel* m_Model;
		CMesh* m_Mesh;

		Vertex* m_Vertex;
		unsigned int* m_Indices;
	};

	void CreateVertice(stIntersect&);

	bool IntersectSphere(stIntersect&, bool = true);
	bool IntersectSurface(stIntersect&, bool = true);

public:
	static std::vector<stIntersect> g_ClickedObject;
};

