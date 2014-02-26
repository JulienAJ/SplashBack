#include <irrlicht/irrlicht.h>
#include "Game.hpp"

using namespace irr;

Game::Game()
{
	device = createDevice(video::EDT_OPENGL,
			core::dimension2d<u32>(680, 480), 32);

	if(device)
	{
		driver = device->getVideoDriver();
		smgr = device->getSceneManager();
	}

	camera = 0;
	exitCode = OK;
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
			swprintf(title, sizeof(title), L"FPS: %d", fps);
			device->setWindowCaption(title);
			last_fps = fps;
		}
	}
}

void Game::render()
{
	scene::ISceneCollisionManager *colmgr = smgr->getSceneCollisionManager();
	static scene::ISceneNode *last_selected = 0;
	scene::ISceneNode *selected = 0;

	driver->beginScene(true, true, video::SColor(0, 0, 0, 0));

	core::line3d<f32> ray;
	ray.start = camera->getPosition();
	ray.end = ray.start + (camera->getTarget() - ray.start).normalize() * 1000.0f;

	selected = colmgr->getSceneNodeFromRayBB(ray);
	if(selected && last_selected != selected)
	{
		if(last_selected)
			last_selected->setMaterialFlag(video::EMF_WIREFRAME, false);

		selected->setMaterialFlag(video::EMF_WIREFRAME, true);
		last_selected = selected;
	}

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
	core::vector3d<f32> tile_size = tile_mesh->getBoundingBox().getExtent();

	f32 radius = tile_size.Z/3;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			scene::IMeshSceneNode *tile = smgr->addMeshSceneNode(
					tile_mesh,
					0,
					-1,
					core::vector3df(i*tile_size.Z, j*tile_size.Z, 0),
					core::vector3df(90, 0, 0)
					);

			tile->setMaterialFlag(video::EMF_LIGHTING, false);
			tile->setMaterialTexture(0, driver->getTexture("media/tile.jpg"));

			scene::ITriangleSelector *selector = smgr->createTriangleSelector(tile_mesh, tile);
			tile->setTriangleSelector(selector);


			scene::ISceneNodeAnimator *anim = smgr->createCollisionResponseAnimator(selector,
					camera, core::vector3df(30, 60, 30), core::vector3df(0, 0, 0));
			selector->drop(); //plus besoin
			camera->addAnimator(anim);
			anim->drop(); //plus besoin


			f32 x = (tile_size.Z*i)-(radius/2);
			f32 y = (tile_size.Z*j)-(radius/2);
			scene::ISceneNode *water_ball = smgr->addSphereSceneNode(radius,
					16, 0, -1, irr::core::vector3df(x, y, 0));
			water_ball->setMaterialTexture(0,
					driver->getTexture("media/WaterTexture.jpg"));
			water_ball->setMaterialFlag(video::EMF_LIGHTING, false);
		}
	}
}

void Game::setupCamera()
{
	SKeyMap keymap[4];

	keymap[0].Action = EKA_MOVE_FORWARD;
	keymap[0].KeyCode = KEY_KEY_Z;

	keymap[1].Action = EKA_MOVE_BACKWARD;
	keymap[1].KeyCode = KEY_KEY_S;

	keymap[2].Action = EKA_STRAFE_LEFT;
	keymap[2].KeyCode = KEY_KEY_Q;

	keymap[3].Action = EKA_STRAFE_RIGHT;
	keymap[3].KeyCode = KEY_KEY_D;


	camera = smgr->addCameraSceneNodeFPS(0, 60.0f, 0.1f, -1, keymap, 4, true);
	camera->setPosition(core::vector3df(0, 0, -50));
}

int Game::getExitCode()
{
	return exitCode;
}
