#ifndef SHOT_HPP
#define SHOT_HPP

#include <irrlicht/irrlicht.h>

class Shot
{
	private:
		irr::scene::IMeshSceneNode *node;
		irr::scene::ISceneNodeAnimator *flyAnimator;
		irr::scene::ISceneNodeAnimatorList collisionAnimators;
	
	public:
		Shot(irr::scene::ISceneManager*);
		~Shot();

		bool hasFinished() const;
		void update();
		void stop();
		void shoot(irr::scene::ISceneManager*);
		irr::scene::IMeshSceneNode* getShotSceneNode() const;
		void addCollisionResponseAnimator(
				irr::scene::ISceneNodeAnimatorCollisionResponse*);
		bool removeCollisionResponseAnimator(
				const irr::scene::ISceneNodeAnimatorCollisionResponse*);
};

#endif
