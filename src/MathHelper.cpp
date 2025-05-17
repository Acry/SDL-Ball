#include "MathHelper.h"
#include <random>

float random_float(const float total, const float negative) {
    thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution dist(-negative, total - negative);
    return dist(rng);
}