#include "MyEntity.h"
using namespace Simplex;
//  MyEntity
void MyEntity::Init(void)
{

}
void MyEntity::Swap(MyEntity& other)
{
	
}
void MyEntity::Release(void)
{
	
}
//The big 3
MyEntity::MyEntity()
{
	Init();
	m_pMeshMngr = MyMeshManager::GetInstance(); // singleton
}
MyEntity::MyEntity(MyEntity const& other)
{
	
}
MyEntity& MyEntity::operator=(MyEntity const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyEntity temp(other);
		Swap(temp);
	}
	return *this;
}
MyEntity::~MyEntity(){Release();};

void MyEntity::GenerateTorus(void)
{
	m_nMeshIndex = m_pMeshMngr->GenerateTorus(1.0f, 0.7f, 12, 12, C_YELLOW);
	GenerateRigidBody();
}

void MyEntity::AddToRenderList(void)
{
	m_pMeshMngr->AddConeToRenderList(m_m4ModelToWorld);
	m_pMeshMngr->AddMeshToRenderList(m_nMeshIndex, IDENTITY_M4);
}

void MyEntity::SetModelMatrix(matrix4 a_M4ToWorld)
{
	m_m4ModelToWorld = a_M4ToWorld;
}

void MyEntity::GenerateRigidBody(void)
{
	MyMesh* pMesh = m_pMeshMngr->GetMesh(m_nMeshIndex);
	m_pRigidBody = new MyRigidBody(pMesh->GetVertexList());
}
//Accessors

//--- Non Standard Singleton Methods

