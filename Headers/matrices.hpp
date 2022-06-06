#pragma once

#include<vector>
#include<random>
#include<thread>
#include<iostream>
#include<climits>

#include"../variables.hpp"

class Matrix
{
    void init(float input)                                                  // Initiallizes weights for weight matrices using 'He Initiallization'
    {
        std::uniform_real_distribution<float> WeightDist( -1/sqrt(input), 1/sqrt(input) );


        for(int i = 0; i < rows*cols; i++)
        {
            arr[i] = WeightDist( ENGINE );
        }
    }

    public:

    unsigned int rows, cols;
    std::vector<float> arr;
    int static threads;                                                     // Number of threads allowed to processing matrix operations

    Matrix(unsigned int r = 0, unsigned int c = 0, bool nullify = false)
    {
        rows = r;
        cols = c;

        arr.resize(rows * cols);

        if ( !nullify )
        {
            init(cols);
        }
    }

    int getElementCount()
    {
        return rows * cols;
    }

    Matrix operator * (const Matrix &m)                                     // Function that multiplies two matrices using parrallel processing
    {
        if ( cols != m.rows )
        {
            std :: cout << " Error : matrices unfit for multiplication\n";
            return Matrix(rows, cols, 1);
        }
        else
        {
            Matrix result( rows, m.cols, 1 );

            auto lambda = [&](float j)
            {
                float val = 0;

                for ( int i = rows * j / threads; i < rows * ( j + 1 ) / threads; i++ )
                {
                    for ( int j = 0; j < m.cols; j++ )
                    {
                        for ( int k = 0; k < cols; k++ )
                        {
                            val += arr[ i * cols + k ] * m.arr[ k * m.cols + j ];
                        }

                        result.arr[ i * result.cols + j ] = val;
                        val = 0;
                    }
                }
            };

            std::thread workers[threads - 1];

            for(int i = 0; i < threads - 1; i++)
            {
                workers[i] = std::thread( lambda, i );
            }

            lambda(threads - 1);

            for(int i = 0; i < threads - 1; i++)
            {
                workers[i].join();
            }

            return result;
        }
    }

    Matrix operator + (const Matrix &m)                                     // A function that adds two matrices using parrallel processing
    {
        if ( rows != m.rows || cols != m.cols )
        {
            std :: cout << " Error : cannot add matrices of unequal order\n";
            return Matrix(rows, cols, 1);
        }
        else
        {
            Matrix result(rows, cols, 1);

            auto lambda = [&](float j)
            {
                for( int i = arr.size() * j / threads; i < arr.size() * (j + 1) / threads; i++ )
                {
                    result.arr[i] = arr[i] + m.arr[i];
                }
            };

            std::thread workers[threads - 1];

            for(int i = 0; i < threads - 1; i++)
            {
                workers[i] = std::thread( lambda, i );
            }

            lambda(threads - 1);

            for(int i = 0; i < threads - 1; i++)
            {
                workers[i].join();
            }

            return result;
        }
    }

    void operator = (const Matrix &m)
    {
        rows = m.rows;
        cols = m.cols;
        arr = m.arr;
    }
};


int Matrix::threads = 1;