#pragma once

#include <vector>

struct Points {
    int N;
    int curInd;

    std::vector<float> px;
    std::vector<float> py;
    std::vector<float> pz;

    auto arrays() {
        return std::make_tuple(
            &px, &py, &pz
        );
    }

    Points(int nVal) : N(nVal){
        std::apply([&](auto*... arr) {
            (arr->resize(N, 0.0f), ...);
        }, arrays());
    }
};
