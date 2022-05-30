#pragma once

#include"layer.hpp"

class Brain
{
public:
    std::vector<Layer> layers;
    int layerCount;

    Brain(int layers = 0)
    {
        this -> layers.resize(layers);
        layerCount = layers;
    }

    void Decide(const Matrix &input)
    {
        layers[0].WeightedSums = layers[0].Weights * input + layers[0].Bias;
        layers[0].activate();

        for(int i = 1; i < layerCount - 1; i++)
        {
            layers[i].WeightedSums = layers[i].Weights * layers[i - 1].WeightedSums + layers[i].Bias;
            layers[i].activate();
        }

        layers[layerCount - 1].WeightedSums = layers[layerCount - 1].Weights * layers[layerCount - 2].WeightedSums + layers[layerCount - 1].Bias;
        layers[layerCount - 1].activate();
    }

    Brain CrossOver(const Brain &brain)
    {
        Brain child = brain;

        for(int i = 0; i < layerCount; i++)
        {
            child.layers[i] = layers[i].crossOver(brain.layers[i]);
        }

        return child;
    }

    void Train(float targets[], const Matrix &input)
    {
        float error = 0;

        for(int i = 0; i < layers[ layerCount - 1 ].Weights.rows ; i++)
        {
            error += pow( targets[i] - layers[ layerCount - 1 ].WeightedSums.arr[i] ,2);
        }

        float *cachDeriv = nullptr, *nextCachDeriv = new float [ layers[ layerCount - 2 ].Weights.rows ];

        layers[layerCount - 1].backPropOutput(targets, layers[layerCount - 2], nextCachDeriv, 1, error);

        for(int i = layerCount - 2; i > 0; i--)
        {
            cachDeriv = nextCachDeriv;

            nextCachDeriv = new float[ layers[ i - 1 ].Weights.rows ];

            layers[i].backPropHidden(cachDeriv, layers[ i - 1 ], 1, error, nextCachDeriv);

            delete[] cachDeriv;
        }

        cachDeriv = nextCachDeriv;

        layers[0].backPropHidden(cachDeriv, input, 1, error);
        
        delete[] cachDeriv;
    }

    std::vector<float> getOutput()
    {
        return layers[ layerCount - 1 ].WeightedSums.arr;
    }

    void save(const std::string &fileName)
    {
        std::ofstream fout(fileName, std::ios::binary | std::ios::trunc);
        fout.close();

        for(int i = 0; i < layerCount; i++)
        {
            layers[i].save(fileName);
        }
    }
};