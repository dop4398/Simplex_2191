#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;
	// This is the root
	m_pRoot = this;
	m_lChild.clear();
	// Min and Max vectors stored here
	std::vector<vector3> lMinMax;

	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjects; i++)
	{
		// Get the min and max of each entity for lMinMax
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}

	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);
	vector3 vHalfWidth = pRigidBody->GetHalfWidth();
	float fMax = vHalfWidth.x;
	for (int i = 1; i < 3; i++)
	{
		if (fMax < vHalfWidth[i])
			fMax = vHalfWidth[i];
	}
	vector3 v3Center = pRigidBody->GetCenterLocal();
	// clear the list and delete pRigidBody since we're done with it
	lMinMax.clear();
	SafeDelete(pRigidBody);
	// Set up the last few variables
	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax)); // since this is a cube, we can use fMax for x, y, and z
	m_v3Max = m_v3Center + (vector3(fMax));

	m_uOctantCount++;
	std::cout << "Just before ConstructTree(m_uMaxLevel) " << m_uMaxLevel << std::endl;
	ConstructTree(m_uMaxLevel);
	std::cout << "End of constructor" << std::endl;
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
	m_uOctantCount++;
	ConstructTree(m_uMaxLevel);
}

MyOctant::MyOctant(MyOctant const& other)
{
	m_uChildren = other.m_uChildren;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;

	m_pParent = other.m_pParent;
	m_pRoot = other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (int i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

MyOctant::~MyOctant(void) { Release(); };

void MyOctant::Release(void)
{
	if (m_uLevel == 0)
		KillBranches();
	// children? not here
	m_fSize = 0.0f;
	m_uChildren = 0;
	// clear them lists
	m_EntityList.clear();
	m_lChild.clear();
}

void MyOctant::Init(void)
{
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_fSize, other.m_fSize);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	std::swap(m_pParent, other.m_pParent);
	for (int i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

float MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool MyOctant::IsColliding(uint a_uRBIndex)
{
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	// no collision if the given index is greater than the # of entities in our box
	if (a_uRBIndex >= nObjectCount)
		return false;
	// Get the min and max of the potentially colliding entity
	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 v3MinOctant = pRigidBody->GetMinGlobal();
	vector3 v3MaxOctant = pRigidBody->GetMaxGlobal();

	// Check for x
	if (m_v3Max.x < v3MinOctant.x)
		return false;
	if (m_v3Min.x > v3MaxOctant.x)
		return false;

	// Check for y
	if (m_v3Max.y < v3MinOctant.y)
		return false;
	if (m_v3Min.y > v3MaxOctant.y)
		return false;

	// Check for z
	if (m_v3Max.z < v3MinOctant.z)
		return false;
	if (m_v3Min.z > v3MaxOctant.z)
		return false;

	//std::cout << "IsColliding() returns true" << std::endl;
	// If everything passes, then the objects are colliding, so return true
	return true;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	//Displays a specific octant, specified by the index. Includes the objects underneath it (I assume this means its leafs)
	if (m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
			glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}

	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_nIndex);
	}
}

void MyOctant::Display(vector3 a_v3Color)
{
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	// Displays the non-empty leafs in the octree
	// Check each leaf - see if it contains any entities
	// Draw it if it does
	uint nLeafs = m_lChild.size();
	for (uint i = 0; i < nLeafs; i++)
	{
		m_lChild[i]->DisplayLeafs(a_v3Color); // recursion baby!
	}
	// Render
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void MyOctant::ClearEntityList(void)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList(); // even more recursion!
	}
	m_EntityList.clear();
}

