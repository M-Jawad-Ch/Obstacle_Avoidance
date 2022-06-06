#pragma once

#include"edges.hpp"
#include"brain.hpp"
#include"../variables.hpp"

#include<vector>
#include<chrono>
#include<random>

#include<SFML/Graphics.hpp>

class Agent
{
    void setDirection()
    {
        direction = front - center;
        Edge e; e.p1 = center; e.p2 = front;

        direction.x /= -e.magnitude();
        direction.y /= -e.magnitude();
    }

    sf::Vector2f transform(sf::Vector2f p, sf::Vector2f origin, float angle)
    {
        p -= origin;

        sf::Vector2f resultant;

        resultant.x = p.x * cos(angle) + p.y * sin(angle);
        resultant.y = -p.x * sin(angle) + p.y * cos(angle);

        resultant += origin;

        return resultant;
    }

    void setRays(int rayCount)
    {
        float pi = 22.0 / 7.0;

        for(float x = 0; x < 2*pi; x += 2*pi / rayCount)
        {
            Edge e;
            e.p1 = center;

            e.p2.x = MaxDist; e.p2.y = 0;
            e.p2 = transform(e.p2, sf::Vector2f(0,0), x);
            
            e.p2.x += center.x; e.p2.y += center.y;

            rays.push_back(e);
        }
    }

    Agent CrossOver(const Agent &SecondParent, int Remainder)
    {
        Agent Child( brain.layerCount, AGENT_SIZE, RAY_COUNT, MAX_DISTANCE  );

        for(int i = 0; i < LAYER_COUNT; i++)
        {
            Child.brain.layers[i] = brain.layers[i].crossOver(SecondParent.brain.layers[i], Remainder);
        }
        
        return Child;
    }

    void Mutate()
    {
        std::uniform_int_distribution<int> layerToMutate(0, LAYER_COUNT - 1);
        std::uniform_int_distribution<int> WeightCountToMutate(0, MAX_MUTATED_WEIGHTS);

        unsigned int WeightsToMutate = WeightCountToMutate( ENGINE ), Layer = layerToMutate(ENGINE), Weight;
        std::uniform_int_distribution<int> Weights(0, brain.layers[ Layer ].Weights.arr.size());

        for(int i = 0; i < WeightsToMutate; i++)
        {
            Weight = Weights(ENGINE);
            brain.layers[ Layer ].Weights.arr[ Weight ] += 0.25 * brain.layers[ Layer ].Weights.arr[ Weight ];
        }
    }

    struct Data 
    {
        float Upper_Bound, Lower_Bound;
    };

    Edge e1, e2, e3, e4;
    float maxDist;
    std::uniform_int_distribution<int> SpawnPos;

public:
    sf::Vector2f center, front;
    sf::Vector2f direction;
    sf::Vector2f Spawn;
    std::vector<Edge> rays;
    Brain brain;
    float MaxDist;
    bool isAlive;

    float Fitness;
    float MaxDistance;
    float MaxDisplacement;
    float LifeSpan;

    Agent(int layers, float scale, int rayCount, float maxDist)
    {
        LifeSpan = 0;
        Fitness = 0;
        isAlive = true;

        MaxDistance = 0;
        MaxDisplacement = 0;

        SpawnPos = std::uniform_int_distribution<int>(0, SPAWNS.size() - 1);

        sf::Vector2f cent = SPAWNS[SpawnPos(ENGINE)];
        Spawn = cent;

        MaxDist = 0;

        sf::Vector2f p1, p2, p3, p4;

        p1.x = -1.5; p1.y = -2;
        p2.x = 1.5; p2.y = -2;
        p3.x = 1.5; p3.y = 2;
        p4.x = -1.5; p4.y = 2;
        front.x = 0; front.y = -2;

        p1 *= scale; p2 *= scale;
        p3 *= scale; p4 *= scale;
        front *= scale;

        p1 += cent; p2 += cent;
        p3 += cent; p4 += cent;
        front += cent;

        e1.p1 = p1; e1.p2 = p2;
        e2.p1 = p2; e2.p2 = p3;
        e3.p1 = p3; e3.p2 = p4;
        e4.p1 = p4; e4.p2 = p1;

        center = cent;

        setDirection();

        MaxDist = maxDist;

        setRays(rayCount);

        brain = Brain(layers);
    }

