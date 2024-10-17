#include <irrlicht.h>

#include "audio.hpp"
#include "vector3d.h"

using namespace irr;

class EventReceiver : public IEventReceiver {
public:
  bool KeyIsDown[KEY_KEY_CODES_COUNT];

  EventReceiver() {
    for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
      KeyIsDown[i] = false;
  }

  virtual bool OnEvent(const SEvent &event) {
    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
      KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
    }
    return false;
  }

  bool IsKeyDown(EKEY_CODE keyCode) const { return KeyIsDown[keyCode]; }
};

f32 thrust = 15000.0f;

void updatePhysics(scene::IMeshSceneNode *rocket, f32 dt) {
  const f32 GRAVITY = 9.81f;
  const f32 AIR_DENSITY = 1.225f;
  const f32 LIFT_COEFFICIENT = 0.5f;
  const f32 CORIOLIS_COEFFICIENT = 0.0001f;
  const f32 IGNITE_SPEED = 2000.f;

  core::vector3df position = rocket->getPosition();
  core::vector3df velocity = rocket->getRotation();

  thrust -= dt * IGNITE_SPEED;
  if (thrust < 0) thrust = 0;
  f32 rocketRadius = 0.5f;
  f32 mass = 500.0f;     // Example mass in kilograms
  f32 spinRate = 20.0f;  // Example spin rate in radians per second
  core::vector3df wind(5.0f, 0.0f, -3.0f);
  core::vector3df relativeWind = velocity - wind;

  core::vector3df thrustForce(0, thrust, 0);
  core::vector3df gravityForce(0, -mass * GRAVITY, 0);
  core::vector3df magnusForce = 0.5f * AIR_DENSITY * spinRate * rocketRadius *
                                velocity.crossProduct(relativeWind);
  core::vector3df liftForce = LIFT_COEFFICIENT * AIR_DENSITY *
                              relativeWind.getLengthSQ() *
                              relativeWind.crossProduct(velocity).normalize();
  core::vector3df coriolisForce =
      CORIOLIS_COEFFICIENT * mass *
      velocity.crossProduct(core::vector3df(0, 1, 0));
  core::vector3df totalForce =
      thrustForce + gravityForce + magnusForce + liftForce + coriolisForce;
  core::vector3df acceleration = totalForce / mass;

  velocity += acceleration * dt;
  position += velocity * dt;

  const f32 MOVEMENT_SPEED = 20.f;
  rocket->setPosition(position);
  rocket->setRotation(velocity);
}

void updateCamera(scene::ICameraSceneNode *camera, EventReceiver &receiver,
                  f32 dt) {
  const f32 MOVEMENT_SPEED = 100.f;
  const f32 LOOK_SPEED = 40.f;

  core::vector3df camTarget = camera->getTarget();
  core::vector3df camPosition = camera->getPosition();
  core::vector3df forward = (camTarget - camPosition).normalize();
  core::vector3df right =
      forward.crossProduct(camera->getUpVector()).normalize();

  if (receiver.IsKeyDown(KEY_KEY_W)) {
    camPosition += forward * MOVEMENT_SPEED * dt;
    camTarget += forward * MOVEMENT_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_KEY_A)) {
    camPosition += right * MOVEMENT_SPEED * dt; // Left
    camTarget += right * MOVEMENT_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_KEY_R)) {
    camPosition -= forward * MOVEMENT_SPEED * dt; // Backward
    camTarget -= forward * MOVEMENT_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_KEY_S)) {
    camPosition -= right * MOVEMENT_SPEED * dt; // Right
    camTarget -= right * MOVEMENT_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_UP)) {
    camTarget.Y += LOOK_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_DOWN)) {
    camTarget.Y -= LOOK_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_LEFT)) {
    camTarget.X -= LOOK_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_RIGHT)) {
    camTarget.X += LOOK_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_SPACE)) {
    camPosition.Y += MOVEMENT_SPEED * dt;
    camTarget.Y += MOVEMENT_SPEED * dt;
  }
  if (receiver.IsKeyDown(KEY_CONTROL)) {
    camPosition.Y -= MOVEMENT_SPEED * dt;
    camTarget.Y -= MOVEMENT_SPEED * dt;
  }

  camera->setPosition(camPosition);
  camera->setTarget(camTarget);
}
int main() {
  // audio::play("resources/321_ignition.wav");
  audio::loop::start("resources/rocket_sound.wav");

  SIrrlichtCreationParameters params;
  params.DriverType = video::EDT_OPENGL;
  params.WindowSize = core::dimension2d<u32>(800, 600);
  params.AntiAlias = 8;
  IrrlichtDevice *device = createDeviceEx(params);
  if (!device)
    return 1;

  device->setWindowCaption(L"Rocket Simulator");
  device->setResizable(true);

  video::IVideoDriver *driver = device->getVideoDriver();
  scene::ISceneManager *smgr = device->getSceneManager();

  EventReceiver receiver;
  device->setEventReceiver(&receiver);

  // Hide the mouse cursor
  device->getCursorControl()->setVisible(false);

  scene::IMesh *mesh = smgr->getMesh("resources/rocket.obj");
  if (!mesh) {
    device->drop();
    return 1;
  }
  scene::IMeshSceneNode *rocket = smgr->addMeshSceneNode(mesh);
  if (rocket) {
    rocket->setMaterialFlag(video::EMF_LIGHTING, false);
    rocket->setScale(core::vector3df(100.0f, 100.0f, 100.0f));
  }
  for (int i = 0; i < 10; ++i) {
    scene::ISceneNode *cloud =
        smgr->addSphereSceneNode(5.0f, 16, smgr->getRootSceneNode());
    if (cloud) {
      // Set position of cloud
      cloud->setPosition(core::vector3df(rand() % 200 - 100, rand() % 50 + 300,
                                         rand() % 200 - 100));
      cloud->setMaterialFlag(video::EMF_LIGHTING, false);
      cloud->setMaterialType(video::EMT_SOLID);    // Set material type to solid
      video::SMaterial &material = cloud->getMaterial(0); // Get the first material
      material.DiffuseColor = video::SColor(255, 192, 192, 192); // Set color to gray
      material.AmbientColor =
          video::SColor(255, 192, 192, 192);              // Set ambient color to gray
      material.EmissiveColor = video::SColor(0, 0, 0, 0); // No emission
      material.SpecularColor = video::SColor(0, 0, 0, 0); // No specular reflection
      material.Shininess = 0.0f;                   // Not shiny
    }
  }

  scene::ICameraSceneNode *camera = smgr->addCameraSceneNode(
      0, core::vector3df(0, 30, -40), core::vector3df(0, 5, 0));

  u32 then = device->getTimer()->getTime();

  while (device->run() && driver) {
    const u32 now = device->getTimer()->getTime();
    const f32 dt = (f32)(now - then) / 1000.f;
    then = now;

    updateCamera(camera, receiver, dt);
    updatePhysics(rocket, dt);

    // Render the scene
    driver->beginScene(true, true, video::SColor(10, 10, 90, 255));
    smgr->drawAll();
    driver->endScene();
  }

  device->drop();
  audio::loop::stop();
  return 0;
}
