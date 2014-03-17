#ifndef GAME_HPP
#define GAME_HPP

#include <irrlicht/irrlicht.h>
#include "Splash.hpp"

class Game
{
	irr::IrrlichtDevice *device;
	irr::video::IVideoDriver *driver;
	irr::scene::ISceneManager *smgr;
	irr::scene::ICameraSceneNode *camera;
	Splash *splash;

	int exitCode;

	void setupCamera();
	void loadScene();

	public:
		Game();
		~Game();

		void run();
		void update();
		void render();

		enum { OK = 0, DEVICE_ERROR };
		int getExitCode();
};

#endif
