#include <irrlicht/irrlicht.h>
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
	EventReceiver eventReceiver(device);
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
	//animations, etc..
}

void Game::loadScene()
{
	scene::IMesh *tile_mesh = smgr->getMesh("media/tile.3ds");
	scene::IAnimatedMesh *ball_mesh = smgr->getMesh("media/v4.md3");
	scene::IAnimatedMeshMD2* weaponMesh = static_cast<scene::IAnimatedMeshMD2*>(
			smgr->getMesh("media/gun.md2"));
	core::vector3d<f32> tile_size = tile_mesh->getBoundingBox().getExtent();

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
			s32 x = j*tile_size.Z;
			s32 y = (3-i)*tile_size.Z;

			scene::IMeshSceneNode *tile = smgr->addMeshSceneNode(tile_mesh, 0, 0,
					core::vector3df(x, y, 0), core::vector3df(90, 0, 0));
			tile->setMaterialFlag(video::EMF_LIGHTING, false);
			tile->setMaterialTexture(0, driver->getTexture("media/tile.jpg"));

			scene::ITriangleSelector *selector = smgr->createTriangleSelector(tile_mesh, tile);
			tile->setTriangleSelector(selector);


			scene::ISceneNodeAnimator *anim = smgr->createCollisionResponseAnimator(
					selector, camera, core::vector3df(30, 60, 30), core::vector3df(0, 0, 0));
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
				water_ball->setFrameLoop(cell*45, cell*45);
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
