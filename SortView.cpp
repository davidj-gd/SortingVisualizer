#include "SortView.h"
#include "Config.h"
#include "SortContext.h"
#include "raylib.h"
#include <algorithm>
#include <string>

static BarVisual bar_color_tag(int idx, const SortContext& ctx) {
    if (ctx.done) {
        return BarVisual::Sorted;
    }
    if (idx == ctx.highlightA || idx == ctx.highlightB) {
        if (ctx.algo == SortAlgorithm::Heap && ctx.heap_phase == SortContext::HeapPhase::Extracting &&
            ctx.heap_sift_pos < 0 && (idx == 0 || idx == ctx.heap_size)) {
            return BarVisual::Swap;
        }
        return BarVisual::Compare;
    }
    if (ctx.algo == SortAlgorithm::Heap && ctx.heap_phase == SortContext::HeapPhase::Extracting &&
        idx >= ctx.heap_size) {
        return BarVisual::Sorted;
    }
    return BarVisual::Default;
}

static Color make_rgb(BarVisual v) {
    switch (v) {
    case BarVisual::Compare:
        return {230, 180, 50, 255};
    case BarVisual::Swap:
        return {220, 60, 60, 255};
    case BarVisual::Sorted:
        return {80, 180, 120, 255};
    default:
        return {70, 120, 200, 255};
    }
}

void RedrawBarStrip(const SortContext& ctx) {
    const int n = static_cast<int>(ctx.values.size());
    if (n <= 0) {
        return;
    }
    const float panelTop = 110.0f;
    const float panelHeight = static_cast<float>(Config::ScreenHeight) - panelTop - 40.0f;
    const float barWidth = static_cast<float>(Config::ScreenWidth - 80) / static_cast<float>(n);
    int vmax = ctx.values[0];
    for (int v : ctx.values) {
        vmax = (std::max)(vmax, v);
    }
    const float baseY = static_cast<float>(Config::ScreenHeight) - 40.0f;

    for (int i = 0; i < n; i++) {
        BarVisual cls = bar_color_tag(i, ctx);
        if (cls == BarVisual::Default && ctx.algo == SortAlgorithm::Bubble && !ctx.done && ctx.bub_i > 0 &&
            i >= n - ctx.bub_i) {
            cls = BarVisual::Sorted;
        }
        if (cls == BarVisual::Default && ctx.algo == SortAlgorithm::Insertion && !ctx.done && i < ctx.ins_i &&
            i != ctx.ins_j && i != ctx.ins_j - 1) {
            cls = BarVisual::Sorted;
        }

        const float h = (static_cast<float>(ctx.values[static_cast<size_t>(i)]) / static_cast<float>(vmax)) *
                        (panelHeight - 10.0f);
        const float x = 40.0f + static_cast<float>(i) * barWidth;
        const float y = baseY - h;
        DrawRectangle(static_cast<int>(x + 1), static_cast<int>(y), static_cast<int>(barWidth - 2.0f),
                      static_cast<int>(h), make_rgb(cls));
    }
}

void DrawHelpOverlay(const SortContext& ctx, Scene scene) {
    const char* algoName = "Bubble Sort";
    if (ctx.algo == SortAlgorithm::Insertion) {
        algoName = "Insertion Sort";
    } else if (ctx.algo == SortAlgorithm::Heap) {
        algoName = "Heap Sort";
    }

    DrawText(algoName, 40, 20, 28, RAYWHITE);
    DrawText("TAB: Menu / Sort   1/2/3: Algorithm   R: shuffle   SPACE: pause", 40, 52, 18, LIGHTGRAY);

    std::string stats =
        "Comparisons: " + std::to_string(ctx.comparisons) + "    Swaps: " + std::to_string(ctx.swaps);
    if (ctx.done) {
        stats += "    (finished)";
    } else if (ctx.paused) {
        stats += "    (paused)";
    }
    DrawText(stats.c_str(), 40, 78, 18, SKYBLUE);

    if (scene == Scene::Menu) {
        DrawRectangle(0, 0, Config::ScreenWidth, Config::ScreenHeight, {0, 0, 0, 140});
        DrawText("Sorting Visualizer — G", Config::ScreenWidth / 2 - 200, Config::ScreenHeight / 2 - 80, 32,
                 RAYWHITE);
        DrawText("Press TAB to enter sorting view.", Config::ScreenWidth / 2 - 220, Config::ScreenHeight / 2 - 20, 22,
                 LIGHTGRAY);
        DrawText("Keys 1: Bubble   2: Insertion   3: Heap", Config::ScreenWidth / 2 - 240,
                 Config::ScreenHeight / 2 + 20, 22, LIGHTGRAY);
    }
}
