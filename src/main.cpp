#include <ginv/clustering/decaying_max_heap.hpp>
#include <ginv/istanbul_ein_dataset.hpp>
#include <iostream>
#include <osigma/oconnections.hpp>
#include <osigma/ograph.hpp>
#include <osigma/onodes.hpp>
#include <vector>

int main(int argc, char** argv)
{

    clustering::DecayingMaxHeap<float, int, int> heap(10);

    std::cout << heap.describe() << std::endl;

    heap.push(10, 15, 4);

    std::cout << heap.show_element(0) << std::endl;
    std::cout << heap.show_element(1) << std::endl;
    std::cout << heap.show_element(2) << std::endl;

    return 0;

    istanbul::IstanbulEinDatasetBin istanbul_dataset("./data/istanbul");
    std::cout << istanbul_dataset.describe() << std::endl;

    return 0;
}