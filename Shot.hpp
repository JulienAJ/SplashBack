#ifndef SHOT_HPP
#define SHOT_HPP

#include <irrlicht/irrlicht.h>
#include <vector>

class Shot
{
	private:
		irr::scene::IMesh *bulletMesh;
		irr::scene::IMeshSceneNode *node;
		irr::core::vector3df radius;
		irr::scene::ISceneNodeAnimator *flyAnimator;
		irr::scene::ISceneNodeAnimator *toDelete;
		std::vector<irr::scene::ISceneNodeAnimator*> collisionAnimators;

	public:
		Shot(irr::scene::ISceneManager*);
		~Shot();

		bool hasFinished() const;
		void update();
		void stop();
		void shoot(irr::scene::ISceneManager*);
		irr::scene::IMeshSceneNode* getShotSceneNode() const;
		void addCollisionResponseAnimator(
				irr::scene::ISceneNodeAnimatorCollisionResponse*,
				int = -1);
		void removeCollisionResponseAnimators();
		void removeCollisionResponseAnimator(int);
		irr::core::vector3df getRadius() const;
};

#endif
