
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

    fin.read((char*)&spawn.x, sizeof(float));
    fin.read((char*)&spawn.y, sizeof(float));

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

int Matrix::threads = 4, Layer::threads = 4;