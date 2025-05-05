/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "HelloWorldScene.h"
#include "audio/include/AudioEngine.h"

using namespace cocos2d;
using namespace std;

USING_NS_CC;

static char arr[13][23];
const int TILE_SIZE = 40;

// on initialization, create a scene and add a layer to it
Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();

    // Create a solid color layer (RGBA)
    auto background = LayerColor::create(Color4B(49, 142, 34, 255)); // ground colour

    scene->addChild(background, -1); // z-order -1 to be behind everything

    auto layer = HelloWorld::create();
    scene->addChild(layer);
    
    AudioEngine::play2d("audio/bgm.mp3", true);

    return scene;
}


// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }


    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    score = 0; // initialize score

    scoreLabel = Label::createWithTTF("Score: 0", "fonts/PressStart2P-Regular.ttf", 24);
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->setPosition(Vec2(origin.x + 150, origin.y + visibleSize.height - 30));
    this->addChild(scoreLabel, 10); 


    timeRemaining = 150; 
    timerLabel = Label::createWithTTF("Time: 150", "fonts/PressStart2P-Regular.ttf", 24);
    timerLabel->setPosition(Vec2(origin.x + visibleSize.width - 150 , origin.y + visibleSize.height - 30)); 
    this->addChild(timerLabel, 10);

    // Schedule the timer to update every second
    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorld::updateTimer), 1.0f);


    const int ROWS = 13;
    const int COLS = 23;

	//Storing the grid in a 2D array
    for (int y = 0; y < ROWS; y++) 
    {
        for (int x = 0; x < COLS; x++) 
        {
            arr[y][x] = '0';
        }
    }


    //Placing solid walls
    for (int y = 0; y < ROWS; y++) 
    {
        for (int x = 0; x < COLS; x++) 
        {
            arr[y][x] = '0';
            if (x == 0 || y==0 || y==ROWS-1 || x==COLS-1 || (x%2==0 && y%2==0))
            {
                auto tile = Sprite::create("block.png");
                tile->setPosition(Vec2(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2));
                this->addChild(tile,3);
				arr[y][x] = '2';
            }
			

        }
    }


	//to store the coordinates of the door and grid placements
    ofstream log("log.txt");


    // Randomly place 70 destructible walls
    int i = 1;
	srand(time(0)); //seeding the random number generator
    while (i<=70)
    {
		int x = rand() % COLS;
		int y = rand() % ROWS;
		if (arr[y][x]!='0' || (x==1 && y==1) || (x==2 && y==1) || (x==1 && y==2))
		{
			continue;
		}
		else
		{
            if (i==1)
            {
                //Placing the door
				arr[y][x] = '5';
				log << "Door: " << y << ", " << x << endl;
            }
            else
            {
				arr[y][x] = '1';
            }
			i++;
            auto wall=Sprite::create("wall.png");
            wall->setPosition(Vec2(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2));
            this->addChild(wall, -1);
		}
    }


	//Randomly placing the enemies and making them move in one direction after they hit a wall
    enemies.clear();
    for (int i = 0; i < 10; ++i)
    {
        auto enemy = Sprite::create("monster3.png");
        int x = rand() % COLS;
        int y = rand() % ROWS;
        while (arr[y][x] != '0')
        {
            x = rand() % COLS;
            y = rand() % ROWS;
        }
        enemy->setPosition(Vec2(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2));
        this->addChild(enemy, 2);
        
        if (i < 5)
        {
            int direction = 1;
            auto movement = CallFunc::create([enemy, direction]() mutable {
                int dx = direction * TILE_SIZE;
                int dy = 0;
                int x = enemy->getPositionX() / TILE_SIZE;
                int y = enemy->getPositionY() / TILE_SIZE;


                if (arr[y][x + direction] == '0')
                {
                    Vec2 moveBy = Vec2(dx, dy);
                    enemy->runAction(MoveBy::create(1.0f, moveBy));
                }
                else if (arr[y][x + direction] == '1' || arr[y][x + direction] == '2')
                {
                    direction = -direction; // turn around
                }
                });
            auto moveLoop = RepeatForever::create(Sequence::create(
                movement,
                DelayTime::create(1.0f),
                nullptr
            ));
            enemy->runAction(moveLoop);
        }
        else
        {
            int direction = 1;
            auto movement = CallFunc::create([enemy, direction]() mutable {
                int dx = 0;
                int dy = direction * TILE_SIZE;
                int x = enemy->getPositionX() / TILE_SIZE;
                int y = enemy->getPositionY() / TILE_SIZE;


                if (arr[y + direction][x] == '0')
                {
                    Vec2 moveBy = Vec2(dx, dy);
                    enemy->runAction(MoveBy::create(1.0f, moveBy));
                }
                else if (arr[y + direction][x] == '1' || arr[y + direction][x] == '2')
                {
                    direction *= -1; // turn around
                }
                });
            auto moveLoop = RepeatForever::create(Sequence::create(
                movement,
                DelayTime::create(1.0f),
                nullptr
            ));
            enemy->runAction(moveLoop);
        }
        enemies.push_back(enemy);
        
    }
    

	// Printing the array to a file
    for (int i = 0; i < 23; i++)
    {
        log << i << "\t";
    }
	log << endl;
    for (int i = 0; i < ROWS; i++) 
    {
        log << i;
        for (int j = 0; j < COLS; j++) {
            log << arr[i][j] << "\t";
        }
        log << endl;
    }


	//Placing the player
    try
    {
        player = Sprite::create("player.png");
        if (player == nullptr)
        {
            throw "player.png";

        }
		
    }
    catch (char* filename)
    {
        problemLoading("player.png");
		return false;
    }
    player->setPosition(Vec2(TILE_SIZE + TILE_SIZE / 2, TILE_SIZE + TILE_SIZE / 2));
    this->addChild(player,5);


	//creating the event listener for keyboard input
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    this->scheduleUpdate();

    return true;
}
  

