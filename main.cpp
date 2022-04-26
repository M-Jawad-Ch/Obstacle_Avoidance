#include<vector>
#include<fstream>
#include<iostream>
#include<chrono>


#include<SFML/Graphics.hpp>
#include"Headers/agent.hpp"
#include"Headers/dataSet.hpp"



void load_Path(std::vector<Edge> &edges, sf::VertexArray &vertices, sf::Vector2f screen, sf::Vector2f &spawn)
{
    std :: cout << " Enter the environment name : ";
    std :: string fileName; std :: cin >> fileName;

    fileName = "Environments/" + fileName + ".bin";

    std :: ifstream fin(fileName);

    int vertexCount; sf::Vector2f temp; Edge tempE;

    fin.read((char*)&vertexCount, sizeof(int));

    for(int i = 0; i < vertexCount; i++)
    {
        fin.read((char*)&temp.x, sizeof(float));
        fin.read((char*)&temp.y, sizeof(float));

        temp.x *= screen.x;
        temp.y *= screen.y;

        tempE.p1 = temp;
        
        vertices.append(sf::Vertex(temp));

        fin.read((char*)&temp.x, sizeof(float));
        fin.read((char*)&temp.y, sizeof(float));

        temp.x *= screen.x;
        temp.y *= screen.y;

        tempE.p2 = temp;
        
        vertices.append(sf::Vertex(temp));
        edges.push_back(tempE);

        i++;
    }

    vertices.setPrimitiveType(sf::PrimitiveType::Lines);

    fin.read((char*)&spawn.x, sizeof(float));
    fin.read((char*)&spawn.y, sizeof(float));

    spawn.x *= screen.x;
    spawn.y *= screen.y;

    fin.close();
}

void getInputs(Matrix &input, const Agent &agent, const std :: vector <sf::CircleShape> &circle)
{
    Edge temp;

    for(int i = 0; i < circle.size(); i++)
    {
        temp.p1 = agent.center;
        temp.p2 = circle[i].getPosition();

        input.arr[i] = 1.0 / temp.magnitude();
    }
}

int Matrix::threads = 1, Layer::threads = 1;

int main()
{
    float pi = 22.0 / 7.0, size = 5.0 / 1300;
    float width = 800, height = 600;
    int rayCount = 36;

    sf::VertexArray path, entities;
    std::vector<Edge> edges;
    sf::Vector2f screen( width, height ), spawn;

    load_Path(edges, path, screen, spawn);

    sf::ContextSettings settings; settings.antialiasingLevel = 4.0;
    sf::RenderWindow window(sf::VideoMode(width, height), "Renderer", sf::Style::Default, settings);
    sf::Event event;

    Agent agent(2, spawn, size * width, rayCount, window);
    agent.brain.layers[0] = Layer(16, rayCount);
    agent.brain.layers[1] = Layer(4, 16);

    Matrix input = Matrix(rayCount, 1, 1);
    std::vector<float> actions; actions.resize(3);
    sf::Vector2f target;
    bool Wpress = false, Apress = false, Dpress = false, training = false;


    window.setFramerateLimit(30);

    DataSet dataSet("DataSet/dataSet.bin");

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

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::K) )
                    {
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

        window.draw(path);

        window.draw(agent.drawable());
    
        std :: vector <sf::CircleShape> circle = agent.inter(edges);
        for(int i = 0; i < rayCount; i++)
        {
            window.draw( circle[i] );
        }

        window.display();

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = finish - start;

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

            dataSet.append(input, actions);
        }

        if ( agent.touch(edges) )
        {
            Agent temp(1, spawn, size * width, rayCount, window);
            temp.brain = agent.brain;

            agent = temp;

            dataSet.clear();
        }

        getInputs(input, agent, circle);
    }
}