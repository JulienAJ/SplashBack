#ifndef GAME_HPP
#define GAME_HPP

#include <irrlicht/irrlicht.h>
#include <list>
#include "Splash.hpp"
#include "VProgressBar.hpp"

struct Animation
{
	irr::scene::ISceneNodeAnimator * animator;
	irr::scene::ISceneNode *node;
	int comboLevel;
	int direction;
};

class Game
{
	irr::IrrlichtDevice *device;
	irr::video::IVideoDriver *driver;
	irr::scene::ISceneManager *smgr;
	irr::scene::ICameraSceneNode *camera;

	gui::IGUIFont* font;

	VProgressBar *shots;

	irr::scene::IMesh *bullet_mesh;
	std::list<Animation> bulletsAnim;
	Splash *splash;
	irr::f32 tile_size;
	int exitCode;
	int state;

	void setupCamera();
	void loadScene();
	void loadBalls();
	void updateBoard();
	void clearBoard();

	Animation createAnimation(std::pair<int, int>&, std::pair<int, int>&, int, int);

	irr::core::stringw getLevel();

	public:
		Game();
		~Game();

		void run();
		void update();
		void render();
		void play(int, int, bool = true, int = 0);
		void gameOver();
		void restart();

		enum { OK = 0, DEVICE_ERROR };
		enum { PLAYING = 0, OVER };
		int getExitCode();
		int getState();
};

#endif
