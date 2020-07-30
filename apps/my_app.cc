// Copyright (c) 2020 CS126SP20. All rights reserved.
#include "my_app.h"
#include <cinder/Font.h>
#include <cinder/Vector.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/audio/audio.h>
#include <stdlib.h>
#include <ostream>
#include <Box2D/Common/b2Settings.h>
#include <Box2D/Box2D.h>

namespace myapp {
    using cinder::Color;
    using cinder::ColorA;
    using cinder::Rectf;

    //audio
    using cinder::audio::VoiceRef;
    using cinder::audio::SourceFile;
    using cinder::audio::Voice;
    using cinder::audio::SourceFileRef;
    //--
    using cinder::TextBox;
    using namespace std;

    VoiceRef bgMusic;

    bool playerJumping = false;
    bool playerRunning = false;
    bool playerHit = false;
    bool playerAttacking = false;
    bool gameRunning = false;
    float bgLoc;
    int hp;
    float playerPosX;
    float playerPosY;
    int spellsLeft;
    float imagePos = 1;
    float time_left = 7;
    const float PLAYER_SIZE = 64.0f;
    float points = 0;
    //box2D
    b2Vec2 grav(0.0f, 4000.0f);//Not Earth's gravity, positive grav = down
    b2World world(grav);
    map<std::string, b2Body *> bodyMap;
    std::list<b2Body *> enemyList;
    map<std::string, b2BodyDef> bodyDefMap;
    int numEnemies;
    std::list<b2Body *> toDestroy; // list of all enemies to destroy
    //--

    MyApp::MyApp() {}

    void MyApp::setup() {
        cinder::gl::enableDepthWrite();
        cinder::gl::enableDepthRead();
        cinder::audio::SourceFileRef bgFile = cinder::audio::load(
                cinder::loadFile(
                        "C:\\Users\\omarm\\OneDrive\\Desktop\\Cinder\\cinder_0.9.2_vc2015\\my-projects\\snake-obabu2\\resources\\GKGS_Freestyle.wav"));
        bgMusic = cinder::audio::Voice::create(bgFile);
        bgMusic->setVolume(15.0);
        bgMusic->start();
        playerPosX = 50;
        playerPosY = getWindowHeight() -
                     (4 * PLAYER_SIZE);
        gameRunning = true;
        bgLoc = 0;
        hp = 5;
        spellsLeft = 2;

// Box2D Setup
        worldP = &world;
        b2BodyDef groundDef;
        groundDef.position.Set(getWindowWidth() / 2,
                               getWindowHeight() - PLAYER_SIZE);
        b2Body *groundBody = world.CreateBody(&groundDef);
        b2PolygonShape groundShape;
        groundShape.SetAsBox(getWindowWidth(),
                             30.0f);    //straight from the documentation
        groundBody->CreateFixture(&groundShape, 0.0f);

//Making the player
        b2BodyDef playerDef;
        playerDef.type = b2_dynamicBody;    //this will be a dynamic body
        playerDef.position.Set(3 * PLAYER_SIZE,
                               getWindowHeight() - 4 * PLAYER_SIZE);
        b2Body *playerBody = world.CreateBody(&playerDef);
        bodyMap.insert(std::pair<std::string, b2Body *>("player", playerBody));
        bodyDefMap.insert(
                std::pair<std::string, b2BodyDef>("player", playerDef));

//Making first enemy
        b2BodyDef enemyDef;
        enemyDef.type = b2_dynamicBody;    //this will be a dynamic body
        enemyDef.position.Set(getWindowWidth() / 2,
                              getWindowHeight() - 2 * PLAYER_SIZE);
        b2Vec2 enemyPos = {400.0f,
                           getWindowHeight() - 2 * PLAYER_SIZE
        };
        b2Body *enemyBody = makeEnemy(enemyDef, enemyPos, 12.0f);
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(PLAYER_SIZE,
                            PLAYER_SIZE);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.friction = 0.3f;
        fixtureDef.density = 1.0f;

//need fixture for collisions
        playerBody->CreateFixture(&fixtureDef);
    }    //setup

