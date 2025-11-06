#include <cassert>
#include <iostream>
#include <list>
#include <thread>

#include "buffered_channel.h"

using Chan = BufferedChannel<int>;

int main() {
  {
    auto chan = Chan(1);
    chan.Send(123);
    assert(chan.IsFull());
    assert(chan.Recv() == 123);
    assert(chan.IsEmpty());

    std::cout << "Sanity ok\n";
  }

  auto rounds = 100;
  auto chan_cap = 20;
  auto threads = 101;

  for (int round = 0; round < rounds; ++round) {
    auto thread_buf = std::vector<std::thread>();
    auto list = std::list<int>();
    auto chan = Chan(chan_cap);

    for (int i = 1; i <= threads; ++i) {
      thread_buf.emplace_back([&, i]() { chan.Send(i); });
    }

    for (int i = 1; i <= threads; ++i) {
      list.push_back(0);
      auto *item = &list.back();
      thread_buf.emplace_back([&, item]() { *item = chan.Recv(); });
    }

    for (auto &thread : thread_buf) {
      thread.join();
    }

    assert(chan.IsEmpty());

    size_t sum = 0;
    for (auto &item : list) {
      sum += item;
    }

    assert(sum == (threads + 1) * threads / 2);

    std::cout << "\e[2K\r"; // Clears current line and returns carriage
    std::cout << "Fuzzing round " << round;
    std::cout.flush();
  }

  std::cout << "\e[2K\r";
  printf("Completed %i fuzzing rounds chan_cap=%i, threads=%i\n", rounds, chan_cap, threads);
}