// Function to handle key presses
void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) 
{
    int cols = 23;
	int rows = 13;
    Vec2 pos = player->getPosition();
    int a = pos.x;
    int b = pos.y;
    

    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) 
    {
		int c = (a - TILE_SIZE - (TILE_SIZE / 2)) / TILE_SIZE;
		int d = (b - (TILE_SIZE / 2)) / TILE_SIZE;

        if (arr[d][c] == '0' || arr[d][c]=='3')
        {
			pos.x -= 40;
        }
        
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) 
    {
        int c = (a + TILE_SIZE - (TILE_SIZE / 2)) / TILE_SIZE;
        int d = (b - (TILE_SIZE / 2)) / TILE_SIZE;

        if (arr[d][c] == '0' || arr[d][c] == '3')
        {
            pos.x += 40;
        }
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) 
    {
		int c = (a - (TILE_SIZE / 2)) / TILE_SIZE;
		int d = (b + TILE_SIZE - (TILE_SIZE / 2)) / TILE_SIZE;
        if (arr[d][c] == '0' || arr[d][c] == '3')
		{
			pos.y += 40;
		}
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) 
    {
		int c = (a - (TILE_SIZE / 2)) / TILE_SIZE;
		int d = (b - TILE_SIZE - (TILE_SIZE / 2)) / TILE_SIZE;
        if (arr[d][c] == '0' || arr[d][c] == '3')
		{
			pos.y -= 40;
		}
    }
    
    if (!player) return;


	// Check for collision with enemies
    for (auto enemy : enemies)
    {
		Vec2 ePos = enemy->getPosition();
        if (fabs(ePos.x - pos.x) < TILE_SIZE * 0.5f && fabs(ePos.y - pos.y) < TILE_SIZE * 0.5f)
        {
            gameOver();
            break;
        }
    }
    

	//Bomb placement and going through the door
    if (keyCode == EventKeyboard::KeyCode::KEY_SPACE) {
        // Place a bomb
		int c = (pos.x - TILE_SIZE / 2) / TILE_SIZE;
		int d = (pos.y - TILE_SIZE / 2) / TILE_SIZE;
        if (arr[d][c] == '3' && enemies.empty())
        {
			playerWins();
        }
        else
        {
            auto bomb = Sprite::create("bomb.png");
            bomb->setPosition(pos);
            this->addChild(bomb, 2);

            // Explode after delay
            this->runAction(Sequence::create(
                DelayTime::create(1.5f),
                CallFunc::create([=]() {
                    // Add explosion logic here
                    bomb->removeFromParent();
                    bomb_explode(pos);
                    }),
                nullptr
            ));
        }
        
    }


    player->setPosition(pos);
}


