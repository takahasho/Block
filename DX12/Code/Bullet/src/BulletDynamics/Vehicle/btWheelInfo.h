/*
 * Copyright (c) 2005 Erwin Coumans https://bulletphysics.org
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erwin Coumans makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
*/
#ifndef BT_WHEEL_INFO_H
#define BT_WHEEL_INFO_H

#include "LinearMath/btVector3.h"
#include "LinearMath/btTransform.h"

class btRigidBody;

struct btWheelInfoConstructionInfo
{
	btVector3 m_chassisConnectionCS;
	btVector3 m_wheelDirectionCS;
	btVector3 m_wheelAxleCS;
	btScalar m_suspensionRestLength;
	btScalar m_maxSuspensionTravelCm;
	btScalar m_wheelRadius;

	btScalar m_suspensionStiffness;
	btScalar m_wheelsDampingCompression;
	btScalar m_wheelsDampingRelaxation;
	btScalar m_frictionSlip;
	btScalar m_maxSuspensionForce;
	bool m_bIsFrontWheel;
};

/// btWheelInfo contains information per wheel about friction and suspension.
struct btWheelInfo
{
	struct RaycastInfo
	{
		//set by raycaster
		btVector3 m_contactNormalWS;  //contactnormal
		btVector3 m_contactPointWS;   //raycast hitpoint
		btScalar m_suspensionLength = 0;
		btVector3 m_hardPointWS;       //raycast starting point
		btVector3 m_wheelDirectionWS;  //direction in worldspace
		btVector3 m_wheelAxleWS;       // axle in worldspace
		bool m_isInContact = 0;
		void* m_groundObject = 0;  //could be general void* ptr
	};

	RaycastInfo m_raycastInfo;

	btTransform m_worldTransform;

	btVector3 m_chassisConnectionPointCS;  //const
	btVector3 m_wheelDirectionCS;          //const
	btVector3 m_wheelAxleCS;               // const or modified by steering
	btScalar m_suspensionRestLength1 = 0;      //const
	btScalar m_maxSuspensionTravelCm = 0;
	btScalar getSuspensionRestLength() const;
	btScalar m_wheelsRadius = 0;              //const
	btScalar m_suspensionStiffness = 0;       //const
	btScalar m_wheelsDampingCompression = 0;  //const
	btScalar m_wheelsDampingRelaxation = 0;   //const
	btScalar m_frictionSlip = 0;
	btScalar m_steering = 0;
	btScalar m_rotation = 0;
	btScalar m_deltaRotation = 0;
	btScalar m_rollInfluence = 0;
	btScalar m_maxSuspensionForce = 0;

	btScalar m_engineForce = 0;

	btScalar m_brake = 0;

	bool m_bIsFrontWheel = 0;

	void* m_clientInfo = 0;  //can be used to store pointer to sync transforms...

	btWheelInfo() {}

	btWheelInfo(btWheelInfoConstructionInfo& ci)

	{
		m_suspensionRestLength1 = ci.m_suspensionRestLength;
		m_maxSuspensionTravelCm = ci.m_maxSuspensionTravelCm;

		m_wheelsRadius = ci.m_wheelRadius;
		m_suspensionStiffness = ci.m_suspensionStiffness;
		m_wheelsDampingCompression = ci.m_wheelsDampingCompression;
		m_wheelsDampingRelaxation = ci.m_wheelsDampingRelaxation;
		m_chassisConnectionPointCS = ci.m_chassisConnectionCS;
		m_wheelDirectionCS = ci.m_wheelDirectionCS;
		m_wheelAxleCS = ci.m_wheelAxleCS;
		m_frictionSlip = ci.m_frictionSlip;
		m_steering = btScalar(0.);
		m_engineForce = btScalar(0.);
		m_rotation = btScalar(0.);
		m_deltaRotation = btScalar(0.);
		m_brake = btScalar(0.);
		m_rollInfluence = btScalar(0.1);
		m_bIsFrontWheel = ci.m_bIsFrontWheel;
		m_maxSuspensionForce = ci.m_maxSuspensionForce;
	}

	void updateWheel(const btRigidBody& chassis, RaycastInfo& raycastInfo);

	btScalar m_clippedInvContactDotSuspension = 0;
	btScalar m_suspensionRelativeVelocity = 0;
	//calculated by suspension
	btScalar m_wheelsSuspensionForce = 0;
	btScalar m_skidInfo = 0;
};

#endif  //BT_WHEEL_INFO_H
