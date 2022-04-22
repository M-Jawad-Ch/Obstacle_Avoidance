#pragma once

#include<vector>
#include<string>
#include<fstream>

#include"agent.hpp"

class DataSet
{
    std::string fileName;

    std::vector<float> dataSet;

public:

    DataSet(std::string fileName)
    {
        this -> fileName = fileName;
    }

    void append(const Matrix &inputs, const std::vector<float> &actions)
    {
        for(int i = actions.size() - 1; i > -1; i--)
        {
            dataSet.push_back(actions[i]);
        }

        for(int i = inputs.rows - 1; i > -1; i--)
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

        dataSet.clear();
    }

    void clear()
    {
        dataSet.clear();
    }
};