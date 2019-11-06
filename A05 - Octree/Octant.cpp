#include "Octant.h"
using namespace Simplex;

uint Octant::m_uOctantCount = 0;
uint Octant::m_uMaxLevel = 3;
uint Octant::m_uIdealEntityCount = 5;

Simplex::Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = 0; //
	// This is the root
	m_pRoot = this;
	m_lChild.clear();
	// Min and Max vectors stored here
	std::vector<vector3> lMinMax;

	//nt nObjects = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
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
	for (int i = 0; i < 3; i++)
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
	ConstructTree(m_uMaxLevel);
}

Simplex::Octant::Octant(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
	m_uOctantCount++;
	ConstructTree(m_uMaxLevel);
}

Simplex::Octant::Octant(Octant const& other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pRoot = other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pParent = other.m_pParent;
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

Octant& Simplex::Octant::operator=(Octant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Octant temp(other);
		Swap(temp);
	}
	return *this;
}

Simplex::Octant::~Octant(void) { Release(); }//;

void Simplex::Octant::Release(void)
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

void Simplex::Octant::Init(void)
{
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pParent = nullptr;
	m_pRoot = nullptr;
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
}

void Simplex::Octant::Swap(Octant& other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

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

	//std::swap(m_pChild, other.m_pChild);
	//std::swap(m_EntityList, other.m_EntityList);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
}

float Simplex::Octant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::Octant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::Octant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::Octant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::Octant::IsColliding(uint a_uRBIndex)
{
	return false;
}

void Simplex::Octant::Display(uint a_nIndex, vector3 a_v3Color)
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

void Simplex::Octant::Display(vector3 a_v3Color)
{
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void Simplex::Octant::DisplayLeafs(vector3 a_v3Color)
{
	// Displays the non-empty leafs in the octree
	// Check each leaf - see if it contains any entities
	// Draw it if it does
}

void Simplex::Octant::ClearEntityList(void)
{
}

void Simplex::Octant::Subdivide(void)
{
	// Return if we're at the maximum depth
	if (m_uLevel >= m_uMaxLevel)
		return;

	// also return if we've already subdivided
	if (m_uChildren != 0)
		return;

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
	m_pChild[0] = new Octant(v3Center, fSizeDouble);

	// Bottom right back
	v3Center.x += fSizeDouble;
	m_pChild[1] = new Octant(v3Center, fSizeDouble);

	// Bottom right front
	v3Center.z += fSizeDouble;
	m_pChild[2] = new Octant(v3Center, fSizeDouble);

	// Bottom left front
	v3Center.x -= fSizeDouble;
	m_pChild[3] = new Octant(v3Center, fSizeDouble);

	// Top left front
	v3Center.y += fSizeDouble;
	m_pChild[4] = new Octant(v3Center, fSizeDouble);

	// Top left back
	v3Center.z -= fSizeDouble;
	m_pChild[5] = new Octant(v3Center, fSizeDouble);

	// Top right back
	v3Center.x += fSizeDouble;
	m_pChild[6] = new Octant(v3Center, fSizeDouble);

	// Top right front
	v3Center.z += fSizeDouble;
	m_pChild[7] = new Octant(v3Center, fSizeDouble);

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		// If an octant contains more than the ideal # of entities, subdivide it.
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
			m_pChild[i]->Subdivide(); // Recursion baby!
	}
}

Octant* Simplex::Octant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

Octant* Simplex::Octant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::Octant::IsLeaf(void)
{
	if (m_uChildren == 0)
		return true;
	return false;
}

bool Simplex::Octant::ContainsMoreThan(uint a_nEntities)
{
	if(m_EntityList.size() >= a_nEntities) // contains **at least** this many entities
		return true;
	return false;
}

void Simplex::Octant::KillBranches(void)
{
}

void Simplex::Octant::ConstructTree(uint a_nMaxLevel)
{
}

void Simplex::Octant::AssignIDtoEntity(void)
{
}

uint Simplex::Octant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void Simplex::Octant::ConstructList(void)
{
}
