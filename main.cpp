#include <irrlicht.h>
#include <iostream>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class EventReceiver : public IEventReceiver {
public:
	struct SMouseState
	{
		core::position2di Position;
		bool LeftButtonDown;
		SMouseState() : LeftButtonDown(false) { }
	} MouseState;
	EventReceiver() {}
	virtual bool OnEvent(const SEvent& event) {
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch (event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.LeftButtonDown = true;
				break;

			case EMIE_LMOUSE_LEFT_UP:
				MouseState.LeftButtonDown = false;
				break;

			case EMIE_MOUSE_MOVED:
				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				break;

			default:
				break;
			}
		}
		return false;
	}
};

void updatePhysics(IMeshSceneNode* rocket, f32 dt) {
}

int main()
{
	SIrrlichtCreationParameters params;
	params.DriverType = EDT_OPENGL;
	params.WindowSize = core::dimension2d<u32>(800, 600);
	params.AntiAlias = 8;
	IrrlichtDevice* device =
		createDeviceEx(params);
	if (!device)
		return 1;

	device->setWindowCaption(L"Rocket Simulator");
	device->setResizable(true);

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();
	IGUISkin* skin = guienv->getSkin();
	IGUIFont* font = guienv->getFont("../resources/fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);

	skin->setFont(guienv->getBuiltInFont(), EGDF_TOOLTIP);

	EventReceiver receiver;
	device->setEventReceiver(&receiver);

	IMesh* mesh = smgr->getMesh("../resources/rocket.obj");
	if (!mesh) {
		device->drop();
		return 1;
	}
	IMeshSceneNode* rocket = smgr->addMeshSceneNode(mesh);
	if (rocket) {
		rocket->setMaterialFlag(EMF_LIGHTING, false);
		rocket->setScale(vector3df(100.0f, 100.0f, 100.0f));
	}
	ICameraSceneNode* camera = smgr->addCameraSceneNode(0, vector3df(0, 30, -40), vector3df(0, 5, 0));

	/*IAnimatedMesh* mesh = smgr->getMesh("../resources/sydney.md2");
	if (!mesh) {
		device->drop();
		return 1;
	}
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (node) {
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(EMAT_STAND);
		node->setMaterialTexture(0, driver->getTexture("../resources/sydney.bmp"));
	}
	smgr->addCameraSceneNode(0, vector3df(0, 30, -40), vector3df(0, 5, 0));*/

	u32 then = device->getTimer()->getTime();
	const f32 MOVEMENT_SPEED = 20.f;

	while (device->run() && driver) {
		const u32 now = device->getTimer()->getTime();
		const f32 dt = (f32)(now - then) / 1000.f;
		then = now;

		updatePhysics(rocket, dt);

		driver->beginScene(true, true, SColor(0, 0, 0, 0));
		smgr->drawAll();
		driver->endScene();
		
	}

	device->drop();

	return 0;
}
