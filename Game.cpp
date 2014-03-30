#include <irrlicht/irrlicht.h>
#include <iostream>
#include "Game.hpp"
#include "EventReceiver.hpp"

using namespace irr;

Game::Game()
{
	device = createDevice(video::EDT_OPENGL,
			core::dimension2d<u32>(680, 480), 32);

	splash = new Splash();

	if(device)
	{
		driver = device->getVideoDriver();
		smgr = device->getSceneManager();
	}

	camera = 0;
	tile_size = 0;
	exitCode = OK;
	bullet_mesh = smgr->getMesh("media/elipse.3ds");

	shots = new VProgressBar(device, 50, 600, 50, 200, 20, device->getVideoDriver()->getTexture("media/WaterTexture2.jpg"));	
	shots->setValue(splash->getShots());
	shots->setCritical(5);

	font = device->getGUIEnvironment()->getFont("media/ScoreFont.png");
}

Game::~Game()
{
	delete splash;
	delete shots;
}

void Game::run()
{
	if(!device)
	{
		exitCode = DEVICE_ERROR;
		return;
	}

	setupCamera();
	loadScene();

	// Evenements
	EventReceiver eventReceiver(device, this);
	device->setEventReceiver(&eventReceiver);

	int last_fps = -1;
	wchar_t title[16];

	while(device->run())
	{
		update();
		render();

		//fps
		int fps = driver->getFPS();
		if(last_fps != fps)
		{
			swprintf(title, sizeof(title)/sizeof(*title), L"FPS: %d", fps);
			device->setWindowCaption(title);
			last_fps = fps;
		}
	}
}

void Game::render()
{
	driver->beginScene(true, true, video::SColor(0, 0, 0, 0));
	smgr->drawAll();
	shots->draw();
	font->draw(getLevel(), core::rect<s32>(0, 0, 300, 100), video::SColor(255, 255, 255, 255));
	device->getGUIEnvironment()->drawAll();
	driver->endScene();
}

void Game::update()
{
	std::list<Animation>::iterator it;
	std::list<std::list<Animation>::iterator> toErase;
	std::list<std::list<Animation>::iterator>::iterator eraseIt;

	for(it = bulletsAnim.begin(); it != bulletsAnim.end(); ++it)
	{
		if(it->first->hasFinished())
		{
			it->first->drop();

			core::vector3df position = it->second->getPosition();
			int l = -((position.Y/tile_size)-3);
			int c = position.X/tile_size;

			if(splash->getCell(l, c) != 0)
				play(l, c, false);

			it->second->remove();

			toErase.push_back(it);
		}
	}

	for(eraseIt = toErase.begin(); eraseIt != toErase.end(); ++eraseIt)
		bulletsAnim.erase(*eraseIt);

	if(bulletsAnim.empty() && splash->empty())
	{
		splash->nextLevel();
		loadBalls();
	}
	shots->setValue(splash->getShots());
}

void Game::play(int line, int column, bool userEvent)
{
	if(userEvent && !bulletsAnim.empty())
		return;

	const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
	Bullets bullets;
	splash->action(line, column, bullets, userEvent);

	if(bullets.source.first != -1)
	{
		int xs = bullets.source.first;
		int ys = bullets.source.second;
		core::vector3df start(ys*tile_size, (3-xs)*tile_size, 0);

		for(int i = 0; i < 4; i++)
		{
			int x = bullets.finalPosition[i].first;
			int y = bullets.finalPosition[i].second;

			core::vector3df end(y*tile_size, (3-x)*tile_size, 0);
			u32 time = (((x-xs)*deltas[i][0])+((y-ys)*deltas[i][1]))*400; // 400ms/case

			int direction = 0;
			if(ys == y)
				direction = 90;

			scene::ISceneNodeAnimator *animator = smgr->createFlyStraightAnimator(
					start, end, time);
			scene::IMeshSceneNode *bullet = smgr->addMeshSceneNode(
					bullet_mesh, 0, 0, core::vector3df(x, y, 0), core::vector3df(0, 90, direction),
					core::vector3df(1.5f, 1.5f, 1.5f));
			bullet->setMaterialFlag(video::EMF_LIGHTING, false);
			bullet->setMaterialTexture(0, driver->getTexture("media/WaterTexture2.jpg"));
			bullet->addAnimator(animator);

			bulletsAnim.push_back(Animation(animator, bullet));
		}
	}

	updateBoard();
}

void Game::updateBoard()
{
	scene::IAnimatedMeshSceneNode *node = 0;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			node = static_cast<scene::IAnimatedMeshSceneNode*>(
					smgr->getSceneNodeFromId(i*4+j+1));

			if(node)
			{
				int cell = splash->getCell(i, j);
				if(cell == 0)
					node->remove();
				else
					node->setFrameLoop(node->getEndFrame(), cell*45);
			}
		}
	}
}

