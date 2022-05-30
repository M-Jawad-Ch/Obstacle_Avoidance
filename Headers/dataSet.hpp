#pragma once

#include<vector>
#include<string>
#include<fstream>

#include"agent.hpp"

class LayerMeta
{
public:

    int rows, cols;
};

class Meta
{
public:

    int rayCount;
    int layerCount;
    int floatCount;

    std::vector<LayerMeta> layerMeta;

    Meta(int rayCount = 0, int layerCount = 0) : rayCount(rayCount), layerCount(layerCount), floatCount(0)
    {
        layerMeta.resize(layerCount);
    }

    void save(const std::string &fileName)
    {
        std::ofstream fout(fileName, std::ios::binary | std::ios::trunc);

        fout.write((char*)&rayCount, sizeof(int));
        fout.write((char*)&layerCount, sizeof(int));

        fout.write((char*)&floatCount, sizeof(int));

        for(int i = 0; i < layerCount; i++)
        {
            fout.write((char*)&layerMeta[i].rows, sizeof(int));
            fout.write((char*)&layerMeta[i].cols, sizeof(int));
        }

        fout.close();
    }

    void read(const std::string &fileName)
    {
        std::ifstream fin(fileName, std::ios::binary);

        fin.read((char*)&rayCount, sizeof(int));
        fin.read((char*)&layerCount, sizeof(int));

        fin.read((char*)&floatCount, sizeof(int));

        layerMeta.resize(layerCount);

        for(int i = 0; i < layerCount; i++)
        {
            fin.read((char*)&layerMeta[i].rows, sizeof(int));
            fin.read((char*)&layerMeta[i].cols, sizeof(int));
        }

        fin.close();
    }
};


class DataSet
{
    std::string fileName;

public:

    Meta meta;

    std::vector<float> dataSet;

    DataSet(std::string fileName)
    {
        this -> fileName = fileName;
    }

    void append(const Matrix &inputs, float *actions)
    {
        for(int i = 0; i < 2; i++)
        {
            dataSet.push_back(actions[i]);
        }

        for(int i = 0; i < inputs.rows; i++)
        {
            dataSet.push_back( inputs.arr[i] );
        }
    }

    void save()
    {
        std :: ofstream fout( fileName, std::ios::binary | std::ios::app );

        for(int i = 0; i < dataSet.size(); i++)
        {
            fout.write((char*)&dataSet[i], sizeof(float));
        }

        fout.close();

        meta.floatCount += dataSet.size();

        meta.save("meta-" + fileName);

        dataSet.clear();
    }

    void read(const std::string &fileName)
    {
        dataSet.clear();

        meta.read( "meta-" + fileName );

        dataSet.resize( meta.floatCount );

        std::ifstream fin(fileName, std::ios::binary);

        for(int i = 0; i < meta.floatCount; i++)
        {
            fin.read((char*)&dataSet[i], sizeof(float));
        }

        fin.close();
    }

    void clear()
    {
        dataSet.clear();
    }
};