    void MyApp::update() {
        b2Body *player = bodyMap.at("player");
        world.Step(1.0f / 60.0f, 35, 50);
//deleting enemies
        if (!toDestroy.empty()) {
            for (auto it = toDestroy.begin(); it != toDestroy.end(); ++it) {
                b2Body *bodyToDestroy = *it;
                world.DestroyBody(bodyToDestroy);
                enemyList.remove(*it);
            }
            toDestroy.clear();
        }
//background scrolling
        if (bgLoc >= getWindowBounds().x2) bgLoc = 0;
        if (abs(player->GetLinearVelocity().x) <= 0.5f)
            playerRunning = false;    //information for sprite display
        bgLoc += (bodyMap.at("player")->GetLinearVelocity().x) / (128);
//reset to idle animation when player on ground
        if (player->GetPosition().y >=
            getWindowHeight() - 3 * PLAYER_SIZE)
            playerJumping = false;
//check for collisions between player and enemies
        for (b2Body *enemy: enemyList) {
            for (b2ContactEdge *edge = enemy->GetContactList(); edge; edge = edge->next) {
                if ((edge->contact->GetFixtureA()->GetBody() == player ||
                     edge->contact->GetFixtureB()->GetBody() == player) &&
                    edge->contact->IsTouching()) {
                    if (playerAttacking) {
                        points += 150.0f;
                        toDestroy.push_back(enemy);
                    } else {
                        if (hp <= 0) {
                            cinder::gl::color(1.0, 0.975, 0.975);
                            playerHit = true;
                        } else {
                            points -= 5;
                            hp -= 1;
                        }
                    }
                }
//don't change enemy movement if in the air
                if (enemy->GetPosition().y <
                    getWindowHeight() - 2 * PLAYER_SIZE) {
                    continue;
                }
                float impulse = 0;
//move enemy toward player
                if (player->GetPosition().x - enemy->GetPosition().x >
                    50.0f)
                    impulse = ((enemy->GetMass() + 2) * 50);
                else if (player->GetPosition().x - enemy->GetPosition().x <
                         -50.0f)
                    impulse = -((enemy->GetMass() + 2) * 50);
                enemy->ApplyLinearImpulse(b2Vec2(impulse, 0),
                                          enemy->GetWorldCenter());    //from iforce2d.net
            }
        }

//points and spawning enemies
        updatePoints();
        int pointValue = ceil(points);
        if (pointValue % 40 == 0) {
            float enemyX = static_cast<float> (rand()) /
                           (static_cast<float> (RAND_MAX /
                                                getWindowWidth()));
            b2BodyDef enemy1Def;
            enemy1Def.position.Set(enemyX,
                                   4.0f);
            b2Vec2 enemyPos = enemy1Def.position;
            makeEnemy(enemy1Def, enemyPos, 12.0f);
        }
    } //update

    void MyApp::draw() {
        cinder::gl::enableAlphaBlending();
        if (!gameRunning) {
            return;
        }
        cinder::gl::clear();
        if (playerHit) {
            DrawFinish();
        }
        DrawEnemy();
        cinder::gl::color(1, 1, 1);
        DrawPlayer();
        DrawBackground();

    }    //draw

    void MyApp::DrawBackground() const {
//loading and drawing background
        cinder::gl::drawSolidRect(Rectf(0,
                                        getWindowHeight() - PLAYER_SIZE,
                                        getWindowWidth(),
                                        getWindowHeight()));
        cinder::gl::Texture2dRef bgTexture = ci::gl::Texture2d::create(
                loadImage(cinder::app::loadAsset("blackBg.jpg")));
        cinder::gl::Texture2dRef nextBgTexture = ci::gl::Texture2d::create(
                loadImage(cinder::app::loadAsset("blackBg.jpg")));
        Rectf bgRect = {getWindowBounds().x1 - bgLoc,
                        static_cast<float> (getWindowBounds().y1),
                        getWindowBounds().x2 - bgLoc,
                        static_cast<float> (getWindowBounds().y2)
        };
        Rectf nextBgRect = {2 * getWindowBounds().x1 - bgLoc,
                            static_cast<float> (getWindowBounds().y1),
                            2 * getWindowBounds().x2 - bgLoc,
                            static_cast<float> (getWindowBounds().y2)
        };
        cinder::gl::draw(bgTexture, bgRect);
        cinder::gl::draw(nextBgTexture, nextBgRect);
    }    //DrawBackground

