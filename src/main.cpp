#include <ginv/clustering/decaying_max_heap.hpp>
#include <ginv/istanbul_ein_dataset.hpp>
#include <iostream>
#include <osigma/oconnections.hpp>
#include <osigma/ograph.hpp>
#include <osigma/onodes.hpp>
#include <vector>
#include <tuple>

int main(int argc, char** argv)
{

    clustering::DecayingMaxHeap<float, int, int> heap(10);

    std::cout << heap.describe() << std::endl;

    heap.push(10, 15, 4);
    heap.push(1, 11, 12.22);
    auto top_key = heap.top_key();
    std::get<0>(top_key) = 1333;
    heap.push(top_key, 17);
    auto top = heap.top();
    std::get<1>(top) = 1333;
    std::get<2>(top) = 0.1;
    heap.push(top);

    std::cout << heap.describe_element(0) << std::endl;
    std::cout << heap.describe_element(1) << std::endl;
    std::cout << heap.describe_element(2) << std::endl;
    std::cout << heap.describe_element(3) << std::endl;
    std::cout << heap.describe_element(4) << std::endl;

    auto show = [](auto&... values) { return ((std::to_string(values) + ", ") + ...); };
    auto pop_show = [&heap, &show]() {
        
        auto result = heap.pop();
        std::cout<<std::apply(show, result)<<std::endl;
        std::cout<<heap.size()<<std::endl;
    };

    pop_show();
    pop_show();
    pop_show();
    pop_show();

    std::cout << "-----------------" << std::endl;
    std::cout << heap.describe_element(0) << std::endl;
    std::cout << heap.describe_element(1) << std::endl;
    std::cout << heap.describe_element(2) << std::endl;
    std::cout << heap.describe_element(3) << std::endl;
    std::cout << heap.describe_element(4) << std::endl;

    return 0;

    istanbul::IstanbulEinDatasetBin istanbul_dataset("./data/istanbul");
    std::cout << istanbul_dataset.describe() << std::endl;

    return 0;
}