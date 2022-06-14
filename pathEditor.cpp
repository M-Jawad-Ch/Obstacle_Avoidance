#include<SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include<fstream>
#include<chrono>

void save(sf::VertexArray &path, const std::vector<sf::Vector2f> &Spawns)
{
    std::vector<float> data;
    
    for(int i = 0; i < path.getVertexCount(); i++)
    {
        data.push_back( path[i].position.x );
        data.push_back( path[i].position.y );
    }

    std :: cout << " Enter the name of your path : ";
    std :: string fileName; std :: cin >> fileName;
    fileName = "Environments/" + fileName + ".bin";

    std :: ofstream fout(fileName);
    int vertexCount = path.getVertexCount();

    fout.write((char*)&vertexCount, sizeof(int));

    for(int i = 0; i < data.size(); i++)
    {
        fout.write((char*)&data[i], sizeof(float));
    }

    data.clear();

    for(int i = 0; i < data.size(); i++)
    {
        fout.write((char*)&data[i], sizeof(float));
    }

    int size = Spawns.size();

    fout.write((char*)&size, sizeof(int));

    for(int i = 0; i < Spawns.size(); i++)
    {
        fout.write((char*)&Spawns[i].x, sizeof(float));
        fout.write((char*)&Spawns[i].y, sizeof(float));
    }

    fout.close();
}

int main()
{
    int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH,SCREEN_HEIGHT), "Path Editor");
    sf::Event event;

    sf::VertexArray path, spawns;
    path.setPrimitiveType(sf::PrimitiveType::Lines);
    bool sameObj = false, target = false; int startOfPath = 0;
    bool PathMode = true;

    std::vector<sf::Vector2f> Spawns;

    bool wPress = false, aPress = false, sPress = false, dPress = false;

    window.setFramerateLimit(60);

    sf::Vector2f pos = sf::Vector2f( window.getView().getCenter() );
    sf::Vector2f screen = pos, MousePos;

    auto start = std::chrono::high_resolution_clock::now();

    while(window.isOpen())
    {
        window.clear(sf::Color::Black);

        window.draw(path);
        window.draw(spawns);

        window.display();

        while(window.pollEvent(event))
        {  
            switch (event.type)
            {
                case ( sf::Event::Closed ):
                    window.close();
                    break;
                
                case ( sf::Event::MouseButtonPressed ):
                    if ( PathMode )
                    {
                        if ( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                        {
                            if ( path.getVertexCount() > 0 && sameObj )
                            {
                                sf::Vector2f point = sf::Vector2f(sf::Mouse::getPosition(window)) + pos - screen;
                                path.append( sf::Vertex( point ) );
                                path.append( path[ path.getVertexCount() - 1 ] );
                            }
                            else
                            {
                                sf::Vector2f point = sf::Vector2f(sf::Mouse::getPosition(window)) + pos - screen;
                                path.append( sf::Vertex( point ) );
                                startOfPath = path.getVertexCount() - 1;
                                sameObj = true;
                            }
                        }
                        else if ( sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) )
                        {
                            path.append( path[startOfPath] );
                            sameObj = false;
                        }
                    }
                    else
                    {
                        MousePos = sf::Vector2f(sf::Mouse::getPosition(window)) + pos - screen;
                        Spawns.push_back(MousePos);
                        spawns.append( sf::Vertex( Spawns[ Spawns.size() - 1 ], sf::Color::Red ) );
                    }

                    break;

                case ( sf::Event::KeyPressed ):
                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) )
                        window.close();

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace) && !PathMode )
                    {
                        if ( Spawns.size() > 0 )
                        {
                            Spawns.pop_back();
                            spawns.resize( spawns.getVertexCount() - 1 );
                        }
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::S) )
                    {
                        window.close();
                        save(path, Spawns);
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::C) )
                    {
                        path.clear();
                        Spawns.clear();
                        spawns.clear();
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::P) )
                    {
                        if ( PathMode )
                        {
                            PathMode = false;
                            window.setTitle("Spawn Editing Mode");
                        }
                        else
                        {
                            PathMode = true;
                            window.setTitle("Path Editing Mode");
                        }
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::L) )
                    {
                        Spawns.clear();
                        spawns.clear();
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
                        wPress = true;

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left) )
                        aPress = true;

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
                        sPress = true;
                    
                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
                        dPress = true;

                    break;

                    case sf::Event::KeyReleased:
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
                            wPress = false;
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
                            sPress = false;
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Left) )
                            aPress = false;
                        if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
                            dPress = false;
                        break;

                    case sf::Event::Resized:
                        SCREEN_WIDTH = window.getSize().x;
                        SCREEN_HEIGHT = window.getSize().y;

                        pos.x += SCREEN_WIDTH/2 - pos.x;
                        pos.y += SCREEN_HEIGHT/2 - pos.y;

                        screen = sf::Vector2f(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);

                        window.setView( sf::View( pos, sf::Vector2f(SCREEN_WIDTH,SCREEN_HEIGHT) ) );
                        break;
            }
        }

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> deltaTime = finish - start;

        start = std::chrono::high_resolution_clock::now();

        if ( wPress )
        {
            pos += sf::Vector2f( 0, -200 * deltaTime.count() );
            window.setView(sf::View( pos, sf::Vector2f(SCREEN_WIDTH,SCREEN_HEIGHT) ));
        }

        if ( sPress )
        {
            pos += sf::Vector2f( 0, 200 * deltaTime.count() );
            window.setView(sf::View( pos, sf::Vector2f(SCREEN_WIDTH,SCREEN_HEIGHT) ));
        }

        if ( aPress )
        {
            pos += sf::Vector2f( -200 * deltaTime.count(), 0 );
            window.setView(sf::View( pos, sf::Vector2f(SCREEN_WIDTH,SCREEN_HEIGHT) ));  
        }

        if ( dPress )
        {
            pos += sf::Vector2f( 200 * deltaTime.count(), 0 );
            window.setView(sf::View( pos, sf::Vector2f(SCREEN_WIDTH,SCREEN_HEIGHT) ));
        }
    }
}