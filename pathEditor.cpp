#include<SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include<fstream>
#include<chrono>

void save(sf::VertexArray &path, sf::Vector2f spawn)
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

    spawn.x;
    spawn.y;

    fout.write((char*)&spawn.x, sizeof(float));
    fout.write((char*)&spawn.y, sizeof(float));

    fout.close();
}

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Path Editor", sf::Style::Fullscreen);
    sf::Event event;

    sf::VertexArray path;
    path.setPrimitiveType(sf::PrimitiveType::Lines);
    bool sameObj = false, target = false; int startOfPath = 0;
    sf::Vector2f spawn, pos = sf::Vector2f( sf::VideoMode::getDesktopMode().width / 2.0, sf::VideoMode::getDesktopMode().height / 2.0 );

    bool wPress = false, aPress = false, sPress = false, dPress = false;

    sf::Vector2f screen = sf::Vector2f(sf::VideoMode::getDesktopMode().width / 2.0, sf::VideoMode::getDesktopMode().height / 2.0);

    window.setFramerateLimit(60);

    auto start = std::chrono::high_resolution_clock::now();

    while(window.isOpen())
    {
        window.clear(sf::Color::Black);

        window.draw(path);

        window.display();

        while(window.pollEvent(event))
        {  
            switch (event.type)
            {
                case ( sf::Event::Closed ):
                    window.close();
                    break;
                
                case ( sf::Event::MouseButtonPressed ):
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

                    break;

                case ( sf::Event::KeyPressed ):
                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) )
                        window.close();

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::S) )
                    {
                        window.close();
                        save(path, spawn);
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::C) )
                    {
                        path.clear();
                    }

                    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::P) )
                    {
                        spawn = sf::Vector2f(sf::Mouse::getPosition(window));
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
            }
        }

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> deltaTime = finish - start;

        start = std::chrono::high_resolution_clock::now();

        if ( wPress )
        {
            pos += sf::Vector2f( 0, 1000 * deltaTime.count() );
            wPress = false;

            window.setView(sf::View( pos, sf::Vector2f(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height) ));
        }

        if ( sPress )
        {
            pos += sf::Vector2f( 0, -1000 * deltaTime.count() );
            sPress = false;

            window.setView(sf::View( pos, sf::Vector2f(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height) ));
        }

        if ( aPress )
        {
            pos += sf::Vector2f( -1000 * deltaTime.count(), 0 );
            aPress = false;

            window.setView(sf::View( pos, sf::Vector2f(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height) ));
        }

        if ( dPress )
        {
            pos += sf::Vector2f( 1000 * deltaTime.count(), 0 );
            dPress = false;

            window.setView(sf::View( pos, sf::Vector2f(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height) ));
        }
    }
}