// Function to handle bomb explosion
void HelloWorld:: bomb_explode(Vec2 pos)
{ 
    auto parent = this; 

    auto explosion_mid = Sprite::create("explode_mid.png");
    auto explosion_down = Sprite::create("explode_end1.png");
    auto explosion_right = Sprite::create("explode_end2.png");
    auto explosion_up = Sprite::create("explode_end3.png");
    auto explosion_left = Sprite::create("explode_end4.png");
    

    explosion_mid->setPosition(pos);
    explosion_down->setPosition(Vec2(pos.x, pos.y - 40));
    explosion_up->setPosition(Vec2(pos.x, pos.y + 40));
    explosion_right->setPosition(Vec2(pos.x + 40, pos.y));
    explosion_left->setPosition(Vec2(pos.x - 40, pos.y));


    // Adding to parent node i.e. layer
    parent->addChild(explosion_mid,2);
    parent->addChild(explosion_down,1);
    parent->addChild(explosion_up,1);
    parent->addChild(explosion_right,1);
    parent->addChild(explosion_left,1);


    Vector<Sprite*> explosions = {
        explosion_mid, explosion_down, explosion_up,
        explosion_right, explosion_left
    };


    //fade and remove after 0.5s
    for (auto& sprite : explosions) {
        sprite->runAction(Sequence::createWithTwoActions(
            DelayTime::create(0.5f),
            RemoveSelf::create()
        ));
    }

	//Removing walls through the bomb explosion and updating score
	int c = (pos.x - TILE_SIZE/2) / 40;
	int d = (pos.y - TILE_SIZE / 2) / 40;
	if (arr[d+1][c] == '1' || arr[d+1][c]=='5')
	{
        if (arr[d + 1][c] == '1')
        {
            auto tile = Sprite::create("ground.png");
            tile->setPosition(Vec2((c)*TILE_SIZE + TILE_SIZE / 2, (d + 1) * TILE_SIZE + TILE_SIZE / 2));
            this->addChild(tile, -1);

        }
		if (arr[d + 1][c] == '5')
		{
            arr[d + 1][c] = '3';
			auto door = Sprite::create("door.png");
			door->setPosition(Vec2((c)*TILE_SIZE + TILE_SIZE / 2, (d + 1) * TILE_SIZE + TILE_SIZE / 2));
			this->addChild(door, 1);
		}
        else
        {
			arr[d + 1][c] = '0';
        }
        score += 10;
        scoreLabel->setString("Score: " + to_string(score));
	}
	if (arr[d - 1][c] == '1' || arr[d-1][c]=='5')
	{
        if (arr[d-1][c] == '1')
        {
            auto tile= Sprite::create("ground.png");
            tile->setPosition(Vec2((c)*TILE_SIZE + TILE_SIZE / 2, (d - 1) * TILE_SIZE + TILE_SIZE / 2));
            this->addChild(tile, -1);

        }
        
		if (arr[d - 1][c] == '5')
		{
            arr[d - 1][c] = '3';
			auto door = Sprite::create("door.png");
			door->setPosition(Vec2((c)*TILE_SIZE + TILE_SIZE / 2, (d - 1) * TILE_SIZE + TILE_SIZE / 2));
			this->addChild(door, 1);
		}
		else
		{
			arr[d - 1][c] = '0';
		}
		score += 10;
		scoreLabel->setString("Score: " + to_string(score));
	}
	if (arr[d][c + 1] == '1' || arr[d][c+1]=='5')
	{
        if (arr[d][c+1] == '1')
        {
            auto tile = Sprite::create("ground.png");
            tile->setPosition(Vec2((c+1)*TILE_SIZE + TILE_SIZE / 2, (d) * TILE_SIZE + TILE_SIZE / 2));
            this->addChild(tile, -1);

        }
        
		if (arr[d][c + 1] == '5')
		{
            arr[d][c + 1] = '3';
			auto door = Sprite::create("door.png");
			door->setPosition(Vec2((c + 1) * TILE_SIZE + TILE_SIZE / 2, (d)*TILE_SIZE + TILE_SIZE / 2));
			this->addChild(door, 1);
		}
		else
		{
			arr[d][c + 1] = '0';
		}
        score += 10;
        scoreLabel->setString("Score: " + to_string(score));
	}
	if (arr[d][c - 1] == '1' || arr[d][c-1]=='5')
	{
        if (arr[d][c-1] == '1')
        {
            auto tile = Sprite::create("ground.png");
            tile->setPosition(Vec2((c-1)*TILE_SIZE + TILE_SIZE / 2, (d) * TILE_SIZE + TILE_SIZE / 2));
            this->addChild(tile, -1);

        }
		if (arr[d][c - 1] == '5')
		{
            arr[d][c - 1] = '3';
			auto door = Sprite::create("door.png");
			door->setPosition(Vec2((c - 1) * TILE_SIZE + TILE_SIZE / 2, (d)*TILE_SIZE + TILE_SIZE / 2));
			this->addChild(door, 1);
		}
		else
		{
			arr[d][c - 1] = '0';
		}
        score += 10;
        scoreLabel->setString("Score: " + to_string(score));
	}


    // Kill enemies in explosion range
    for (auto it = enemies.begin(); it != enemies.end(); )
    {
        auto enemy = *it;
        Vec2 ePos = enemy->getPosition();
        float ex = ePos.x;
        float ey = ePos.y;

        // If enemy is within explosion center or 1 tile in any direction
        if ((fabs(ex - pos.x) < TILE_SIZE * 0.5f && fabs(ey - pos.y) < TILE_SIZE * 1.1f) ||
            (fabs(ey - pos.y) < TILE_SIZE * 0.5f && fabs(ex - pos.x) < TILE_SIZE * 1.1f))
        {
            enemy->removeFromParent();
            it = enemies.erase(it); // remove from vector
            score += 100;
            scoreLabel->setString("Score: " + to_string(score));
        }
        else
        {
            ++it;
        }
    }


	// Check for player collision with explosions
    Vec2 playerPos = player->getPosition();
    for (auto fire : explosions)
    {
        Vec2 firePos = fire->getPosition();
        if (fire && player && firePos.distance(playerPos) < TILE_SIZE * 0.5f)
        {
            gameOver();
            break;
        }
    }

}


