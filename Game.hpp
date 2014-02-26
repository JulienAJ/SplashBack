#ifndef GAME_HPP
#define GAME_HPP

#include <irrlicht/irrlicht.h>

class Game
{
	irr::IrrlichtDevice *device;
	irr::video::IVideoDriver *driver;
	irr::scene::ISceneManager *smgr;
	irr::scene::ICameraSceneNode *camera;

	int exitCode;

	void setupCamera();
	void loadScene();

	public:
		Game();

		void run();
		void update();
		void render();

		enum { OK = 0, DEVICE_ERROR };
		int getExitCode();
};

#endif
