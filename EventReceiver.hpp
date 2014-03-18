#include <irrlicht/irrlicht.h>
#include "Splash.hpp"
#include <cstdio>

class EventReceiver : public irr::IEventReceiver
{
	private:
		irr::IrrlichtDevice* mDevice;
		Splash *mSplash;

	public:
		EventReceiver(irr::IrrlichtDevice* device, Splash *splash)
		{
			mDevice = device;
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
						/*
						// frame 0 -> 139
						int start = 0;
						if(selected->getEndFrame() == 135)
							start = 0;
						else
							start = selected->getEndFrame();
						
						selected->setFrameLoop(start, (start+45));
						*/
						irr::s32 id = selected->getID()-1;
						printf("click: %d, %d\n", id/4, id%4);

						mSplash->action_cli(id/4, id%4);
						mSplash->display();

						for(int i = 0; i < 4; i++)
						{
							for(int j = 0; j < 4; j++)
							{
								irr::scene::IAnimatedMeshSceneNode *node =
									static_cast<irr::scene::IAnimatedMeshSceneNode*>(
											smgr->getSceneNodeFromId(i*4+j+1)
											);

								if(node)
								{
									int cell = mSplash->getCell(i, j);
									if(cell == 0)
										node->remove();
									else
										node->setFrameLoop(node->getEndFrame(), cell*45);
								}
							}
						}

					}

					return true;
				}
			}

			return false;
		}
};