// Function to update the timer
void HelloWorld::updateTimer(float dt)
{
    timeRemaining--;
    if (timeRemaining <= 0)
    {
        timeRemaining = 0;
        this->unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::updateTimer));

        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();

        CCLOG("Time's up!");
        
        auto gameOverLabel = Label::createWithTTF("Time's UP!!", "fonts/PressStart2P-Regular.ttf", 60);
        gameOverLabel->setTextColor(Color4B::BLACK);
        gameOverLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
        this->addChild(gameOverLabel, 999);


        this->runAction(Sequence::create(
            DelayTime::create(2.0f),
            CallFunc::create([this, gameOverLabel]() {
				gameOverLabel->removeFromParent();
                this->gameOver();
                }),
            nullptr
        ));
    }
    timerLabel->setString("Time: " + std::to_string(timeRemaining));
}


// Function to handle player winning
void HelloWorld::playerWins()
{
    // Stop all audio and play win sound
    AudioEngine::stopAll();
    AudioEngine::play2d("audio/win.mp3");


    CCLOG("You Win!");


    // Stop game updates
    this->unscheduleUpdate();
    this->unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::updateTimer));


    // Display "You Win!!" label
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto winLabel = Label::createWithTTF("You Win!!", "fonts/PressStart2P-Regular.ttf", 60);
    winLabel->setTextColor(Color4B::BLACK);
    winLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(winLabel, 999);


    // Transition to the starting scene after delay
    this->runAction(Sequence::create(
        DelayTime::create(10.0f),
        CallFunc::create([]() {

            auto scene = HelloWorld::createScene(); // Replace with MenuScene::createScene() if needed
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
            }),
        nullptr
    ));
}


// Function to handle game over
void HelloWorld::gameOver()
{
    AudioEngine::stopAll();
    AudioEngine::play2d("audio/gameover.mp3");


    CCLOG("Game Over!");


    // Stop game updates
    this->unscheduleUpdate();
    this->unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::updateTimer));


    // Display "Game Over" label
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    auto gameOverLabel = Label::createWithTTF("Game Over(T T)!!", "fonts/PressStart2P-Regular.ttf", 50);
    gameOverLabel->setTextColor(Color4B::BLACK);
    gameOverLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(gameOverLabel, 999); 


    // Transition to the starting scene after delay
    this->runAction(Sequence::create(
        DelayTime::create(5.0f),
        CallFunc::create([]() {
            auto scene = HelloWorld::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
            }),
        nullptr
    ));
}


// Function to handle close menu item
void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
}