#include<vector>
#include<fstream>
#include<iostream>
#include<chrono>

#include<SFML/Graphics.hpp>
#include"Headers/agent.hpp"
#include"Headers/dataSet.hpp"
#include"Headers/helper.hpp"

int main()
{
    float pi = 22.0 / 7.0, size = 5, maxDist = 500;
    float width = 800, height = 600;
    int rayCount = 180;

    sf::VertexArray path, entities;
    std::vector<Edge> edges;
    sf::Vector2f screen( width, height ), spawn;

    load_Path(edges, path, screen, spawn);

    sf::ContextSettings settings; settings.antialiasingLevel = 4.0;
    sf::RenderWindow window(sf::VideoMode(width, height), "Renderer", sf::Style::Default, settings);
    sf::Event event;

    std::string brainFile = "NeuralNets/first.bin";

    Agent agent(2, spawn, size, rayCount, window, maxDist);
    agent.brain.layers[0] = Layer(16, rayCount);
    agent.brain.layers[1] = Layer(3, 16);

    read( agent.brain, brainFile );

    Matrix input = Matrix(rayCount, 1, 1);
    float actions[3] = {0};
    sf::Vector2f target;
    bool Wpress = false, Apress = false, Dpress = false, testing = false, training = false;
    int index = 0, Epoch = 1;

    window.setFramerateLimit(60);

    DataSet dataSet("DataSet/dataSet.bin");

    dataSet.meta.layerCount = agent.brain.layerCount;
    dataSet.meta.rayCount = agent.rays.size();

    dataSet.meta.layerMeta.resize( dataSet.meta.layerCount );

    for(int i = 0; i < agent.brain.layerCount; i++)
    {
        dataSet.meta.layerMeta[i].rows = agent.brain.layers[i].Weights.rows;
        dataSet.meta.layerMeta[i].cols = agent.brain.layers[i].Weights.cols;
    }

    float steps_per_second = 100, angle_per_second = ( pi / 2 );

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
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x + 20, window.getView().getCenter().y ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
                    {
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x - 20, window.getView().getCenter().y ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
                    {
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x, window.getView().getCenter().y + 20 ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
                    {
                        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x, window.getView().getCenter().y - 20 ), window.getView().getSize() ) );
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::T) )
                    {
                        if ( testing )
                        {
                            testing = false;
                            std :: cout << " Testing toggled off\n";
                        }
                        else 
                        {
                            testing = true;
                            training = false;
                            std :: cout << " Testing toggled on and training toggled off\n";
                        }
                    }

                    if (!testing )
                    {
                        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::K) )
                        {
                            std :: cout << " DataSet saved\n";
                            dataSet.save();
                        }

                        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::W) )
                        {
                            Wpress = true;
                            actions[0] = 1;
                        }

                        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::A) )
                        {
                            Apress = true;
                            actions[1] = 1;
                        }

                        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::D) )
                        {
                            Dpress = true;
                            actions[2] = 1;
                        }
                    }

                    break;
                }

                case ( sf::Event::KeyReleased ):
                {
                    if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::W) )
                    {
                        Wpress = false;
                        actions[0] = 0;
                    }

                    if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::A) )
                    {
                        Apress = false;
                        actions[1] = 0;
                    }
                    
                    if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::D) )
                    {
                        Dpress = false;
                        actions[2] = 0;
                    }
                    
                    break;
                }

            default:
                break;
            }
        }

        window.clear(sf::Color::Black);

        //window.draw(path);

        window.draw(agent.drawable());
    
        std :: vector <sf::CircleShape> circle = agent.inter(edges);
        for(int i = 0; i < rayCount; i++)
        {
            window.draw( circle[i] );
        }

        window.display();

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = finish - start;

        if ( testing )
        {
            getInputs(input, agent, circle);

            agent.brain.Decide(input);
            
            if ( agent.brain.getOutput()[0] > 0.5 )
            {
                agent.translate( steps_per_second * elapsed.count(), window);
                actions[0] = 1;
            }
            else actions[0] = 0;

            if ( agent.brain.getOutput()[1] > 0.5)
            {
                agent.rotate( angle_per_second * elapsed.count() );
                actions[1] = 1;
            }
            else actions[1] = 0;

            if ( agent.brain.getOutput()[2] > 0.5)
            {
                agent.rotate( -angle_per_second * elapsed.count() );
                actions[2] = 1;
            }
            else actions[2] = 0;
        }
        else
        {
            if ( Wpress || Apress || Dpress )
            {
                if ( Wpress )
                {
                    agent.translate( steps_per_second * elapsed.count(), window);
                }

                if ( Apress )
                {
                    agent.rotate( angle_per_second * elapsed.count() );
                }

                if ( Dpress )
                {
                    agent.rotate( -angle_per_second * elapsed.count() );
                }

                getInputs(input, agent, circle);

                dataSet.append(input, actions);
            }
        }

        if ( agent.touch(edges) )
        {
            Agent temp(1, spawn, size, rayCount, window, maxDist);
            temp.brain = agent.brain;

            agent = temp;

            dataSet.clear();
        }
    }
}