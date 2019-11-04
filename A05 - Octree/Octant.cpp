#include "Octant.h"
using namespace Simplex;

Simplex::Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
}

Simplex::Octant::Octant(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
}

Simplex::Octant::Octant(Octant const& other)
{
	m_uOctantCount = other.m_uOctantCount;
	m_uMaxLevel = other.m_uMaxLevel;
	m_uIdealEntityCount = other.m_uIdealEntityCount;

	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityMngr = other.m_pEntityMngr;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pParent = other.m_pParent;
	//m_pChild = other.m_pChild; // *** giving error ************************

	m_EntityList = other.m_EntityList;

	m_pRoot = other.m_pRoot;
	m_lChild, other.m_lChild;
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

Simplex::Octant::~Octant(void) { Release(); }

void Simplex::Octant::Release(void)
{
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;

	if (m_pChild)
	{
		delete[] m_pChild;
		//m_pChild = nullptr; // same error as above
	}
	SafeDelete(m_pParent);
	SafeDelete(m_pRoot);
}

void Simplex::Octant::Init(void)
{
	m_uID = 0;
	m_uLevel = 0;
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pParent = nullptr;
	//m_pChild = nullptr; // ??? ****************************************
	m_pRoot = nullptr;
}

void Simplex::Octant::Swap(Octant& other)
{
	std::swap(m_uOctantCount, other.m_uOctantCount);
	std::swap(m_uMaxLevel, other.m_uMaxLevel);
	std::swap(m_uIdealEntityCount, other.m_uIdealEntityCount);

	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);

	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_pEntityMngr, other.m_pEntityMngr);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	std::swap(m_pParent, other.m_pParent);
	std::swap(m_pChild, other.m_pChild);

	std::swap(m_EntityList, other.m_EntityList);

	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
}

float Simplex::Octant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::Octant::GetCenterGlobal(void)
{
	return m_v3Center; // *** this and the two methods below may need to be applied to global space, unknown if they already are ***
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
}

void Simplex::Octant::Display(vector3 a_v3Color)
{
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
