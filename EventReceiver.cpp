#include <irrlicht/irrlicht.h>

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
				if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
				{
					irr::scene::IAnimatedMeshSceneNode *selected = 0;
					selected = static_cast<irr::scene::IAnimatedMeshSceneNode*>(mDevice->getSceneManager()->getSceneCollisionManager()->getSceneNodeFromCameraBB(mDevice->getSceneManager()->getActiveCamera(), 0xffffff));
					if(selected)
					{
						// frame 0 -> 139
						int start = 0;
						if(selected->getEndFrame() == 135)
							start = 0;
						else
							start = selected->getEndFrame();
						
						selected->setFrameLoop(start, (start+45));
					}
				}
			}

			return false;
		}
};

