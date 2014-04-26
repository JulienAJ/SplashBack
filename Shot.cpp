#include <irrlicht/irrlicht.h>
#include <cstdio>
#include "Shot.hpp"

Shot::Shot(irr::scene::ISceneManager *smgr)
{
	node = smgr->addSphereSceneNode(1, 16, 0, -1);
	node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	node->setVisible(false);
	flyAnimator = 0;
	collisionAnimators.resize(16, 0); //16 balles
}

Shot::~Shot()
{
	if(flyAnimator)
		flyAnimator->drop();

	node->remove();
}

bool Shot::hasFinished() const
{
	return (flyAnimator) ? flyAnimator->hasFinished() : true;
}

void Shot::stop()
{
	if(flyAnimator)
	{
		node->removeAnimator(flyAnimator);
		flyAnimator = 0;

		node->setVisible(false);
	}
}

void Shot::update()
{
	if(flyAnimator && flyAnimator->hasFinished())
		stop();
}

void Shot::shoot(irr::scene::ISceneManager *smgr)
{
	if(!hasFinished())
		return;

	stop();

	irr::scene::ICameraSceneNode *camera = smgr->getActiveCamera();
	irr::core::vector3df target = camera->getTarget();
	irr::core::vector3df pos = camera->getPosition();

	flyAnimator = smgr->createFlyStraightAnimator(
			pos, target, pos.getDistanceFrom(target)*10); // 10ms/unité

	node->addAnimator(flyAnimator);
	flyAnimator->drop();
	node->setVisible(true);
}

irr::scene::IMeshSceneNode* Shot::getShotSceneNode() const
{
	return node;
}

void Shot::addCollisionResponseAnimator(
		irr::scene::ISceneNodeAnimatorCollisionResponse *anim,
		int nodeID)
{
	if(anim)
	{
		node->addAnimator(anim);
		if(nodeID != -1)
			collisionAnimators[nodeID-1] = anim;
	}
}

void Shot::removeCollisionResponseAnimator(int nodeID)
{
	int n = nodeID-1;
	node->removeAnimator(collisionAnimators[n]);
	collisionAnimators[n] = 0;
}

void Shot::removeCollisionResponseAnimators()
{
	std::vector<irr::scene::ISceneNodeAnimator*>::iterator it;
	it = collisionAnimators.begin();

	for(; it != collisionAnimators.end(); ++it)
	{
		node->removeAnimator(*it);
		*it = 0;
	}
}
