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

    int CurrentPath = 0;

    std::string pathNames[] = {"path1", "path2"};
    int pathCount = sizeof(pathNames) / sizeof(pathNames[0]);
    
    sf::VertexArray *VertexArrays = new sf::VertexArray[ pathCount ];
    std::vector<Edge> *PATHS = new std::vector<Edge>[ pathCount ];
    std::vector<sf::Vector2f> *SPAWNS = new std::vector<sf::Vector2f>[ pathCount ];

    for(int i = 0; i < pathCount; i++)
    {
        load_Path( PATHS[i], VertexArrays[i], SPAWNS[i], pathNames[i] );
    }

    sf::ContextSettings settings; settings.antialiasingLevel = 4.0;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Renderer", sf::Style::Default, settings);
    sf::Event event;

    std::vector<Agent> Agents;
    for(int i = 0; i < AGENT_COUNT; i++)
    {
        Agent agent( LAYER_COUNT , AGENT_SIZE, RAY_COUNT, MAX_DISTANCE, SPAWNS[ CurrentPath ]);
        agent.brain.layers[0] = Layer(SECOND_LAYER_SIZE, RAY_COUNT);
        agent.brain.layers[1] = Layer(CLASSES, SECOND_LAYER_SIZE);

        Agents.push_back(agent);
    }

    Matrix input = Matrix(RAY_COUNT, 1, 1);
    float actions[ CLASSES ] = {0};
    bool pathVisible = true;
    Timer GenerationLife;
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
                getInputs( input, Agents[i], Agents[i].inter( PATHS[CurrentPath] ) );

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
                
                if ( Agents[i].touch( PATHS[CurrentPath] ) )
                {
                    Agents[i].isAlive = false;
                    ALIVE_AGENTS--;
                }
            }
        }

        if ( (int)GenerationLife.getInterval().count() == MAX_SIMULATION_DURATION )
        {
            ALIVE_AGENTS = 0;
            return;
        }
    };

    std::string th = "st";

    bool Up = false, Down = false, Left = false, Right = false;

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
                        Left = true;
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
                    {
                        Right = true;
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
                    {
                        Up = true;
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
                    {
                        Down = true;
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

                    case sf::Event::KeyReleased:
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
                            Up = false;
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
                            Down = false;
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Left) )
                            Left = false;
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
                            Right = false;
                }

            default:
                break;
            }
        }

        window.setTitle( "ALIVE: " + std::to_string(ALIVE_AGENTS) + "/" + std::to_string(AGENT_COUNT) + "     " + std::to_string(GenerationCount) + th + " Gen     Duration: " + std::to_string((int)GenerationLife.getInterval().count()) + "/" + std::to_string(MAX_SIMULATION_DURATION) + "s" );

        window.clear(sf::Color::Black);

        if ( pathVisible ) window.draw( VertexArrays[CurrentPath] );

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

        Displace(window, elapsed.count(), Up, Down, Left, Right);


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
            GenerationLife.stop();

            Agent::AssignFitness(Agents);

            if ( CurrentPath == pathCount - 1 ) CurrentPath = 0;
            else CurrentPath++;

            Agent::Reproduce( Agents, SPAWNS[CurrentPath] );

            ALIVE_AGENTS = AGENT_COUNT;

            GenerationCount++;
            GenerationLife.reset();

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

            MAX_SIMULATION_DURATION *= 1.1;
        }
    }

    delete[] PATHS;
    delete[] VertexArrays;
}