#include "blocking_queue.h"
#include <iostream>
#include <thread>
#include <vector>

BlockingQueue<char> block_queue(1000);

void writee() {
    for (int i = 0; i < 300; i++) {
        block_queue.push('c');
    }
}

void wirteNoWait(){
    for (int i = 0; i < 300; i++) {
        block_queue.pushNoWait('c');
    }
}

void readd() {
    for (int i = 0; i < 1500; i++) {
        block_queue.pop();
    }
}

void readFor() {
    for (int i = 0; i < 1500; i++) {
        block_queue.popFor(std::chrono::milliseconds(3));
    }
}

int main() {
    std::vector<std::thread> threads_write;
    std::vector<std::thread> threads_write_nowait;
    std::thread tread1(readd);
    std::thread tread2(readFor);
    for (int i = 0; i < 10; i++) {
        threads_write.emplace_back(std::thread(writee));
        threads_write_nowait.emplace_back(std::thread(wirteNoWait));
    }
    
    for (auto &x : threads_write) {
        x.join();
    }

    for (auto &x : threads_write_nowait) {
        x.join();
    }
    std::cout << block_queue.size() << std::endl;

    tread1.join();
    tread2.join();

    std::cout << block_queue.size() << std::endl;

    return 0;
}