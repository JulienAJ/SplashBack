#include <irrlicht/irrlicht.h>
#include "Game.hpp"

using namespace irr;

class EventReceiver : public irr::IEventReceiver
{
	private:
		irr::IrrlichtDevice* mDevice;

	public:
		EventReceiver(irr::IrrlichtDevice* device)
		{
			mDevice = device;
		}

		virtual bool OnEvent(const irr::SEvent& event)
		{
			if(event.EventType == irr::EET_KEY_INPUT_EVENT)
			{
				//Clavier
				if(event.KeyInput.Key == irr::KEY_ESCAPE)
				{
					mDevice->closeDevice();
					return true;
				}
			}
			else if(event.EventType == irr::EET_MOUSE_INPUT_EVENT)
			{
				if(event.MouseInput.Event == irr::EMIE_LMOUSE_DOUBLE_CLICK)
				{
					return true;
				}
			}

			return false;
		}
};

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
	scene::ISceneCollisionManager *colmgr = smgr->getSceneCollisionManager();
	static scene::ISceneNode *last_selected = 0;
	scene::ISceneNode *selected = 0;

	driver->beginScene(true, true, video::SColor(0, 0, 0, 0));

	selected = colmgr->getSceneNodeFromCameraBB(camera, 0xffffff);
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

	f32 radius = tile_size.Z/7;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			scene::IMeshSceneNode *tile = smgr->addMeshSceneNode(
					tile_mesh,
					0,
					0,
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

			if(splash->getCell(i, j) != 0)
			{

				f32 x = (tile_size.Z*i)-(radius/2);
				f32 y = (tile_size.Z*j)-(radius/2);
				scene::ISceneNode *water_ball = smgr->addSphereSceneNode(splash->getCell(i, j)*radius,
						16, 0, 1, irr::core::vector3df(x, y, 0));
				water_ball->setMaterialTexture(0,
						driver->getTexture("media/WaterTexture.jpg"));
				water_ball->setMaterialFlag(video::EMF_LIGHTING, false);
			}
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

	camera = smgr->addCameraSceneNodeFPS(0, 60.0f, 0.1f, -1, keymap, 8, true);
	camera->setPosition(core::vector3df(0, 0, -50));
}

int Game::getExitCode()
{
	return exitCode;
}
