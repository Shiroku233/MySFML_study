#include <SFML/Graphics.hpp>
#include<vector>
#include<cstdlib>
#include<ctime>

int main()
{

    sf::RenderWindow window(sf::VideoMode({800,600}),"接球游戏");

    sf::RectangleShape paddle({100,20});
    paddle.setFillColor(sf::Color::Black);
    paddle.setPosition({350,550});

    float paddleSpeed=800.f;
    
    std::vector<sf::CircleShape> balls;
    std::vector<sf::Vector2f> ballSpeeds;

    float ballRadius=15.f;

    int score=0;
    bool gameOver=false;

    sf::Clock clock;
    float spawnTimer=0.f;
    float spawnInterval=1.f;
    

    window.setFramerateLimit(60);

    while(window.isOpen())
    {
        float deltaTime=clock.restart().asSeconds();
        spawnTimer+=deltaTime;

        while(const std::optional event=window.pollEvent())
        {
            if(event->is<sf::Event::Closed>()) window.close();
            if(gameOver) 
            {
                if(auto* keyPressed=event->getIf<sf::Event::KeyPressed>())
                {
                    if(keyPressed->code==sf::Keyboard::Key::Space)
                    {
                        gameOver=false;
                        score=0;
                        balls.clear();
                        ballSpeeds.clear();
                        spawnTimer=0.f;
                    }
                }
            }
        }
        if(!gameOver)
        {
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
            sf::Vector2f paddleMove{0,0};
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) paddleMove.x-=paddleSpeed*deltaTime;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) paddleMove.x+=paddleSpeed*deltaTime;

            paddle.move(paddleMove);

            sf::Vector2f paddlePos=paddle.getPosition();
            if(paddlePos.x<0) paddlePos.x=0;
            if(paddlePos.x>800-100) paddlePos.x=800-100;
            paddle.setPosition(paddlePos);

            for(size_t i=0;i<balls.size();i++)
            {
                balls[i].move(ballSpeeds[i]*deltaTime);

                sf::Vector2f ballPos=balls[i].getPosition();

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
                    gameOver=true;
                    break;
                }
                if(ballPos.x<0||ballPos.x>800-ballRadius*2) ballSpeeds[i].x*=-1;
            }
        }
        window.clear(sf::Color::White);
        
        for(const auto& ball:balls)
        {
            window.draw(ball);
        }
        window.draw(paddle);
        
        if(gameOver)
        {
            sf::RectangleShape overlay({800,600});
            overlay.setFillColor(sf::Color(0,0,0,200));
            window.draw(overlay);
        }
        window.display();
    }
    return 0;
}