#include <irrlicht/irrlicht.h>
#include <iostream>
#include "Game.hpp"
#include "EventReceiver.hpp"
#include "Shot.hpp"

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
	state = PLAYING;
	bullet_mesh = smgr->getMesh("media/elipse.3ds");
	ball_mesh = 0;

	shotsBar = new VProgressBar(device, 50, 600, 50, 200, 20, device->getVideoDriver()->getTexture("media/WaterTexture2.jpg"));
	shotsBar->setValue(splash->getShots());
	shotsBar->setCritical(5);
	shotsBar->setOverBar(true);

	font = device->getGUIEnvironment()->getFont("media/ScoreFont.png");

	shot = new Shot(smgr);
}

Game::~Game()
{
	delete splash;
	delete shotsBar;
	delete shot;
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
	EventReceiver eventReceiver(device, this, splash);
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
	shotsBar->draw();
	font->draw(getLevel(), core::rect<s32>(0, 0, 300, 100), video::SColor(255, 255, 255, 255));
	device->getGUIEnvironment()->drawAll();
	driver->endScene();
}

void Game::update()
{
	std::list<Animation>::iterator it = bulletsAnim.begin();

	while(it != bulletsAnim.end())
	{
		if(it->animator->hasFinished())
		{
			it->animator->drop();

			core::vector3df position = it->node->getPosition();
			int l = -((position.Y/tile_size)-3);
			int c = position.X/tile_size;

			if(splash->getCell(l, c) != 0)
				play(l, c, false, it->comboLevel+1);
			else if(!splash->onEdge(l, c, it->direction))
			{
				std::pair<int, int> source(l, c);
				std::pair<int, int> fp(splash->getFinalPosition(l, c, it->direction));
				bulletsAnim.push_back(createAnimation(source, fp, it->comboLevel,
							it->direction));
			}

			it->node->remove();
			bulletsAnim.erase(it++);
		}
		else
			++it;
	}

	int score = splash->getShots();
	shotsBar->setValue(score);

	if(bulletsAnim.empty())
	{
		if(splash->empty())
		{
			splash->nextLevel();
			loadBalls();
		}

		if(state == PLAYING && score == 0)
			gameOver();
	}

	shot->update();
}

void Game::play(int line, int column, bool userEvent, int lastComboLevel)
{
	if(userEvent && !bulletsAnim.empty())
		return;

	Bullets bullets = splash->action(line, column, userEvent, lastComboLevel);

	if(bullets.source.first != -1)
	{
		for(int i = 0; i < 4; i++)
			bulletsAnim.push_back(createAnimation(bullets.source,
						bullets.finalPosition[i], bullets.lastComboLevel, i));
	}

	updateBoard();
}

Animation Game::createAnimation(std::pair<int, int> &source,
		std::pair<int, int> &finalPosition, int comboLevel, int direction)
{
	const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
	Animation anim;

	int xs = source.first;
	int ys = source.second;
	core::vector3df start(ys*tile_size, (3-xs)*tile_size, 0);

	int x = finalPosition.first;
	int y = finalPosition.second;

	core::vector3df end(y*tile_size, (3-x)*tile_size, 0);
	u32 time = (((x-xs)*deltas[direction][0])+((y-ys)*deltas[direction][1]))*400; // 400ms/case

	int orientation = 0;
	if(ys == y)
		orientation = 90;

	scene::ISceneNodeAnimator *animator = smgr->createFlyStraightAnimator(
			start, end, time);

	scene::IMeshSceneNode *bullet = smgr->addMeshSceneNode(bullet_mesh, 0, -1,
			core::vector3df(x, y, 0), core::vector3df(0, 90, orientation),
			core::vector3df(1.5f, 1.5f, 1.5f));
	bullet->setMaterialFlag(video::EMF_LIGHTING, false);
	bullet->setMaterialTexture(0, driver->getTexture("media/WaterTexture2.jpg"));
	bullet->addAnimator(animator);

	anim.animator = animator;
	anim.node = bullet;
	anim.comboLevel = comboLevel;
	anim.direction = direction;

	return anim;
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
			int cell = splash->getCell(i, j);

			if(node)
			{
				if(cell == 0)
					node->remove();
				else
					node->setFrameLoop(node->getEndFrame(), cell*45);
			}
			else
			{
				createWaterBall(cell, i, j);
			}
		}
	}
}

