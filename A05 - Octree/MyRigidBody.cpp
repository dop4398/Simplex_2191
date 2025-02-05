#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3CenterL = ZERO_V3;
	m_v3CenterG = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;

	m_nCollidingCount = 0;
	m_CollidingArray = nullptr;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bVisibleBS, other.m_bVisibleBS);
	std::swap(m_bVisibleOBB, other.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, other.m_bVisibleARBB);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3ColorColliding, other.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, other.m_v3ColorNotColliding);

	std::swap(m_v3CenterL, other.m_v3CenterL);
	std::swap(m_v3CenterG, other.m_v3CenterG);
	std::swap(m_v3MinL, other.m_v3MinL);
	std::swap(m_v3MaxL, other.m_v3MaxL);

	std::swap(m_v3MinG, other.m_v3MinG);
	std::swap(m_v3MaxG, other.m_v3MaxG);

	std::swap(m_v3HalfWidth, other.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, other.m_v3ARBBSize);

	std::swap(m_m4ToWorld, other.m_m4ToWorld);

	std::swap(m_nCollidingCount, other.m_nCollidingCount);
	std::swap(m_CollidingArray, other.m_CollidingArray);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3CenterL; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){ return m_v3CenterG; }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	m_v3CenterG = vector3(m_m4ToWorld * vector4(m_v3CenterL, 1.0f));

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3CenterL = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3CenterL, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = other.m_pMeshMngr;

	m_bVisibleBS = other.m_bVisibleBS;
	m_bVisibleOBB = other.m_bVisibleOBB;
	m_bVisibleARBB = other.m_bVisibleARBB;

	m_fRadius = other.m_fRadius;

	m_v3ColorColliding = other.m_v3ColorColliding;
	m_v3ColorNotColliding = other.m_v3ColorNotColliding;

	m_v3CenterL = other.m_v3CenterL;
	m_v3CenterG = other.m_v3CenterG;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;
	m_v3ARBBSize = other.m_v3ARBBSize;

	m_m4ToWorld = other.m_m4ToWorld;

	m_nCollidingCount = other.m_nCollidingCount;
	m_CollidingArray = other.m_CollidingArray;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- other Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* other)
{
	//if its already in the list return
	if (IsInCollidingArray(other))
		return;
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/

	//insert the entry
	PRigidBody* pTemp;
	pTemp = new PRigidBody[m_nCollidingCount + 1];
	if (m_CollidingArray)
	{
		memcpy(pTemp, m_CollidingArray, sizeof(MyRigidBody*) * m_nCollidingCount);
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
	pTemp[m_nCollidingCount] = other;
	m_CollidingArray = pTemp;

	++m_nCollidingCount;
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* other)
{
	//if there are no dimensions return
	if (m_nCollidingCount == 0)
		return;

	//we look one by one if its the one wanted
	for (uint i = 0; i < m_nCollidingCount; i++)
	{
		if (m_CollidingArray[i] == other)
		{
			//if it is, then we swap it with the last one and then we pop
			std::swap(m_CollidingArray[i], m_CollidingArray[m_nCollidingCount - 1]);
			PRigidBody* pTemp;
			pTemp = new PRigidBody[m_nCollidingCount - 1];
			if (m_CollidingArray)
			{
				memcpy(pTemp, m_CollidingArray, sizeof(uint) * (m_nCollidingCount - 1));
				delete[] m_CollidingArray;
				m_CollidingArray = nullptr;
			}
			m_CollidingArray = pTemp;

			--m_nCollidingCount;
			return;
		}
	}
}
void MyRigidBody::ClearCollidingList(void)
{
	m_nCollidingCount = 0;
	if (m_CollidingArray)
	{
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
}
uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	/*
	ra = this.GetRadius()
	rb = a_pOther.GetRadius()
	u[3] = local x-, y-, and z- axes (vector3[3])
	a = this
	b = a_pOther
	c = GetCenterGlobal
	e = positive GetHalfWidth() extents of OBB along each axis
	*/

	matrix3 R, AbsR;
	float ra, rb;
	// Set up 'u's and 'e's ******************************************************************************************************
	vector3 uA[3] = {
		vector3(this->GetModelMatrix()[0]),
		vector3(this->GetModelMatrix()[1]),
		vector3(this->GetModelMatrix()[2])
	};
	vector3 uB[3] = {
		vector3(a_pOther->GetModelMatrix()[0]),
		vector3(a_pOther->GetModelMatrix()[1]),
		vector3(a_pOther->GetModelMatrix()[2])
	};
	float eA[3] = {
		this->GetHalfWidth().x,
		this->GetHalfWidth().y,
		this->GetHalfWidth().z
	};
	float eB[3] = {
		a_pOther->GetHalfWidth().x,
		a_pOther->GetHalfWidth().y,
		a_pOther->GetHalfWidth().z
	};

	// Compute the rotation matrix expressing a_pOther in this MyRigidBody's coordinate frame
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = glm::dot(uA[i], uB[j]);
		}
	}
	// Compute the translation vector t
	vector3 t = a_pOther->GetCenterGlobal() - this->GetCenterGlobal();
	// Begin the translation into a's coordinate frame
	t = vector3(glm::dot(t, uA[0]), glm::dot(t, uA[1]), glm::dot(t, uA[2]));

	// Compute common subexpressions. Add in an epsilon term to counteract arithmetic errors when two edges ar eparallel and their cross product is (near) null
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AbsR[i][j] = glm::abs(R[i][j]) + FLT_EPSILON;
		}
	}

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		ra = eA[i];
		rb = (eB[0] * AbsR[i][0]) + (eB[1] * AbsR[i][1]) + (eB[2] * AbsR[i][2]);
		if (glm::abs(t[i]) > ra + rb)
		{
			// Different axis depending on i
			switch (i)
			{
			case 0:	// A0
				return eSATResults::SAT_AX;
				break;
			case 1: // A1
				return eSATResults::SAT_AY;
				break;
			default: // A2
				return eSATResults::SAT_AZ;
				break;
			}
		}
	}
	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		rb = eB[i];
		ra = (eA[0] * AbsR[0][i]) + (eA[1] * AbsR[1][i]) + (eA[2] * AbsR[2][i]);
		if (glm::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)
		{
			// Different axis depending on i
			switch (i)
			{
			case 0: // B0
				return eSATResults::SAT_BX;
				break;
			case 1: // B1
				return eSATResults::SAT_BY;
				break;
			default: // B2
				return eSATResults::SAT_BZ;
				break;
			}
		}
	}

	// Test axis L = A0 x B0
	ra = (eA[1] * AbsR[2][0]) + (eA[2] * AbsR[1][0]);
	rb = (eB[1] * AbsR[0][2]) + (eB[2] * AbsR[0][1]);
	if (glm::abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)
		return eSATResults::SAT_AXxBX;
	// Test axis L = A0 x B1
	ra = (eA[1] * AbsR[2][1]) + (eA[2] * AbsR[1][1]);
	rb = (eB[0] * AbsR[0][2]) + (eB[2] * AbsR[0][0]);
	if (glm::abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)
		return eSATResults::SAT_AXxBY;
	// Test axis L = A0 x B2
	ra = (eA[1] * AbsR[2][2]) + (eA[2] * AbsR[1][2]);
	rb = (eB[0] * AbsR[0][1]) + (eB[1] * AbsR[0][0]);
	if (glm::abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)
		return eSATResults::SAT_AXxBZ;

	// Test axis L = A1 x B0
	ra = (eA[0] * AbsR[2][0]) + (eA[2] * AbsR[0][0]);
	rb = (eB[1] * AbsR[1][2]) + (eB[2] * AbsR[1][1]);
	if (glm::abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)
		return eSATResults::SAT_AYxBX;
	// Test axis L = A1 x B1
	ra = (eA[0] * AbsR[2][1]) + (eA[2] * AbsR[0][1]);
	rb = (eB[0] * AbsR[1][2]) + (eB[2] * AbsR[1][0]);
	if (glm::abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)
		return eSATResults::SAT_AYxBY;
	// Test axis L = A1 x B2
	ra = (eA[0] * AbsR[2][2]) + (eA[2] * AbsR[0][2]);
	rb = (eB[0] * AbsR[1][1]) + (eB[1] * AbsR[1][0]);
	if (glm::abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)
		return eSATResults::SAT_AYxBZ;

	// Test axis L = A2 x B0
	ra = (eA[0] * AbsR[1][0]) + (eA[1] * AbsR[0][0]);
	rb = (eB[1] * AbsR[2][2]) + (eB[2] * AbsR[2][1]);
	if (glm::abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)
		return eSATResults::SAT_AZxBX;
	// Test axis L = A2 x B1
	ra = (eA[0] * AbsR[1][1]) + (eA[1] * AbsR[0][1]);
	rb = (eB[0] * AbsR[2][2]) + (eB[2] * AbsR[2][0]);
	if (glm::abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)
		return eSATResults::SAT_AZxBY;
	// Test axis L = A2 x B2
	ra = (eA[0] * AbsR[1][2]) + (eA[1] * AbsR[0][2]);
	rb = (eB[0] * AbsR[2][1]) + (eB[1] * AbsR[2][0]);
	if (glm::abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)
		return eSATResults::SAT_AZxBZ;

	//there is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding
	bool bColliding = true;
	//bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius); // this was commented out
	//if they are check the Axis Aligned Bounding Box
	if (bColliding) //they are colliding with bounding sphere
	{
		if (this->m_v3MaxG.x < a_pOther->m_v3MinG.x) //this to the right of other
			bColliding = false;
		if (this->m_v3MinG.x > a_pOther->m_v3MaxG.x) //this to the left of other
			bColliding = false;

		if (this->m_v3MaxG.y < a_pOther->m_v3MinG.y) //this below of other
			bColliding = false;
		if (this->m_v3MinG.y > a_pOther->m_v3MaxG.y) //this above of other
			bColliding = false;

		if (this->m_v3MaxG.z < a_pOther->m_v3MinG.z) //this behind of other
			bColliding = false;
		if (this->m_v3MinG.z > a_pOther->m_v3MaxG.z) //this in front of other
			bColliding = false;

		if (bColliding) //they are colliding with bounding box also
		{
			this->AddCollisionWith(a_pOther);
			a_pOther->AddCollisionWith(this);
		}
		else //they are not colliding with bounding box
		{
			this->RemoveCollisionWith(a_pOther);
			a_pOther->RemoveCollisionWith(this);
		}
	}
	else //they are not colliding with bounding sphere
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}
	return bColliding;
}

void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_nCollidingCount > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_nCollidingCount > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_nCollidingCount > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}
bool MyRigidBody::IsInCollidingArray(MyRigidBody* a_pEntry)
{
	//see if the entry is in the set
	for (uint i = 0; i < m_nCollidingCount; i++)
	{
		if (m_CollidingArray[i] == a_pEntry)
			return true;
	}
	return false;
}