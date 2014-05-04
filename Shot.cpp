#include <irrlicht/irrlicht.h>
#include <cstdio>
#include "Shot.hpp"

Shot::Shot(irr::scene::ISceneManager *smgr)
{
	node = smgr->addSphereSceneNode(1, 16, 0, -1);
	node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	node->setVisible(false);
	flyAnimator = 0;
	toDelete = 0;
	collisionAnimators.resize(16, 0); //16 balles

	const irr::core::aabbox3d<irr::f32>& box = node->getBoundingBox();
	radius = box.MaxEdge - box.getCenter();
}

Shot::~Shot()
{
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
		toDelete = flyAnimator;
		flyAnimator = 0;

		node->setVisible(false);
		node->setPosition(irr::core::vector3df(0, 100, -100));
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

	irr::scene::ICameraSceneNode *camera = smgr->getActiveCamera();
	irr::core::vector3df target = camera->getTarget();
	irr::core::vector3df pos = camera->getPosition();

	// aller un peu plus loin pour être sûr de provoquer une collision
	irr::core::vector3df direction = target-pos;
	target += direction;

	node->setVisible(false);
	node->setPosition(irr::core::vector3df(0, 100, -100));

	flyAnimator = smgr->createFlyStraightAnimator(
			pos, target, pos.getDistanceFrom(target)*10); // 10ms/unité

	node->addAnimator(flyAnimator);
	flyAnimator->drop();
	node->setVisible(true);
	node->removeAnimator(toDelete);
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

irr::core::vector3df Shot::getRadius() const
{
	return radius;
}