void Game::clearBoard()
{
	scene::IAnimatedMeshSceneNode *node = 0;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			node = static_cast<scene::IAnimatedMeshSceneNode*>(
					smgr->getSceneNodeFromId(i*4+j+1));

			if(node)
				node->remove();
		}
	}

	// TODO: shot -> remove all animators
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

	//Adjust Camera
	camera->setPosition(core::vector3df(1.5*tile_size, 1.5*tile_size, -50));
	camera->setTarget(core::vector3df(1.5*tile_size, 1.5*tile_size, 0));

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

			scene::IMeshSceneNode *tile = smgr->addMeshSceneNode(tile_mesh, 0, id+17,
					core::vector3df(x, y, 0), core::vector3df(90, 0, 0));
			tile->setMaterialFlag(video::EMF_LIGHTING, false);
			tile->setMaterialTexture(0, driver->getTexture("media/tile.jpg"));

			scene::ITriangleSelector *selector = smgr->createTriangleSelector(tile_mesh, tile);


			const core::aabbox3d<f32>& box = tile->getBoundingBox();
			core::vector3df radius = box.MaxEdge - box.getCenter();
			scene::ISceneNodeAnimator *anim = smgr->createCollisionResponseAnimator(
					selector, camera, radius, core::vector3df(0, 0, 0));
			selector->drop(); //plus besoin
			camera->addAnimator(anim);
			anim->drop(); //plus besoin

			id++;
		}
	}

	loadBalls();
}

void Game::loadBalls()
{
	ball_mesh = smgr->getMesh("media/v4.md3");

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			int cell = splash->getCell(i, j);
			createWaterBall(cell, i, j);
		}
	}
}

void Game::createWaterBall(int size, int line, int column)
{
	if(size == 0)
		return;

	s32 x = column*tile_size;
	s32 y = (3-line)*tile_size;

	s32 id = line*4+column+1;

	scene::IAnimatedMeshSceneNode *water_ball = smgr->addAnimatedMeshSceneNode(
			ball_mesh, 0, id, core::vector3df(x, y, 0));
	water_ball->setMaterialFlag(video::EMF_LIGHTING, false);
	water_ball->setMaterialTexture(0, driver->getTexture("media/WaterTexture2.jpg"));

	water_ball->setLoopMode(false);
	water_ball->setFrameLoop(0, size*45);
	water_ball->setAnimationSpeed(100);

	scene::ITriangleSelector *selector = smgr->createTriangleSelector(water_ball);
	water_ball->setTriangleSelector(selector);

	scene::ISceneNodeAnimatorCollisionResponse *anim =
		smgr->createCollisionResponseAnimator(selector, shot->getShotSceneNode(),
				core::vector3df(.2f, .2f, .2f)*size, core::vector3df(0, 0, 0));
	selector->drop(); //plus besoin
	anim->setCollisionCallback(this);
	shot->addCollisionResponseAnimator(anim);
	anim->drop(); //plus besoin
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

void Game::restart()
{
	bulletsAnim.clear();
	state = PLAYING;
	splash->restart();

	clearBoard();
	updateBoard();
}

int Game::getExitCode()
{
	return exitCode;
}

int Game::getState()
{
	return state;
}

core::stringw Game::getLevel()
{
	wchar_t ret[16];

	swprintf(ret, sizeof(ret)/sizeof(*ret), L"Level %d", splash->getLevel());
	return (core::stringw)ret;
}

void Game::gameOver()
{
	state = OVER;
	device->getGUIEnvironment()->addMessageBox(L"GAME OVER", L"RECOMMENCER ?", true,
			irr::gui::EMBF_YES | irr::gui::EMBF_NO);
}

void Game::shoot()
{
	shot->shoot(smgr);
}

bool Game::onCollision(const scene::ISceneNodeAnimatorCollisionResponse &animator)
{
	shot->stop();
	shot->removeCollisionResponseAnimator(&animator);

	scene::ISceneNode *node = animator.getCollisionNode();
	s32 n = node->getID()-1;

	printf("%d, %d\n", n/4, n%4);

	return true;
}
