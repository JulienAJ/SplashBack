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

					mGame->shoot();

					//Animation Gun
					irr::scene::IAnimatedMeshSceneNode* gun = 0;
					irr::core::list<irr::scene::ISceneNode*> children;
					children = smgr->getActiveCamera()->getChildren();
					gun = static_cast<irr::scene::IAnimatedMeshSceneNode*>(*children.getLast());
					gun->setMD2Animation("pow");
					gun->setAnimationEndCallback(0);

					return true;
				}
			}
			else if(event.EventType == irr::EET_GUI_EVENT)
			{
				if(event.GUIEvent.EventType == irr::gui::EGET_MESSAGEBOX_YES)
				{
					mGame->restart();
					return true;
				}
				else if(event.GUIEvent.EventType == irr::gui::EGET_MESSAGEBOX_NO)
				{
					mDevice->closeDevice();
					return true;
				}
			}

			return false;
		}
};

