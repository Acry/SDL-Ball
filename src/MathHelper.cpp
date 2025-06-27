// MathHelper.cpp
#include "MathHelper.h"
#include <random>

float randomFloat(const float total, const float negative) {
    thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution dist(-negative, total - negative);
    return dist(rng);
}

int randomInt(const int min, const int max) {
    thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
