#ifndef GAME_HPP
#define GAME_HPP

#include <irrlicht/irrlicht.h>
#include <list>
#include "Splash.hpp"

typedef std::pair<irr::scene::ISceneNodeAnimator*, irr::scene::ISceneNode*> Animation;

class Game
{
	irr::IrrlichtDevice *device;
	irr::video::IVideoDriver *driver;
	irr::scene::ISceneManager *smgr;
	irr::scene::ICameraSceneNode *camera;

	irr::scene::IMesh *bullet_mesh;
	std::list<Animation> bulletsAnim;
	Splash *splash;
	irr::f32 tile_size;
	int exitCode;

	void setupCamera();
	void loadScene();
	void loadBalls();
	void updateBoard();

	public:
		Game();
		~Game();

		void run();
		void update();
		void render();
		void play(int, int, bool = true);

		enum { OK = 0, DEVICE_ERROR };
		int getExitCode();
};

#endif
