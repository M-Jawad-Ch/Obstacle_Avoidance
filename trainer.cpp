#include"Headers/dataSet.hpp"
#include"Headers/agent.hpp"

int Matrix::threads = 4, Layer::threads = 4;

int main()
{
    std::string dataSetFile = "DataSet/dataSet.bin";
    DataSet data( dataSetFile ); data.read( dataSetFile );

    float target[3] = {0};

    Brain NeuralNet( data.meta.layerCount );

    for(int i = 0; i < NeuralNet.layerCount; i++)
    {
        NeuralNet.layers[i] = Layer( data.meta.layerMeta[i].rows, data.meta.layerMeta[i].cols );
    }

    Matrix input( data.meta.rayCount, 1, 1 );

    int AutoSave = 10, CurrentEpoch = 0;
    int index = 0;

    while(true)
    {
        for(int i = 0; i < 3; i++)
        {
            target[i] = data.dataSet[ index ];
            index++;
        }

        for(int i = 0; i < data.meta.rayCount; i++)
        {
            input.arr[i] = data.dataSet[ index ];
            index++;
        }

        NeuralNet.Decide(input);
        NeuralNet.Train(target, input);

        if ( index == data.meta.floatCount )
        {
            index = 0;

            CurrentEpoch++;

            std::cout << " Current Epoch: " << CurrentEpoch << "\n";

            NeuralNet.save("NeuralNets/first.bin");
        }
    }
}