#include <irrlicht/irrlicht.h>
#include <cstdio>
#include "Shot.hpp"

Shot::Shot(irr::scene::ISceneManager *smgr)
{
	node = smgr->addSphereSceneNode(1, 16, 0, -1);
	node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	node->setVisible(false);
	flyAnimator = 0;
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
	printf("Stop called !\n");
	if(flyAnimator)
	{
		node->removeAnimator(flyAnimator);
		flyAnimator = 0;

		node->setPosition(irr::core::vector3df(0, 0, 0));
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
		irr::scene::ISceneNodeAnimatorCollisionResponse *anim)
{
	if(anim)
	{
		node->addAnimator(anim);
		collisionAnimators.push_back(anim);
	}
}

bool Shot::removeCollisionResponseAnimator(
		const irr::scene::ISceneNodeAnimatorCollisionResponse *anim)
{
	if(!anim)
		return false;

	irr::scene::ISceneNodeAnimatorList::Iterator it;
	it = collisionAnimators.begin();

	for(; it != collisionAnimators.end(); ++it)
	{
		if(*it == anim)
		{
			collisionAnimators.erase(it);
			node->removeAnimator(*it);
			return true;
		}
	}

	return false;
}
