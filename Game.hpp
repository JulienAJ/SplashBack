#ifndef GAME_HPP
#define GAME_HPP

#include <irrlicht/irrlicht.h>
#include <list>
#include "Splash.hpp"
#include "VProgressBar.hpp"

typedef std::pair<irr::scene::ISceneNodeAnimator*, irr::scene::ISceneNode*> Animation;

class Game
{
	irr::IrrlichtDevice *device;
	irr::video::IVideoDriver *driver;
	irr::scene::ISceneManager *smgr;
	irr::scene::ICameraSceneNode *camera;

	gui::IGUIFont* font;

	VProgressBar *shots;

	irr::scene::IMesh *bullet_mesh;
	std::list<std::pair<Animation, int> > bulletsAnim;
	Splash *splash;
	irr::f32 tile_size;
	int exitCode;

	void setupCamera();
	void loadScene();
	void loadBalls();
	void updateBoard();

	irr::core::stringw getLevel();

	public:
		Game();
		~Game();

		void run();
		void update();
		void render();
		void play(int, int, bool = true, int = 0);

		enum { OK = 0, DEVICE_ERROR };
		int getExitCode();
};

#endif
