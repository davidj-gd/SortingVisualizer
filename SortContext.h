#pragma once

#include "AppTypes.h"

#include <vector>

struct SortContext {
    SortAlgorithm algo = SortAlgorithm::Bubble;
    std::vector<int> values;
    long long comparisons = 0;
    long long swaps = 0;
    bool paused = false;
    bool done = false;

    int highlightA = -1;
    int highlightB = -1;

    int bub_i = 0;
    int bub_j = 0;

    int ins_i = 1;
    int ins_j = 1;

    enum class HeapPhase { Building, Extracting } heap_phase = HeapPhase::Building;
    int heap_build_k = 0;
    int heap_size = 0;
    int heap_sift_pos = -1;

    void BeginSortRun(SortAlgorithm a);
    void Scramble();
    void AdvanceOne();

private:
    void do_bubble();
    void do_insertion();
    void do_heap();
    bool sift_down_once(int& pos, int bound);
    static void swap_counted(std::vector<int>& v, int a, int b, long long& swapCount);
};
