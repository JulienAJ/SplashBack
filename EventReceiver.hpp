#include <irrlicht/irrlicht.h>
#include "Splash.hpp"

class EventReceiver : public irr::IEventReceiver
{
	private:
		irr::IrrlichtDevice* mDevice;
		Game *mGame;

	public:
		EventReceiver(irr::IrrlichtDevice* device, Game *game)
		{
			mDevice = device;
			mGame = game;
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
				if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
				{
					irr::scene::ISceneManager *smgr = mDevice->getSceneManager();

					irr::scene::IAnimatedMeshSceneNode *selected = 0;
					selected = static_cast<irr::scene::IAnimatedMeshSceneNode*>(
							smgr->getSceneCollisionManager()->getSceneNodeFromCameraBB(
								smgr->getActiveCamera(), 0xffffff)
							);

					if(selected)
					{
						irr::s32 id = selected->getID()-1;

						mGame->play(id/4, id%4);
					}

					return true;
				}
			}

			return false;
		}
};

