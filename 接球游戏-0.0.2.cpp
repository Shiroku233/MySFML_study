#include <SFML/Graphics.hpp>
#include<vector>
#include<cstdlib>
#include<ctime>
#include<string>

enum class GameState
{
    MENU,
    PLAYING,
    GAME_OVER
};

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({800,600}),"接球游戏");

    sf::RectangleShape paddle({100,20});
    paddle.setFillColor(sf::Color::Black);
    paddle.setPosition({350,550});

    float paddleSpeed=800.f;
    
    std::vector<sf::CircleShape> balls;
    std::vector<sf::Vector2f> ballSpeeds;

    float ballRadius=15.f;
    int score=0;
    //-- 状态管理 --
    GameState currentState=GameState::MENU;

    //-- 菜单相关 --
    sf::RectangleShape playButton({200,60});
    playButton.setFillColor(sf::Color::Green);
    playButton.setPosition({300,270});
    //开始按钮：绿色矩形

    sf::RectangleShape quitButton({200,60});
    quitButton.setFillColor(sf::Color::Red);
    quitButton.setPosition({300,370});
    //退出按钮：红色矩形


    //-- 游戏时钟 --
    sf::Clock clock;
    float spawnTimer=0.f;
    float spawnInterval=1.f;
    

    window.setFramerateLimit(60);

    while(window.isOpen())
    {
        float deltaTime=clock.restart().asSeconds();

        //-- 事件处理 --

        while(const std::optional event=window.pollEvent())
        {
            //窗口关闭事件
            if(event->is<sf::Event::Closed>()) window.close();

            //鼠标点击事件
            if(auto* mousePressed=event->getIf<sf::Event::MouseButtonPressed>())
            {
                if(mousePressed->button==sf::Mouse::Button::Left)
                {
                    sf::Vector2f mousePos=window.mapPixelToCoords(mousePressed->position);
                    if(currentState==GameState::MENU)
                    {
                        if(playButton.getGlobalBounds().contains(mousePos))
                        {
                            currentState=GameState::PLAYING;
                            score=0;
                            balls.clear();
                            ballSpeeds.clear();
                            spawnTimer=0.f;
                            paddle.setPosition({350,550});
                        }
                        else if(quitButton.getGlobalBounds().contains(mousePos))
                        {
                            window.close();
                        }
                        
                    }
                }
            }
            //键盘事件
            if(auto* keyPressed=event->getIf<sf::Event::KeyPressed>())
            {
                if(currentState==GameState::PLAYING)
                {
                    if(keyPressed->code==sf::Keyboard::Key::Escape)
                    {
                        currentState=GameState::MENU;
                    }
                }
                else if(currentState==GameState::GAME_OVER)
                {
                    if(keyPressed->code==sf::Keyboard::Key::Escape)
                    {
                        score=0;
                        balls.clear();
                        ballSpeeds.clear();
                        spawnTimer=0.f;
                        paddle.setPosition({350,550});
                        currentState=GameState::MENU;
                    }
                }
            }   
        }
        //-- 根据状态更新游戏逻辑 --
        if(currentState==GameState::PLAYING)
        {
            //-- 游戏进行中 --

            //生成新球
            spawnTimer+=deltaTime;
            if(spawnTimer>=spawnInterval)
            {
                spawnTimer=0.f;
                sf::CircleShape newBall(ballRadius);
                newBall.setFillColor(sf::Color::Red);

                float x=ballRadius+(std::rand()%(800-2*static_cast<int>(ballRadius)));
                newBall.setPosition({x,ballRadius});

                sf::Vector2f speed(
                    (std::rand()%200-100)/50.f,
                    200.f+std::rand()%100
                );
                balls.push_back(newBall);
                ballSpeeds.push_back(speed);
            }
            
            //处理移动板子
            sf::Vector2f paddleMove{0,0};
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) paddleMove.x-=paddleSpeed*deltaTime;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) paddleMove.x+=paddleSpeed*deltaTime;

            paddle.move(paddleMove);

            //限制板子范围
            sf::Vector2f paddlePos=paddle.getPosition();
            if(paddlePos.x<0) paddlePos.x=0;
            if(paddlePos.x>800-100) paddlePos.x=800-100;
            paddle.setPosition(paddlePos);

            //生成小球逻辑
            for(size_t i=0;i<balls.size();)
            {
                balls[i].move(ballSpeeds[i]*deltaTime);

                sf::Vector2f ballPos=balls[i].getPosition();

                //检测与板子碰撞
                sf::FloatRect ballBounds=balls[i].getGlobalBounds();
                sf::FloatRect paddleBounds=paddle.getGlobalBounds();
                
                if(ballBounds.findIntersection(paddleBounds))
                {
                    balls.erase(balls.begin()+i);
                    ballSpeeds.erase(ballSpeeds.begin()+i);
                    score+=10;

                    continue;
                }
                if(ballPos.y>600)
                {
                    currentState=GameState::GAME_OVER;
                    break;
                }
                if(ballPos.x<0||ballPos.x>800-ballRadius*2) ballSpeeds[i].x*=-1;
                i++;
            }
        }


        //-- 绘制所有状态 --
        window.clear(sf::Color::White);
        
        
        if(currentState==GameState::MENU)
        {
            //绘制菜单
            window.draw(playButton);
            window.draw(quitButton);

            sf::RectangleShape menuTitle({400,50});
            menuTitle.setFillColor(sf::Color::Yellow);
            menuTitle.setPosition({200,150});
            window.draw(menuTitle);
        }
        else if(currentState==GameState::PLAYING)
        {
            //绘制游戏画面
            for(const auto& ball:balls)
            {
                window.draw(ball);
            }
            window.draw(paddle);

            for(int i=0;i<score/10;i++)
            {
                sf::RectangleShape scoreBlock({5,10});
                scoreBlock.setFillColor(sf::Color::Green);
                scoreBlock.setPosition({(10+(i*8)),10});
                window.draw(scoreBlock);
            }
        }
        else if(currentState==GameState::GAME_OVER)
        {
            //绘制游戏结束画面
            sf::RectangleShape overlay({800,600});
            overlay.setFillColor(sf::Color(255,0,0,100));
            window.draw(overlay);
            //游戏结束显示：全屏红色透明

            sf::RectangleShape gameOverText({400,60});
            gameOverText.setFillColor(sf::Color::White);
            gameOverText.setPosition({200,270});
            window.draw(gameOverText);
            //游戏结束提示文本（暂时代替）：中心白色矩形

        }

        window.display();
    }
    return 0;
}