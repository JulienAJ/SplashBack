#include <irrlicht/irrlicht.h>
#include <cmath>
#include <cstdio>
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

	splash->display();
}

Game::~Game()
{
	delete splash;
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
			printf("finished (%p), remove(%p)\n", it->first, it->second);
			it->first->drop();

			core::vector3df position = it->second->getPosition();
			int l = -((position.Y/tile_size)-3);
			int c = position.X/tile_size;
			printf("postion: %d, %d\n", l, c);

			if(splash->getCell(l, c) != 0)
				play(l, c, false);

			it->second->remove();

			toErase.push_back(it);
		}
	}

	for(eraseIt = toErase.begin(); eraseIt != toErase.end(); ++eraseIt)
		bulletsAnim.erase(*eraseIt);
}

void Game::play(int line, int column, bool userEvent)
{
	if(userEvent && !bulletsAnim.empty())
		return;

	std::list<Bullets> bullets;
	std::list<Bullets>::iterator it;

	splash->action(line, column, bullets, userEvent);
	printf("play(%d, %d, %d)\n", line, column, userEvent);

	for(it = bullets.begin(); it != bullets.end(); ++it)
	{
		int xs = it->source.first;
		int ys = it->source.second;
		core::vector3df start(ys*tile_size, (3-xs)*tile_size, 0);

		printf("source: %f, %f (%d, %d)\n", start.X, start.Y, xs, ys);

		for(int i = 0; i < 4; i++)
		{
			int x = it->finalPosition[i].first;
			int y = it->finalPosition[i].second;

			core::vector3df end(y*tile_size, (3-x)*tile_size, 0);
			u32 time = sqrt((x-xs)*(x-xs)+(y-ys)*(y-ys))*400; // 400 ms par case

			printf("\tend %d: %f, %f (%d, %d) ; time = %d\n", i, end.X, end.Y, x, y, time);

			scene::ISceneNodeAnimator *animator = smgr->createFlyStraightAnimator(
					start, end, time);
			scene::ISceneNode *bullet = smgr->addSphereSceneNode(1, 16, 0, 0,
					core::vector3df(x, y, 0));
			bullet->setMaterialFlag(video::EMF_LIGHTING, false);
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
	scene::IMesh *tile_mesh = smgr->getMesh("media/tile.3ds");
	scene::IAnimatedMesh *ball_mesh = smgr->getMesh("media/v4.md3");
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

	s32 id = 1;
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

			int cell = splash->getCell(i, j);
			if(cell != 0)
			{
				scene::IAnimatedMeshSceneNode *water_ball = smgr->addAnimatedMeshSceneNode(
						ball_mesh, 0, id, core::vector3df(x, y, 0));
				water_ball->setMaterialFlag(video::EMF_LIGHTING, false);
				water_ball->setMaterialTexture(0, driver->getTexture("media/WaterTexture2.jpg"));
				water_ball->setLoopMode(false);
				water_ball->setFrameLoop(0, cell*45);
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
}

int Game::getExitCode()
{
	return exitCode;
}
