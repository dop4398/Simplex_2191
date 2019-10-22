/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/06
----------------------------------------------*/
#ifndef __MYENTITY_H_
#define __MYENTITY_H_

#include "Simplex\Simplex.h"
#include "MyRigidBody.h"
#include "MyMeshManager.h"


// creating a new class that creates both my mesh and the rigidbody for my mesh.
// we craeted the basic variables that we needed. We used a meshmanager rather than a mesh because the meshmngr knows everything in the world that it needs.

namespace Simplex
{
//System Class
class MyEntity
{
	MyMeshManager* m_pMeshMngr = nullptr;
	MyRigidBody* m_pRigidBody = nullptr;

	uint m_nMeshIndex = -1; // Index associated with this mesh
	matrix4 m_m4ModelToWorld;


public:
	/*
	Usage: Constructor
	Arguments: ---
	Output: class object instance
	*/
	MyEntity(void);
	/*
	Usage: Copy Constructor
	Arguments: class object to copy
	Output: class object instance
	*/
	MyEntity(MyEntity const& other);
	/*
	Usage: Copy Assignment Operator
	Arguments: class object to copy
	Output: ---
	*/
	MyEntity& operator=(MyEntity const& other);
	/*
	Usage: Destructor
	Arguments: ---
	Output: ---
	*/
	~MyEntity(void);

	/*
	Usage: Changes object contents for other object's
	Arguments: other -> object to swap content from
	Output: ---
	*/
	void Swap(MyEntity& other);

	void GenerateTorus(void);
	void AddToRenderList(void);
	void SetModelMatrix(matrix4 a_M4ToWorld);
	void GenerateRigidBody(void);

private:
	/*
	Usage: Deallocates member fields
	Arguments: ---
	Output: ---
	*/
	void Release(void);
	/*
	Usage: Allocates member fields
	Arguments: ---
	Output: ---
	*/
	void Init(void);


};//class

} //namespace Simplex

#endif //__MYENTITY_H__

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/