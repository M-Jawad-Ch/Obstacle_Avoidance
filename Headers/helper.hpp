void load_Path(std::vector<Edge> &edges, sf::VertexArray &vertices, std::vector<sf::Vector2f> &spawns, std::string PathFileName = "")
{
    std::string fileName = PathFileName;

    if ( PathFileName == "" )
    {
        std :: cout << " Enter the environment name : ";
        fileName; std :: cin >> fileName;
    }

    fileName = "Environments/" + fileName + ".bin";

    std :: ifstream fin(fileName);

    int vertexCount; sf::Vector2f temp; Edge tempE;

    fin.read((char*)&vertexCount, sizeof(int));

    for(int i = 0; i < vertexCount; i++)
    {
        fin.read((char*)&temp.x, sizeof(float));
        fin.read((char*)&temp.y, sizeof(float));

        tempE.p1 = temp;
        
        vertices.append(sf::Vertex(temp));

        fin.read((char*)&temp.x, sizeof(float));
        fin.read((char*)&temp.y, sizeof(float));

        tempE.p2 = temp;
        
        vertices.append(sf::Vertex(temp));
        edges.push_back(tempE);

        i++;
    }

    vertices.setPrimitiveType(sf::PrimitiveType::Lines);

    int SpawnPosCount;

    fin.read((char*)&SpawnPosCount, sizeof(int));
    spawns.resize( SpawnPosCount );

    for(int i = 0; i < SpawnPosCount; i++)
    {
        fin.read((char*)&spawns[i].x, sizeof(float));
        fin.read((char*)&spawns[i].y, sizeof(float));
    }

    fin.close();
}

void getInputs(Matrix &input, const Agent &agent, const std :: vector <sf::Vector2f> &points)
{
    Edge temp;

    float MaxDist = sqrt( 2*pow(agent.MaxDist,2) );

    for(int i = 0; i < points.size(); i++)
    {
        temp.p1 = agent.center;
        temp.p2 = points[i];

        input.arr[i] = (MaxDist - temp.magnitude()) / MaxDist;
    }
}



void read( Brain &NeuralNet, const std::string &fileName )
{
    std::ifstream fin( fileName, std::ios::binary );

    for(int i = 0; i < NeuralNet.layerCount; i++)
    {
        for(int j = 0; j < NeuralNet.layers[i].Weights.getElementCount(); j++)
        {
            fin.read((char*)&NeuralNet.layers[i].Weights.arr[j], sizeof(float) );
        }

        for(int j = 0; j < NeuralNet.layers[i].Bias.getElementCount(); j++)
        {
            fin.read((char*)&NeuralNet.layers[i].Bias.arr[j], sizeof(float) );
        }

    }

    fin.close();
}

void Displace( sf::RenderWindow &window, float DeltaTime, bool Up, bool Down, bool Left, bool Right )
{
    if ( Up )
        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x, window.getView().getCenter().y - SCREEN_DISPLACEMENT_PER_SECOND * DeltaTime ), window.getView().getSize() ) );

    if ( Down )
        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x, window.getView().getCenter().y + SCREEN_DISPLACEMENT_PER_SECOND * DeltaTime ), window.getView().getSize() ) );

    if ( Left )
        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x - SCREEN_DISPLACEMENT_PER_SECOND * DeltaTime, window.getView().getCenter().y ), window.getView().getSize() ) );

    if ( Right )
        window.setView( sf::View( sf::Vector2f( window.getView().getCenter().x + SCREEN_DISPLACEMENT_PER_SECOND * DeltaTime, window.getView().getCenter().y ), window.getView().getSize() ) );
}