    static void Reproduce(std::vector<Agent> &Agents)
    {
        std::vector<Data> data;
        data.resize(Agents.size());

        float totalFitness = 0;
        float upperBound = 0, lowerBound = 0, pos;

        std::uniform_int_distribution<int> Dist( 0, 100 );

        bool breaker = false;

        int firstParent, secondParent;

        std::vector<Agent> Children;

        for(int i = AGENT_COUNT; i > 0; i -= 2)
        {
            totalFitness = 0; upperBound = 0; lowerBound = 0;
            for(int j = 0; j < Agents.size(); j++)
            {
                totalFitness += Agents[j].Fitness;
            }

            if ( totalFitness == 0 ) totalFitness = 1;

            for(int j = 0; j < Agents.size(); j++)
            {
                    upperBound += Agents[j].Fitness / totalFitness * 100.0;

                    data[j].Lower_Bound = lowerBound;
                    data[j].Upper_Bound = upperBound;

                    lowerBound = upperBound;
            }

            while(true)
            {
                pos = Dist( ENGINE );
                for(int j = 0; j < Agents.size(); j++)
                {
                    if ( pos <= data[j].Upper_Bound && pos >= data[j].Lower_Bound )
                    {
                        firstParent = j;
                        breaker = true;
                        break;
                    }
                }
                if ( breaker )
                    break;
            }

            breaker = false;

            while(true)
            {
                pos = Dist( ENGINE );
                for(int j = 0; j < Agents.size(); j++)
                {
                    if ( pos <= data[j].Upper_Bound && pos >= data[j].Lower_Bound && j != firstParent)
                    {
                        secondParent = j;
                        breaker = true;
                        break;
                    }
                }
                if ( breaker )
                    break;
            }


            Agent child1 = Agents[ firstParent ].CrossOver( Agents[ secondParent ], 1 ),
                child2 = Agents[ secondParent ].CrossOver( Agents[ firstParent ], 1 );

            child1.Mutate();
            child2.Mutate();

            Children.push_back( child1 );
            Children.push_back( child2 );

            totalFitness = 0;
        }

        Agents = Children;
    }

    static void AssignFitness(std::vector<Agent> &Agents)
    {
        for(int i = 0; i < Agents.size(); i++)
        {
            Agents[i].Fitness = Agents[i].LifeSpan * Agents[i].MaxDistance * Agents[i].MaxDisplacement;
            if ( Agents[i].Fitness < 0 ) Agents[i].Fitness = 0;
        }

        float totalFitness = 0;
        for(int i = 0; i < Agents.size(); i++)
        {
            totalFitness += Agents[i].Fitness;
        }

        for(int i = 0; i < Agents.size(); i++)
        {
            Agents[i].Fitness /= totalFitness;
        }
    }

    bool touch( const std :: vector <Edge> &edges )
    {
        sf::Vector2f temp;
        for(int i = 0; i < edges.size(); i++)
        {
            if ( Intersection(e1, edges[i], temp) )
                return true;
            else if ( Intersection(e2, edges[i], temp) )
                return true;
            else if ( Intersection(e3, edges[i], temp) )
                return true;
            else if ( Intersection(e4, edges[i], temp) )
                return true;
        }

        return false;
    }

    void translate(float stepSize)
    {
        setDirection();

        sf::Vector2f step = direction * -stepSize;

        e1.p1 += step; e1.p2 += step;
        e2.p1 += step; e2.p2 += step;
        e3.p1 += step; e3.p2 += step;
        e4.p1 += step; e4.p2 += step;

        center += step; front += step;

        for(int i = 0; i < rays.size(); i++)
        {
            rays[i].p1 += step;
            rays[i].p2 += step;
        }
    }

    void rotate(float angle)
    {
        e1.p1 = transform(e1.p1, center, angle);
        e1.p2 = transform(e1.p2, center, angle);

        e2.p1 = transform(e2.p1, center, angle);
        e2.p2 = transform(e2.p2, center, angle);

        e3.p1 = transform(e3.p1, center, angle);
        e3.p2 = transform(e3.p2, center, angle);

        e4.p1 = transform(e4.p1, center, angle);
        e4.p2 = transform(e4.p2, center, angle);

        front = transform(front, center, angle);

        for(int i = 0; i < rays.size(); i++)
        {
            rays[i].p2 = transform(rays[i].p2, center, angle);
        }

        setDirection();
    }

    sf::VertexArray drawable()
    {
        sf::VertexArray array;

        array.append(sf::Vertex(e1.p1));
        array.append(sf::Vertex(e1.p2));
        array.append(sf::Vertex(e2.p1));
        array.append(sf::Vertex(e2.p2));
        array.append(sf::Vertex(e3.p1));
        array.append(sf::Vertex(e3.p2));
        array.append(sf::Vertex(e4.p1));
        array.append(sf::Vertex(e4.p2));

        array.setPrimitiveType(sf::PrimitiveType::Lines);

        return array;
    }

    std::vector<sf::Vector2f> inter(const std::vector<Edge> &entites)
    {
        std::vector<sf::Vector2f> array;

        for(int i = 0; i < rays.size(); i++)
        {
            sf::Vector2f point, prevPoint = rays[i].p2;

            for(int j = 0; j < entites.size(); j++)
            {
                if ( Intersection(rays[i], entites[j], point) )
                {
                    Edge e1, e2; e1.p1 = center; e1.p2 = point;
                    e2.p1 = center; e2.p2 = prevPoint;

                    if ( e1.magnitude() < e2.magnitude() && e1.magnitude() < MaxDist )
                    {   
                        prevPoint = point;
                    }
                }
            }
            
            array.push_back(prevPoint);
        }

        return array;
    }
};
