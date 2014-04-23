#include <irrlicht/irrlicht.h>
#include "Game.hpp"
#include "Splash.hpp"

class EventReceiver : public irr::IEventReceiver
{
	private:
		irr::IrrlichtDevice *mDevice;
		Game *mGame;
		Splash *mSplash;

	public:
		EventReceiver(irr::IrrlichtDevice* device, Game *game, Splash *splash)
		{
			mDevice = device;
			mGame = game;
			mSplash = splash;
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
				if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN &&
						mGame->getState() != Game::OVER)
				{
					irr::scene::ISceneManager *smgr = mDevice->getSceneManager();

					irr::scene::IAnimatedMeshSceneNode *selected = 0;
					selected = static_cast<irr::scene::IAnimatedMeshSceneNode*>(
							smgr->getSceneCollisionManager()->getSceneNodeFromCameraBB(
								smgr->getActiveCamera(), 0xffffffff)
							);

					if(selected)
					{
						irr::s32 id = selected->getID()-1;
						bool tile = (id > 16);

						if(tile)
							id -= 17;

						int line = id/4;
						int column = id%4;

						if(!tile || mSplash->getCell(line, column) == 0)
							mGame->play(line, column);

						//Animation Gun
						irr::scene::IAnimatedMeshSceneNode* gun = 0;
						irr::core::list<irr::scene::ISceneNode*> children;
						children = smgr->getActiveCamera()->getChildren();
						gun = static_cast<irr::scene::IAnimatedMeshSceneNode*>(*children.getLast());

						gun->setMD2Animation("pow");
						gun->setAnimationEndCallback(0);
					}

					return true;
				}
			}

			return false;
		}
};

