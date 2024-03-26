#pragma once
#include <vector>

// Intended to use to combine multiple vectors into one

std::vector<StatementIR> concatenate(std::vector<StatementIR>& first) {
    return std::move(first);
}

std::vector<StatementIR> concatenate(std::vector<StatementIR>&& first) {
    return std::move(first);
}

template<typename ItemType>
std::vector<StatementIR> concatenate(ItemType&& first) {
    auto vec = std::vector<StatementIR>();
    vec.emplace_back(std::move(first));

    return std::move(vec);
}

template<typename ItemType, typename... Ts>
std::vector<StatementIR> concatenate(ItemType&& first, Ts&&... args) {
    auto vec = std::vector<StatementIR>();
    vec.emplace_back(std::move(first));

    for (auto& elem : concatenate(args...)) {
        vec.emplace_back(std::move(elem));
    }

    return std::move(vec);
}

template<typename ItemType, typename... Ts>
std::vector<StatementIR> concatenate(std::vector<ItemType>& first, Ts&&... args) {
    for (auto& elem : concatenate(args...)) {
        first.emplace_back(std::move(elem));
    }
    return std::move(first);
}

template<typename ItemType, typename... Ts>
std::vector<StatementIR> concatenate(std::vector<ItemType>&& first, Ts&&... args) {
    for (auto& elem : concatenate(args...)) {
        first.emplace_back(std::move(elem));
    }
    return std::move(first);
}
