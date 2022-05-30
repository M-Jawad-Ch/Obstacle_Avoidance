#include<vector>
#include<fstream>
#include<iostream>
#include<chrono>

#include<SFML/Graphics.hpp>
#include"Headers/agent.hpp"
#include"Headers/dataSet.hpp"
#include"Headers/helper.hpp"

const float PI = 22.0 / 7.0;
const float STEPS_PER_SECOND = 100;
const float RADIANS_PER_SECOND =  PI / 2.0;
const float AGENT_SIZE = 5;

const int RAY_COUNT = 32;
const int MAX_DISTANCE = 60;
const int AGENT_COUNT = 10;
const int LAYER_COUNT = 2;

const int SECOND_LAYER_SIZE = 8;
const int CLASSES = 3;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

int main()
{
    sf::VertexArray path, entities;
    std::vector<Edge> edges;
    sf::Vector2f spawn;

    load_Path(edges, path, spawn);

    sf::ContextSettings settings; settings.antialiasingLevel = 4.0;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Renderer", sf::Style::Default, settings);
    sf::Event event;

    std::vector<Agent> Agents;
    for(int i = 0; i < AGENT_COUNT; i++)
    {
        Agent agent( LAYER_COUNT , spawn, AGENT_SIZE, RAY_COUNT, window, MAX_DISTANCE);
        agent.brain.layers[0] = Layer(SECOND_LAYER_SIZE, RAY_COUNT);
        agent.brain.layers[1] = Layer(CLASSES, SECOND_LAYER_SIZE);

        Agents.push_back(agent);
    }

    Matrix input = Matrix(RAY_COUNT, 1, 1);
    float actions[ CLASSES ] = {0};
    bool pathVisible = true;

    window.setFramerateLimit(60);

    sf::VertexArray Agent_Bodies, Agent_Body;

    while(window.isOpen())
    {
        auto start = std::chrono::high_resolution_clock::now();

        while(window.pollEvent(event))
        {
            switch (event.type)
            {
                case ( sf::Event::Closed ):
                    window.close();
                    break;

                case ( sf::Event::KeyPressed ):
                {
                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) )
                    {
                        window.close();
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left) )
                    {
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x - 20, window.getView().getCenter().y ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
                    {
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x + 20, window.getView().getCenter().y ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
                    {
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x, window.getView().getCenter().y - 20 ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
                    {
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x, window.getView().getCenter().y + 20 ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::P) )
                    {
                        if ( pathVisible )
                        {
                            pathVisible = false;
                            std :: cout << " Path invisible\n";
                        }
                        else
                        {
                            pathVisible = true;
                            std :: cout << " Path visible\n";
                        }
                    }

                    break;
                }

            default:
                break;
            }
        }

        window.clear(sf::Color::Black);

        if ( pathVisible ) window.draw(path);

        for(int i = 0; i < AGENT_COUNT; i++)
        {
            if ( Agents[i].isAlive )
            {
                Agent_Body = Agents[i].drawable();

                for(int j = 0; j < Agent_Body.getVertexCount(); j++)
                {
                    Agent_Bodies.append( Agent_Body[j] );
                }

                Agent_Body.clear();
            }
        }

        Agent_Bodies.setPrimitiveType( sf::PrimitiveType::Lines );
        window.draw( Agent_Bodies ); Agent_Bodies.clear();
        window.display();

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;


        for(int i = 0; i < AGENT_COUNT; i++)
        {
            if ( Agents[i].touch(edges) ) Agents[i].isAlive = false;

            if ( Agents[i].isAlive )
            {
                getInputs( input, Agents[i], Agents[i].inter(edges) );

                switch ( Agents[i].brain.layers[ LAYER_COUNT - 1 ].output() )
                {
                    case 0:
                        Agents[i].translate( STEPS_PER_SECOND * elapsed.count() );
                        break;

                    case 1:
                        Agents[i].rotate( RADIANS_PER_SECOND * elapsed.count() );
                        break;

                    case 2:
                        Agents[i].rotate( - RADIANS_PER_SECOND * elapsed.count() );
                        break;
                }
            }
        }
    }
}