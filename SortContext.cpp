#include "SortContext.h"
#include "Config.h"
#include <algorithm>
#include <cstdlib>

void SortContext::swap_counted(std::vector<int>& v, int a, int b, long long& swapCount) {
    std::swap(v[static_cast<size_t>(a)], v[static_cast<size_t>(b)]);
    swapCount++;
}

void SortContext::BeginSortRun(SortAlgorithm a) {
    algo = a;
    comparisons = 0;
    swaps = 0;
    paused = false;
    done = false;
    highlightA = highlightB = -1;

    const int n = static_cast<int>(values.size());
    if (n <= 1) {
        done = true;
        return;
    }

    bub_i = 0;
    bub_j = 0;
    ins_i = 1;
    ins_j = 1;
    heap_phase = HeapPhase::Building;
    heap_build_k = n / 2 - 1;
    heap_size = n;
    heap_sift_pos = -1;
}

void SortContext::Scramble() {
    values.resize(Config::BarCount);
    for (int& v : values) {
        v = Config::BarMin + (rand() % (Config::BarMax - Config::BarMin + 1));
    }
    BeginSortRun(algo);
}

bool SortContext::sift_down_once(int& pos, int bound) {
    const int left = 2 * pos + 1;
    const int right = 2 * pos + 2;
    int largest = pos;

    if (left < bound) {
        comparisons++;
        if (values[static_cast<size_t>(left)] > values[static_cast<size_t>(largest)]) {
            largest = left;
        }
    }
    if (right < bound) {
        comparisons++;
        if (values[static_cast<size_t>(right)] > values[static_cast<size_t>(largest)]) {
            largest = right;
        }
    }
    highlightA = pos;
    highlightB = largest;
    if (largest != pos) {
        swap_counted(values, pos, largest, swaps);
        pos = largest;
        return true;
    }
    return false;
}

void SortContext::do_bubble() {
    const int n = static_cast<int>(values.size());
    highlightA = highlightB = -1;
    if (bub_j > n - 2 - bub_i) {
        bub_i++;
        bub_j = 0;
        if (bub_i >= n - 1) {
            done = true;
            return;
        }
    }
    highlightA = bub_j;
    highlightB = bub_j + 1;
    comparisons++;
    if (values[static_cast<size_t>(bub_j)] > values[static_cast<size_t>(bub_j + 1)]) {
        swap_counted(values, bub_j, bub_j + 1, swaps);
    }
    bub_j++;
}

void SortContext::do_insertion() {
    const int n = static_cast<int>(values.size());
    highlightA = highlightB = -1;
    if (ins_i >= n) {
        done = true;
        return;
    }
    if (ins_j > 0 && values[static_cast<size_t>(ins_j - 1)] > values[static_cast<size_t>(ins_j)]) {
        highlightA = ins_j - 1;
        highlightB = ins_j;
        comparisons++;
        swap_counted(values, ins_j - 1, ins_j, swaps);
        ins_j--;
        return;
    }
    if (ins_j > 0) {
        comparisons++;
    }
    ins_i++;
    if (ins_i >= n) {
        done = true;
        return;
    }
    ins_j = ins_i;
}

void SortContext::do_heap() {
    const int n = static_cast<int>(values.size());
    highlightA = highlightB = -1;

    if (heap_sift_pos >= 0) {
        const int bound = (heap_phase == HeapPhase::Building) ? n : heap_size;
        const bool sunk = sift_down_once(heap_sift_pos, bound);
        if (!sunk) {
            heap_sift_pos = -1;
            if (heap_phase == HeapPhase::Building) {
                heap_build_k--;
            }
        }
        return;
    }

    if (heap_phase == HeapPhase::Building) {
        if (heap_build_k < 0) {
            heap_phase = HeapPhase::Extracting;
            heap_size = n;
        } else {
            heap_sift_pos = heap_build_k;
        }
        return;
    }

    if (heap_size <= 1) {
        done = true;
        return;
    }
    highlightA = 0;
    highlightB = heap_size - 1;
    swap_counted(values, 0, heap_size - 1, swaps);
    heap_size--;
    if (heap_size <= 1) {
        done = true;
        return;
    }
    heap_sift_pos = 0;
}

void SortContext::AdvanceOne() {
    if (done || paused || values.size() <= 1) {
        return;
    }
    switch (algo) {
    case SortAlgorithm::Bubble:
        do_bubble();
        break;
    case SortAlgorithm::Insertion:
        do_insertion();
        break;
    case SortAlgorithm::Heap:
        do_heap();
        break;
    }
}
