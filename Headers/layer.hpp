#pragma once

#include"matrices.hpp"

class Layer
{
    float sigmoid(float val)
    {
        return 1 / (1 + pow(2.718,-val));
    }

public:
    Matrix Weights;
    Matrix WeightedSums;
    Matrix Bias;

    static int threads;

    Layer(int size = 0, int connections = 0)
    {
        Weights = Matrix(size, connections);
        WeightedSums = Matrix(size, 1, true);
        Bias = Matrix(size, 1);
    }

    void activate()
    {
        for(int i = 0; i < WeightedSums.rows; i++)
        {
            WeightedSums.arr[i] = sigmoid(WeightedSums.arr[i]);
            if(WeightedSums.arr[i] != WeightedSums.arr[i]) 
                WeightedSums.arr[i] = 1;
        }
    }

    void save(const std::string &fileName)
    {
        std::ofstream fout(fileName, std::ios::binary | std::ios::app);

        for(int i = 0; i < Weights.getElementCount(); i++)
        {
            fout.write( (char*)&Weights.arr[i], sizeof(Weights.arr[i]) );
        }

        for(int i = 0; i < Bias.getElementCount(); i++)
        {
            fout.write( (char*)&Bias.arr[i], sizeof(Bias.arr[i]) );
        }

        fout.close();
    }

    int output()
    {
        int max = 0;

        for(int i = 0; i < WeightedSums.arr.size(); i++)
        {
            if ( WeightedSums.arr[ max ] < WeightedSums.arr[i] )
                max = i;
        }

        return max;
    }

    Layer crossOver(const Layer &layer, int Remainder)
    {
        Layer child( Weights.rows, Weights.cols );

        for(int i = 0; i < Weights.arr.size(); i++)
        {
            if ( i % 2 == Remainder )
                child.Weights.arr[i] = Weights.arr[i];
            else child.Weights.arr[i] = layer.Weights.arr[i];
        }

        for(int i = 0; i < Bias.arr.size(); i++)
        {
            if ( i % 2 == Remainder )
                child.Bias.arr[i] = Bias.arr[i];
            else child.Bias.arr[i] = layer.Bias.arr[i];
        }

        return child;
    }

    void rel()
    {
        for(int i = 0; i < WeightedSums.rows; i++)
        {
            if ( WeightedSums.arr[i] < 0 ) WeightedSums.arr[i] = 0;
        }
    }

    void softMax()
    {
        for(int i = 0; i < WeightedSums.rows; i++)
        {
            float numerator = pow(2.718, WeightedSums.arr[i]), denomenator = 0;

            for(int j = 0; j < WeightedSums.rows; j++)
            {
                denomenator += pow(2.718, WeightedSums.arr[j]);
            }

            WeightedSums.arr[i]  = numerator / denomenator;
        }
    }

    void backPropOutput(float *targetVals, const Layer& secondLastLayer, float *cachDeriv, float learningRate, const float error)
    {
        auto lambda = [&](int interval)
        {
            float del = 0, val = 0, temp = 0;
            for(int i = interval * (float)Weights.rows / threads ; i < (interval + 1) * (float)Weights.rows / threads; i++)
            {
                for(int j = 0; j < Weights.cols; j++)
                {
                    del = 2 * (WeightedSums.arr[i] - targetVals[i]) * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]) * secondLastLayer.WeightedSums.arr[j];
                    val = Weights.arr[i * Weights.cols + j] - learningRate * del;
                    Weights.arr[i * Weights.cols + j] = val;
                    cachDeriv[j] += 2*(WeightedSums.arr[i] - targetVals[i]) * Weights.arr[i * Weights.cols + j] * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]);
                }

                del = 2 * (WeightedSums.arr[i] - targetVals[i]) * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]);
                val = Bias.arr[i] - learningRate * del;
                Bias.arr[i] = val;
            }
        };

        std :: thread t[threads - 1];

        for(int i = 0; i < threads - 1; i++)
        {
            t[i] = std :: thread(lambda, i);
        }

        lambda(threads - 1);

        for(int i = 0; i < threads - 1; i++)
        {
            t[i].join();
        }
    }

    void backPropHidden(float* cachDeriv, const Layer &previousLayer, float learningRate, const float error, float *nextCachDeriv = nullptr)
    {
        auto lambda = [&](int interval)
        {
            for(int i = interval * (float)Weights.rows / threads ; i < (interval + 1) * (float)Weights.rows / threads; i++)
            {
                float del = 0, val = 0;
                for(int j = 0; j < Weights.cols; j++)
                {
                    del = cachDeriv[i] * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]) * previousLayer.WeightedSums.arr[j];
                    val = Weights.arr[i * Weights.cols + j] - learningRate * del;
                    Weights.arr[i * Weights.cols + j] = val;
                    if(nextCachDeriv) nextCachDeriv[j] += cachDeriv[i] * Weights.arr[i * Weights.cols + j] * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]);
                }

                del = cachDeriv[i] * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]);
                val = Bias.arr[i] - learningRate * del;
                Bias.arr[i] = val;
            }
        };

        std :: thread t[threads - 1];

        for(int i = 0; i < threads - 1; i++)
        {
            t[i] = std :: thread(lambda, i);
        }

        lambda(threads - 1);

        for(int i = 0; i < threads - 1; i++)
        {
            t[i].join();
        }
    }

    void backPropHidden(float* cachDeriv, const Matrix &previousLayer, float learningRate, const float error, float *nextCachDeriv = nullptr)
    {
        auto lambda = [&](int interval)
        {
            float del = 0, val = 0, temp = 0;
            for(int i = interval * (float)Weights.rows / threads; i < (interval + 1) * (float)Weights.rows / threads; i++)
            {
                for(int j = 0; j < Weights.cols; j++)
                {
                    del = cachDeriv[i] * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]) * previousLayer.arr[j];
                    val = Weights.arr[i * Weights.cols + j] - learningRate * del;
                    Weights.arr[i * Weights.cols + j] = val;
                    if(nextCachDeriv) nextCachDeriv[j] += cachDeriv[i] * Weights.arr[i * Weights.cols + j] * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]);
                }

                del = cachDeriv[i] * WeightedSums.arr[i] * (1 - WeightedSums.arr[i]);
                val = Bias.arr[i] - learningRate * del;
                Bias.arr[i] = val;
            }
        };

        std :: thread t[threads - 1];

        for(int i = 0; i < threads - 1; i++)
        {
            t[i] = std :: thread(lambda, i);
        }

        lambda(threads - 1);

        for(int i = 0; i < threads - 1; i++)
        {
            t[i].join();
        }
    }
};

int Layer::threads = 1;