    void MyApp::DrawPlayer() const {
        b2Body *player = bodyMap.at("player");
        //player filepath indexed with numbers, getting the number to load (for animation)
        imagePos += (0.22f);
        if (imagePos >= 6.6f) {
            imagePos = 1.0f;
        }
        std::string filepath = "3 SteamMan/SteamMan_idle_";
        int loc = floor(imagePos);
        std::string imgNum = std::to_string(loc) + ".png";

        if (playerAttacking) {
            filepath = "3 SteamMan/SteamMan_attack_";
            filepath.append(imgNum);

        } else if (playerJumping) {
            filepath = "3 SteamMan/SteamMan_jump_4.png";

        } else if (playerRunning) {
            filepath = "3 SteamMan/SteamMan_run_";
//left vs right movement
            if (player->GetLinearVelocity().x < 0) {
                filepath.append("left_");
            }
            filepath.append(imgNum);
        } else {
//idle only has 4 frames, adjust accordingly
            if (imagePos >= 4.4f) {
                imagePos = 1.0f;
            }
            loc = floor(imagePos);
            imgNum = std::to_string(loc) + ".png";
            filepath.append(imgNum);
        }
//loading image and drawing player
        cinder::gl::Texture2dRef playerTexture = ci::gl::Texture2d::create(
                loadImage(cinder::app::loadAsset(filepath)));
        cinder::gl::draw(playerTexture, Rectf(player->GetPosition().x,
                                              player->GetPosition().y,
                                              player->GetPosition().x +
                                              PLAYER_SIZE + 20.0f,
                                              player->GetPosition().y +
                                              PLAYER_SIZE + 20.0f));
    }    // DrawPlayer

    void MyApp::DrawEnemy() const {
//iterate through each enemy
        for (b2Body *enemy: enemyList) {
            b2Vec2 enemyPos = enemy->GetPosition();
            ci::vec2 circlePos(enemyPos.x, enemyPos.y);
//only one slime
            if (enemy == enemyList.front()) {
                cinder::gl::Texture2dRef slimeTexture = ci::gl::Texture2d::create(
                        loadImage(cinder::app::loadAsset("slime.png")));
                cinder::gl::draw(slimeTexture, Rectf(enemyPos.x - (PLAYER_SIZE),
                                                     enemyPos.y -
                                                     (PLAYER_SIZE * 1.5),
                                                     enemyPos.x + (PLAYER_SIZE),
                                                     enemyPos.y));
            } else {
                cinder::vec2 enemyLc = {enemyPos.x,
                                        enemyPos.y
                };
                cinder::gl::drawSolidCircle(enemyLc, 12.0f);
            }
        }
    }    //DrawEnemy

    void MyApp::keyDown(cinder::app::KeyEvent event) {
        b2BodyDef playerDef = bodyDefMap.at("player");
        b2Body *player = bodyMap.at("player");
        switch (event.getCode()) {
            case cinder::app::KeyEvent::KEY_UP: {
//attack button
                playerAttacking = true;
                break;
            }
            case cinder::app::KeyEvent::KEY_DOWN: {
//move player down
                playerAttacking = false;
                playerJumping = false;
                playerRunning = false;
                float impulse = (player->GetMass() + 850 * 2100);
               player->ApplyLinearImpulse(
                        b2Vec2(player->GetLinearVelocity().x,
                               impulse),
                        player->GetWorldCenter());    //from iforce2d.net
                // action here
                break;
            }

            case cinder::app::KeyEvent::KEY_LEFT: {
//move left
                playerAttacking = false;
                playerRunning = true;

                float impulse = -((player->GetMass() + 2500) *
                                  20000);
               player->ApplyLinearImpulse(b2Vec2(impulse,
                                                                player->GetLinearVelocity().y),
                                                         player->GetWorldCenter());
                break;
            }

            case cinder::app::KeyEvent::KEY_RIGHT: {
//move right
                playerAttacking = false;
                playerRunning = true;

                float impulse = ((player->GetMass() + 2500) *
                                 20000);
                player->ApplyLinearImpulse(b2Vec2(impulse,
                                                               player->GetLinearVelocity().y),
                                                         player->GetWorldCenter());
                break;
            }

            case cinder::app::KeyEvent::KEY_SPACE: {
//jump
                if (abs(player->GetLinearVelocity().y) <=
                    0.1f) {
                    // disables jumping in the air
                    playerJumping = true;
                    playerDef.position.Set(
                            player->GetPosition().x,
                            player->GetPosition().y -
                            250.0f);
                    world.DestroyBody(player);
                    const b2BodyDef *newPlayerDef = &playerDef;
                    player = world.CreateBody(newPlayerDef);
                    b2PolygonShape dynamicBox;
                    dynamicBox.SetAsBox(PLAYER_SIZE,
                                        PLAYER_SIZE);
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    fixtureDef.friction = 0.3f;
                    fixtureDef.density = 1.0f;
                    //same hitbox for player and enemy for now
                    player->CreateFixture(&fixtureDef);
                }

                break;
            }

            case cinder::app::KeyEvent::KEY_ESCAPE: {
                Reset();
            }

            case cinder::app::KeyEvent::KEY_CAPSLOCK: {
//superpower, kill all enemies except slime
                if (spellsLeft > 0) {
                    cinder::gl::color(0, 1, 1);
                    for (b2Body *enemy: enemyList) {
                        if (enemy != enemyList.front() &&
                            enemy->GetPosition().y <= player->GetPosition().y) {
                            playerAttacking = true;
                            toDestroy.push_back(enemy);
                        }
                    }
                }
                spellsLeft--;
                break;
            }
//pause game
            case cinder::app::KeyEvent::KEY_p: {
                gameRunning = !gameRunning;
            }
        }
    }    //keyDown