void MyOctant::Subdivide(void)
{
	std::cout << "Start of Subdivide()" << std::endl;

	// Return if we're at the maximum depth
	if (m_uLevel >= m_uMaxLevel)
		return;

	// also return if we've already subdivided
	if (m_uChildren != 0)
		return;

	std::cout << "Subdivide() continues" << std::endl;

	m_uChildren = 8;
	float fSize = m_fSize / 4.0f;
	float fSizeDouble = fSize * 2.0f;
	vector3 v3Center;

	// *** Make the new octants ***
	v3Center = m_v3Center;

	// Bottom left back
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;
	m_pChild[0] = new MyOctant(v3Center, fSizeDouble);

	// Bottom right back
	v3Center.x += fSizeDouble;
	m_pChild[1] = new MyOctant(v3Center, fSizeDouble);

	// Bottom right front
	v3Center.z += fSizeDouble;
	m_pChild[2] = new MyOctant(v3Center, fSizeDouble);

	// Bottom left front
	v3Center.x -= fSizeDouble;
	m_pChild[3] = new MyOctant(v3Center, fSizeDouble);

	// Top left front
	v3Center.y += fSizeDouble;
	m_pChild[4] = new MyOctant(v3Center, fSizeDouble);

	// Top left back
	v3Center.z -= fSizeDouble;
	m_pChild[5] = new MyOctant(v3Center, fSizeDouble);

	// Top right back
	v3Center.x += fSizeDouble;
	m_pChild[6] = new MyOctant(v3Center, fSizeDouble);

	// Top right front
	v3Center.z += fSizeDouble;
	m_pChild[7] = new MyOctant(v3Center, fSizeDouble);

	std::cout << "Subdivide() after creating new MyOctants" << std::endl;

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		// If an octant contains more than the ideal # of entities, subdivide it.
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			std::cout << "Subdivide() recursive step" << std::endl;
			m_pChild[i]->Subdivide(); // Recursion baby!
		}	
	}
	std::cout << "End of Subdivide()" << std::endl;
}

MyOctant* MyOctant::GetChild(uint a_nChild)
{
	// Return nothing if the index is bad
	if (a_nChild > 7)
		return nullptr;
	return m_pChild[a_nChild];
}

MyOctant* MyOctant::GetParent(void)
{
	return m_pParent;
}

bool MyOctant::IsLeaf(void)
{
	if (m_uChildren == 0)
		return true;
	return false;
}

bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	uint nCount = 0;
	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjects; i++)
	{
		if (IsColliding(i))
			nCount++;
		if (nCount > a_nEntities)
		{
			std::cout << "ContainsMoreThan() returns true" << std::endl;
			return true;
		}
			
	}
	return false;
}

void MyOctant::KillBranches(void)
{
	// Recursion here
	// Essentially goes through the tree until it hits a node with no children.
	// Then it goes back to that node's parent and kills the children.
	// Does this until the node that called the method is reached.
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranches();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}
	m_uChildren = 0;
}

void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	std::cout << "Start of ConstructTree(" << a_nMaxLevel << ")" << std::endl;
	// Should only apply to the root
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;

	// Clear the tree
	//m_EntityList.clear();
	ClearEntityList();
	KillBranches();
	m_lChild.clear();

	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		std::cout << "Just before Subdivide" << std::endl;
		Subdivide();
		std::cout << "Just after Subdivide" << std::endl;

	}
		

	AssignIDtoEntity(); // Add those IDs
	ConstructList(); // Make the list of objects
	std::cout << "End of ConstructTree()" << std::endl;
}

void MyOctant::AssignIDtoEntity(void)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}
	if (m_uChildren == 0) // If a leaf...
	{
		uint nEntities = m_pEntityMngr->GetEntityCount();
		for (uint i = 0; i < nEntities; i++)
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}

uint MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void MyOctant::ConstructList(void)
{
	std::cout << "Start of ConstructList()" << std::endl;

	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList(); // recursion
	}

	if (m_EntityList.size() > 0)
		m_pRoot->m_lChild.push_back(this);

	std::cout << "End of ConstructList()" << std::endl;
}
// found the exact line where the `read access violation` exception is thrown for me, it's happening when i call `m_pRoot->m_lChild.push_back(this);`