void Game::loadScene()
{
	//SkyBox
	smgr->addSkyBoxSceneNode(driver->getTexture("media/skybox/top.JPG"), driver->getTexture("media/skybox/bottom.JPG"),
			driver->getTexture("media/skybox/left.JPG"), driver->getTexture("media/skybox/right.JPG"),
			driver->getTexture("media/skybox/front.JPG"), driver->getTexture("media/skybox/back.JPG"));

	scene::IMesh *tile_mesh = smgr->getMesh("media/tile.3ds");
	scene::IAnimatedMeshMD2* weaponMesh = static_cast<scene::IAnimatedMeshMD2*>(
			smgr->getMesh("media/gun.md2"));
	tile_size = tile_mesh->getBoundingBox().getExtent().Z;

	scene::IAnimatedMeshSceneNode* weaponNode = smgr->addAnimatedMeshSceneNode(
			weaponMesh, camera, 0, core::vector3df(0,0,0), core::vector3df(-90, -90, 90));
	weaponNode->setMaterialFlag(video::EMF_LIGHTING, false);
	weaponNode->setMaterialTexture(0, driver->getTexture("media/gun.jpg"));
	weaponNode->setLoopMode(false);

	s32 count = weaponMesh->getAnimationCount();
	c8 animationNames[count][256];

	for ( s32 i = 0; i != count; ++i )
	{
		snprintf (animationNames[i], 64, "%s", weaponMesh->getAnimationName(i) );
		device->getLogger()->log(animationNames[i], ELL_INFORMATION);
	}

	weaponNode->setMD2Animation(animationNames[2]);
	weaponNode->setAnimationEndCallback(0);

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			s32 x = j*tile_size;
			s32 y = (3-i)*tile_size;

			scene::IMeshSceneNode *tile = smgr->addMeshSceneNode(tile_mesh, 0, 0,
					core::vector3df(x, y, 0), core::vector3df(90, 0, 0));
			tile->setMaterialFlag(video::EMF_LIGHTING, false);
			tile->setMaterialTexture(0, driver->getTexture("media/tile.jpg"));

			scene::ITriangleSelector *selector = smgr->createTriangleSelector(tile_mesh, tile);
			tile->setTriangleSelector(selector);


			scene::ISceneNodeAnimator *anim = smgr->createCollisionResponseAnimator(
					selector, camera, core::vector3df(30, 15, 30), core::vector3df(0, 0, 0));
			selector->drop(); //plus besoin
			camera->addAnimator(anim);
			anim->drop(); //plus besoin
		}
	}

	loadBalls();
}

void Game::loadBalls()
{
	scene::IAnimatedMesh *ball_mesh = smgr->getMesh("media/v4.md3");
	s32 id = 1;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			s32 x = j*tile_size;
			s32 y = (3-i)*tile_size;

			int cell = splash->getCell(i, j);
			if(cell != 0)
			{
				scene::IAnimatedMeshSceneNode *water_ball = smgr->addAnimatedMeshSceneNode(
						ball_mesh, 0, id, core::vector3df(x, y, 0));
				water_ball->setMaterialFlag(video::EMF_LIGHTING, false);
				water_ball->setMaterialTexture(0, driver->getTexture("media/WaterTexture2.jpg"));
				water_ball->setLoopMode(false);
				water_ball->setFrameLoop(0, cell*45);
				water_ball->setAnimationSpeed(100);
			}

			id++;
		}
	}
}


void Game::setupCamera()
{
	SKeyMap keymap[8];

	keymap[0].Action = EKA_MOVE_FORWARD;
	keymap[0].KeyCode = KEY_KEY_Z;

	keymap[1].Action = EKA_MOVE_BACKWARD;
	keymap[1].KeyCode = KEY_KEY_S;

	keymap[2].Action = EKA_STRAFE_LEFT;
	keymap[2].KeyCode = KEY_KEY_Q;

	keymap[3].Action = EKA_STRAFE_RIGHT;
	keymap[3].KeyCode = KEY_KEY_D;

	keymap[4].Action = EKA_MOVE_FORWARD;
	keymap[4].KeyCode = KEY_UP;

	keymap[5].Action = EKA_MOVE_BACKWARD;
	keymap[5].KeyCode = KEY_DOWN;

	keymap[6].Action = EKA_STRAFE_LEFT;
	keymap[6].KeyCode = KEY_LEFT;

	keymap[7].Action = EKA_STRAFE_RIGHT;
	keymap[7].KeyCode = KEY_RIGHT;

	camera = smgr->addCameraSceneNodeFPS(0, 60.0f, 0.1f, 0, keymap, 8, true);
	camera->setPosition(core::vector3df(0, 0, -50));

	// Cursor
	device->getCursorControl()->setActiveIcon(gui::ECI_CROSS);
}

int Game::getExitCode()
{
	return exitCode;
}

core::stringw Game::getLevel()
{
	wchar_t ret[16];

	swprintf(ret, sizeof(ret)/sizeof(*ret), L"Level %d", splash->getLevel());
	return (core::stringw)ret;
}
