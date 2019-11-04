#include "Octant.h"
using namespace Simplex;

Simplex::Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
}

Simplex::Octant::Octant(vector3 a_v3Center, float a_fSize)
{
}

Simplex::Octant::Octant(Octant const& other)
{
}

Octant& Simplex::Octant::operator=(Octant const& other)
{
	// TODO: insert return statement here
}

Simplex::Octant::~Octant(void)
{
}

void Simplex::Octant::Swap(Octant& other)
{
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
}

void Simplex::Octant::Display(vector3 a_v3Color)
{
}

void Simplex::Octant::DisplayLeafs(vector3 a_v3Color)
{
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

void Simplex::Octant::Release(void)
{
}

void Simplex::Octant::Init(void)
{
}

void Simplex::Octant::ConstructList(void)
{
}
