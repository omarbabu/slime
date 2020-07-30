// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <Box2D/Box2D.h>
#include <string>


namespace myapp {

class MyApp : public cinder::app::App {
 public:
  MyApp();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;
  void DrawBackground() const;
  void DrawPlayer() const;
  void DrawEnemy() const;
  void Reset();
  void DrawFinish() const;
  b2Body* makeEnemy(b2BodyDef enemyDef, b2Vec2 pos, float size) const;
  void updatePoints() const;

private:
    b2World* worldP;
};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