    void MyApp::Reset() {}    //Reset

    /**
     *Create an enemy and populate global lists
     *@param enemyDef definition of enemy body
     *@param pos position
     *@param size size of the enemy's "hitbox"
     *@return pointer to an enemy body
     */
    b2Body *MyApp::makeEnemy(b2BodyDef enemyDef, b2Vec2 pos, float size) const {
        enemyDef.type = b2_dynamicBody;    //this will be a dynamic body
        enemyDef.position.Set(pos.x, pos.y);
        b2Body *enemyBody = world.CreateBody(&enemyDef);
        b2PolygonShape dynamicBox;

        dynamicBox.SetAsBox(size / 2,
                            size / 2);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.friction = 0.1f;
        fixtureDef.density = 1.0f;
        enemyBody->CreateFixture(&fixtureDef);
        enemyList.push_back(enemyBody);
        std::string name = "enemy";
        std::string num = std::to_string(numEnemies);
        name.append(num);
        bodyMap.insert(std::pair<std::string, b2Body *>(name, enemyBody));
        bodyDefMap.insert(std::pair<std::string, b2BodyDef>(name, enemyDef));
        numEnemies++;
        return enemyBody;
    }

    void MyApp::updatePoints() const {
        points += (1.0f / 10.0f);
    }

    /**
     * Draw last stage of game, and print score when game is over
     */
    void MyApp::DrawFinish() const {
        string text;
        Color color;
        cinder::ivec2 size;
        cinder::vec2 loc;
        if (time_left <= 0) {
            cinder::gl::clear();
            gameRunning = false;
            color = {1,
                     1,
                     1
            };
            std::string endMessage = "Game Over! You scored ";
            std::string pointStr = std::to_string(floor(points));
            endMessage.append(pointStr).append(" points!");
            text = endMessage;
            size = {200,
                    100
            };
            loc = {getWindowWidth() / 2,
                   getWindowHeight() / 2
            };
        } else {
            text = std::to_string(time_left);
            color = {1 - (time_left / 10),
                     0,
                     0
            };
            time_left -= 1.5f / 60.0f;
            size = {50,
                    50
            };
            loc = {50,
                   50
            };
        }
        auto box = TextBox()
                .alignment(TextBox::CENTER)
                .font(cinder::Font("Arial", 20))
                .size(size)
                .color(color)
                .backgroundColor(ColorA(0, 0, 0, 0))
                .text(text);    // from the snake project
        const auto box_size = box.getSize();
        const cinder::vec2 locp = {loc.x - box_size.x / 2,
                                   loc.y - box_size.y / 2
        };
        const auto surface = box.render();
        const auto texture = cinder::gl::Texture::create(surface);
        cinder::gl::draw(texture, locp);
    }
}    // namespace