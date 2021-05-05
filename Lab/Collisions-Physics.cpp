

#include "Collisions-Physics.h"


btDiscreteDynamicsWorld* handler::initilize()
{

	//Config contains memory and collision setup
	btDefaultCollisionConfiguration* collisionconfig = new btDefaultCollisionConfiguration();

	//Collision Dispatcher option for multithreading look at later.
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionconfig);

	//Other options available will have to read benfits leave for now.
	btBroadphaseInterface* overlapping = new btDbvtBroadphase();

	//Constraint Solver Multithreading option available look at later
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynWorld = new btDiscreteDynamicsWorld(dispatcher, overlapping, solver, collisionconfig);


	return dynWorld;




}