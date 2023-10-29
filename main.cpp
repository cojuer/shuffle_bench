#include <spdlog/spdlog.h>
#include <random>
#include <ranges>
#include <benchmark/benchmark.h>

#include "third_party/pcg-cpp/include/pcg_random.hpp"

namespace stdr = std::ranges;

const auto NUM_ELEM = 10000000;

auto mt_rng(std::vector<int>& vec) -> void {
    std::mt19937 rng(0);
    stdr::shuffle(vec, rng);
}

static void BM_MT19937(benchmark::State& state) {
    std::vector<int> vec(NUM_ELEM);
    stdr::generate(vec, [n=0]() mutable { return n++; });

    for (auto _ : state) {
        mt_rng(vec);
    }
}
BENCHMARK(BM_MT19937);

auto pcg(std::vector<int>& vec) -> void {
    pcg32 rng(0);
    
    stdr::shuffle(vec, rng);
}

static void BM_PCG(benchmark::State& state) {
    std::vector<int> vec(NUM_ELEM);
    stdr::generate(vec, [n=0]() mutable { return n++; });

    for (auto _ : state) {
        pcg(vec);
    }
}
BENCHMARK(BM_PCG);

auto pcg_choose_next_one(const std::vector<int>& vec, double frac_el, std::vector<int>& res) {
    std::vector<bool> visited(NUM_ELEM, false);

    pcg32 rng(0);
    
    size_t num_el = std::round(frac_el * NUM_ELEM);

    for (size_t i = 0; i < num_el; ++i) {
        size_t idx = rng() % NUM_ELEM;
        while (visited[idx]) {
            idx = rng() % NUM_ELEM;
        }

        visited[idx] = true;
        res[i] = vec[idx];
    }
}

static void BM_PCG_NEXT_ONE(benchmark::State& state) {
    auto vec = std::vector<int>(NUM_ELEM);
    stdr::generate(vec, [n=0]() mutable { return n++; });

    auto res = std::vector<int>(NUM_ELEM);
    auto fracs = std::vector<double>{0.25, 0.5, 0.75, 1.0};
    for (auto _ : state) {
        pcg_choose_next_one(vec, fracs.at(state.range(0)), res);
    }
}
BENCHMARK(BM_PCG_NEXT_ONE)->DenseRange(0, 3, 1);

auto pcg_choose_next(const std::vector<int>& vec, double frac_el, std::vector<int>& res) {
    std::vector<bool> visited(NUM_ELEM, false);

    pcg32 rng(0);
    
    size_t num_el = std::round(frac_el * NUM_ELEM);

    for (size_t i = 0; i < num_el; ++i) {
        size_t idx = rng() % NUM_ELEM;
        while (visited[idx]) {
            idx = rng() % NUM_ELEM;
        }

        visited[idx] = true;
        res[i] = idx;
    }

    for (size_t i = 0; i < num_el; ++i) {
        res[i] = vec[res[i]];
    }
}

static void BM_PCG_NEXT(benchmark::State& state) {
    std::vector<int> vec(NUM_ELEM);
    stdr::generate(vec, [n=0]() mutable { return n++; });
    
    auto res = std::vector<int>(NUM_ELEM);
    auto fracs = std::vector<double>{0.25, 0.5, 0.75, 1.0};
    for (auto _ : state) {
        pcg_choose_next(vec, fracs.at(state.range(0)), res);
    }
}
BENCHMARK(BM_PCG_NEXT)->DenseRange(0, 3, 1);

auto pcg_fisher_yates(std::vector<int> vec, double frac_el) -> void {
    std::vector<bool> visited(NUM_ELEM, false);

    pcg32 rng(0);
    
    size_t num_el = std::round(frac_el * NUM_ELEM);

    for (size_t i = num_el - 1; i > 0; --i) {
        size_t idx = rng() % (i + 1);
        std::swap(vec[i], vec[idx]);
    }
}

static void BM_PCG_FISHER_YATES(benchmark::State& state) {
    std::vector<int> vec(NUM_ELEM);
    stdr::generate(vec, [n=0]() mutable { return n++; });

    auto fracs = std::vector<double>{0.25, 0.5, 0.75, 1.0};
    for (auto _ : state) {
        pcg_fisher_yates(vec, fracs.at(state.range(0)));
    }
}
BENCHMARK(BM_PCG_FISHER_YATES)->DenseRange(0, 3, 1);

auto pcg_fisher_yates_uniform(std::vector<int> vec, double frac_el) -> void {
    std::vector<bool> visited(NUM_ELEM, false);

    pcg32 rng(0);
    
    size_t num_el = std::round(frac_el * NUM_ELEM);

    for (size_t i = num_el - 1; i > 0; --i) {
        auto dist = std::uniform_int_distribution<int>(0, i);
        std::swap(vec[i], vec[dist(rng)]);
    }
}

static void BM_PCG_FISHER_YATES_UNI(benchmark::State& state) {
    std::vector<int> vec(NUM_ELEM);
    stdr::generate(vec, [n=0]() mutable { return n++; });

    auto fracs = std::vector<double>{0.25, 0.5, 0.75, 1.0};
    for (auto _ : state) {
        pcg_fisher_yates_uniform(vec, fracs.at(state.range(0)));
    }
}
BENCHMARK(BM_PCG_FISHER_YATES_UNI)->DenseRange(0, 3, 1);

BENCHMARK_MAIN();