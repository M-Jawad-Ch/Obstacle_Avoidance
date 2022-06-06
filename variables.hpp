#pragma once

#include"Headers/layer.hpp"

const float PI = 22.0 / 7.0;
const float STEPS_PER_SECOND = 50;
const float RADIANS_PER_SECOND =  PI / 2.0;
const float AGENT_SIZE = 5;

const int RAY_COUNT = 32;
const int MAX_DISTANCE = 200;
const int AGENT_COUNT = 200;
const int LAYER_COUNT = 2;
const int MAX_MUTATED_WEIGHTS = 8;

const int SECOND_LAYER_SIZE = 8;
const int CLASSES = 4;
const int FRAMES_PER_SECOND = 25;

int SCREEN_WIDTH = 700;
int SCREEN_HEIGHT = 400;

int MAX_SIMULATION_DURATION = 10;
int MAX_TICKS_PER_AGENT = MAX_SIMULATION_DURATION * FRAMES_PER_SECOND;

int ALIVE_AGENTS = AGENT_COUNT;

const int THREAD_COUNT = 4;

std::vector<sf::Vector2f> SPAWNS;

std::default_random_engine ENGINE;

const int SAVE_AFTER_X_GENERATIONS = 10;
