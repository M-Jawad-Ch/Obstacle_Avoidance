#include<vector>
#include<fstream>
#include<iostream>
#include<chrono>
#include<thread>

#include<SFML/Graphics.hpp>
#include"Headers/agent.hpp"
#include"Headers/dataSet.hpp"
#include"Headers/helper.hpp"
#include"Headers/timer.hpp"

#include"variables.hpp"

int main()
{
    std::uniform_int_distribution<int> seedGen( -INT_MAX, INT_MAX );
    std::random_device rDev;
    ENGINE.seed( seedGen(rDev) );

    sf::VertexArray path, entities;
    std::vector<Edge> edges;

    load_Path(edges, path);

    sf::ContextSettings settings; settings.antialiasingLevel = 4.0;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Renderer", sf::Style::Default, settings);
    sf::Event event;

    std::vector<Agent> Agents;
    for(int i = 0; i < AGENT_COUNT; i++)
    {
        Agent agent( LAYER_COUNT , AGENT_SIZE, RAY_COUNT, MAX_DISTANCE);
        agent.brain.layers[0] = Layer(SECOND_LAYER_SIZE, RAY_COUNT);
        agent.brain.layers[1] = Layer(CLASSES, SECOND_LAYER_SIZE);

        Agents.push_back(agent);
    }

    Matrix input = Matrix(RAY_COUNT, 1, 1);
    float actions[ CLASSES ] = {0};
    bool pathVisible = true;
    Timer GeneratioLife;
    int GenerationCount = 1;

    window.setFramerateLimit( FRAMES_PER_SECOND );

    sf::VertexArray Agent_Bodies, Agent_Body;

    std::thread Threads[ THREAD_COUNT ];

    auto Animate = [&](float interval, std::chrono::duration<double> elapsed)
    {
        for(int i = Agents.size() * interval / THREAD_COUNT; i < Agents.size() * (interval + 1) / THREAD_COUNT; i++)
        {
            if ( Agents[i].isAlive )
            {
                if ( Agents[i].LifeSpan == MAX_TICKS_PER_AGENT )
                {
                    ALIVE_AGENTS = 0;
                    break;
                }

                getInputs( input, Agents[i], Agents[i].inter(edges) );

                Agents[i].brain.Decide(input);

                switch ( Agents[i].brain.layers[ LAYER_COUNT - 1 ].output() )
                {
                    case 0:
                        Agents[i].translate( STEPS_PER_SECOND * elapsed.count() );
                        Agents[i].MaxDistance += STEPS_PER_SECOND * elapsed.count();
                        break;

                    case 1:
                        Agents[i].rotate( RADIANS_PER_SECOND * elapsed.count() );
                        break;

                    case 2:
                        Agents[i].rotate( - RADIANS_PER_SECOND * elapsed.count() );
                        break;
                    
                    case 3:
                        Agents[i].translate( -STEPS_PER_SECOND * elapsed.count() );
                        Agents[i].MaxDistance -= STEPS_PER_SECOND * elapsed.count();
                        break;
                }

                Agents[i].LifeSpan++;

                if ( Agents[i].MaxDisplacement < Edge( Agents[i].center, Agents[i].Spawn ).magnitude() )
                    Agents[i].MaxDisplacement = Edge( Agents[i].center, Agents[i].Spawn ).magnitude();
                
                if ( Agents[i].touch(edges) )
                {
                    Agents[i].isAlive = false;
                    ALIVE_AGENTS--;
                }
            }
        }
    };

    std::string th = "st";

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

                case ( sf::Event::Resized ):
                    SCREEN_WIDTH = window.getSize().x;
                    SCREEN_HEIGHT = window.getSize().y;

                    window.setView( sf::View( sf::Vector2f(SCREEN_WIDTH/2.0, SCREEN_HEIGHT/2.0), sf::Vector2f(window.getSize().x * 1.2, window.getSize().y * 1.2) ) );
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

        window.setTitle( "ALIVE: " + std::to_string(ALIVE_AGENTS) + "/" + std::to_string(AGENT_COUNT) + "     " + std::to_string(GenerationCount) + th + " Gen     Duration: " + std::to_string((int)GeneratioLife.getInterval().count()) + "/" + std::to_string(MAX_SIMULATION_DURATION) + "s" );

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


        for(int i = 0; i < THREAD_COUNT; i++)
        {
            Threads[i] = std::thread( Animate, i, elapsed);
        }

        for(int i = 0; i < THREAD_COUNT; i++)
        {
            Threads[i].join();
        }

        if ( GenerationCount % SAVE_AFTER_X_GENERATIONS == 0 )
        {
            int MostFit = 0;
            for(int i = 0; i < Agents.size(); i++)
            {
                if ( Agents[i].Fitness > Agents[MostFit].Fitness )
                    MostFit = i;
            }

            Agents[MostFit].brain.save( "NeuralNets/Gen-" + std::to_string(GenerationCount) + ".bin" );
        }

        if ( ALIVE_AGENTS == 0 )
        {
            GeneratioLife.stop();

            Agent::AssignFitness(Agents);

            Agent::Reproduce( Agents );

            ALIVE_AGENTS = AGENT_COUNT;

            GenerationCount++;
            GeneratioLife.reset();

            switch ( GenerationCount % 10 )
            {
                case 1:
                    th = "st";
                    break;
                case 2:
                    th = "nd";
                    break;
                case 3:
                    th = "rd";
                    break;
                default:
                    th = "th";
                    break;
            }

            if ( GenerationCount == 11 || GenerationCount == 12 || GenerationCount == 13 )
                th = "th";

            MAX_SIMULATION_DURATION *= 1.25;
            MAX_TICKS_PER_AGENT = MAX_SIMULATION_DURATION * FRAMES_PER_SECOND;
        }
    }
}