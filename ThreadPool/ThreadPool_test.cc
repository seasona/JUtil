#include "ThreadPool.h"
#include <chrono>
#include <cstdio>
#include <iostream>

using namespace std;

int calandprint(int x) {
    printf("Hello %d ", x);
    this_thread::sleep_for(chrono::seconds(1));
    printf("World %d\n", x);
    return x * x;
}

int main() {
    vector<future<int>> results;

    {
        ThreadPool pool(4);

        for (int i = 0; i < 8; i++) {
            results.emplace_back(pool.insert(calandprint, i));
        }
    }

    for (auto&& res : results) {
        cout << res.get() << " ";
    }
    cout << endl;

    return 0;
}