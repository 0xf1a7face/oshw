#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <span>
#include <thread>
#include <tuple>
#include <vector>

struct Matrix {
public:
  size_t height;
  size_t width;
  std::vector<int> inner;

  static Matrix random(size_t height, size_t width) {
    std::vector<int> inner(height * width);
    for (auto &e : inner) {
      // e = 0;
      e = rand() % 10;
      // std::cout << e << "\n";
    }

    return Matrix{
        .height = height,
        .width = width,
        .inner = std::move(inner),
    };
  }

  Matrix zeroed() {
    auto other = *this;
    for (auto &e : inner) {
      e = 0;
    }
    return other;
  }

  std::span<int> operator[](size_t i) {
    return std::span<int>(inner.begin() + this->width * i,
                          inner.begin() + this->width * (i + 1));
  }

  friend std::ostream &operator<<(std::ostream &out, Matrix &self) {
    // out << self.height << "x" << self.width << "\n";
    // out << self.inner.size() << "\n";
    out << "[\n";
    for (size_t i = 0; i < self.height; ++i) {
      out << "  ";
      for (size_t j = 0; j < self.width; ++j) {
        if (j != 0) {
          out << ", ";
        }
        out << self[i][j];
      }
      out << ";\n";
    }
    out << "]\n";

    return out;
  }
};

struct Block {
  size_t i_min;
  size_t i_max;
  size_t j_min;
  size_t j_max;
};

struct MulContext {
  Matrix left, right, res;

  static MulContext with(Matrix l, Matrix r) {
    auto m = MulContext{
        .left = std::move(l),
        .right = std::move(r),
    };
    m.resize_res();

    return std::move(m);
  }

  void resize_res() {
    size_t res_len = this->left.height * this->right.width;
    if (this->res.inner.size() != res_len) {
      this->res.height = this->left.height;
      this->res.width = this->right.width;
      this->res.inner.resize(res_len);
    }

    assert(this->left.width == this->right.height);
  }

  void compute_block(Block d) {
    for (size_t i = d.i_min; i < d.i_max; ++i) {
      for (size_t j = d.j_min; j < d.j_max; ++j) {
        int64_t res = 0;
        for (size_t k = 0; k < this->left.width; ++k) {
          res += this->left[i][k] * this->right[k][j];
        }
        this->res[i][j] = res;
      }
    }
  }
};

struct Args {
  MulContext *ctx;
  Block dims;
};

#ifdef USE_STDTHREAD
struct Thread {
  std::thread thread;

  Thread(Args args) {
    thread = std::thread([=]() { args.ctx->compute_block(args.dims); });
  }

  void join() { thread.join(); }
};
#endif

#ifdef USE_PTHREAD
#include <pthread.h>
struct Thread {
  pthread_t pthread;
  std::unique_ptr<Args> arg;

  static void *run_pthread(void *data) {
    Args *args = (Args *)data;
    args->ctx->compute_block(args->dims);

    return NULL;
  }

  Thread(Args args) {
    arg = std::make_unique<Args>(args);

    int res = pthread_create(&pthread, NULL, run_pthread, &*arg);
    assert(res == 0);
  }

  void join() { pthread_join(pthread, NULL); }
};
#endif

#ifdef USE_WINTHREAD
#include <windows.h>
struct Thread {
  HANDLE thread;
  std::unique_ptr<Args> arg;

  static DWORD WINAPI run_winthread(LPVOID data) {
    Args *args = (Args *)data;
    args->ctx->compute_block(args->dims);

    return NULL;
  }

  Thread(Args args) {
    arg = std::make_unique<Args>(args);

    thread = CreateThread(NULL, 0, run_winthread, &*arg, 0, NULL);
    assert(thread != 0);
  }

  void join() { WaitForSingleObject(thread, INFINITE); }
};
#endif

struct Pool {
  MulContext *ctx;
  std::vector<Thread> threads;
  Matrix thread_matrix;

  static Pool with(MulContext *ctx) {
    return Pool{
        .ctx = ctx,
        .thread_matrix = ctx->res.zeroed(),
    };
  }

  void spawn(Block d) {
    auto *m = ctx;

    threads.emplace_back(Args{m, d});

#ifdef THREAD_MATRIX
    for (size_t i = d.i_min; i < d.i_max; ++i) {
      for (size_t j = d.j_min; j < d.j_max; ++j) {
        assert(thread_matrix[i][j] == 0);
        thread_matrix[i][j] = threads.size();
      }
    }
#endif
  }

  void wait() {
    for (auto &thread : threads) {
      thread.join();
    }
  }
};

int main(int argc, char **argv) {
  srand(time(0));

  size_t n = 0;
  size_t k = 0;
  size_t m = 0;
  size_t stride = 100;

  if (argc >= 2) {
    n = strtoul(argv[1], NULL, 10);
  }
  if (argc == 3) {
    k = n;
    m = strtoul(argv[2], NULL, 10);
  }
  if (argc >= 4) {
    k = strtoul(argv[2], NULL, 10);
    m = strtoul(argv[3], NULL, 10);
  }
  if (argc == 5) {
    stride = strtoul(argv[4], NULL, 10);
  }
  if (argc > 6) {
    std::cout << "Too many arguments\n";
    return 1;
  }

  if (n == 0) {
    n = 200;
    k = n;
    m = n;
  } else if (m == 0) {
    k = n;
    m = n;
  }

  auto c = std::make_unique<MulContext>(
      MulContext::with(Matrix::random(n, k), Matrix::random(k, m)));

  // std::cout << "Left:\n";
  // std::cout << c->left;
  // std::cout << "Right:\n";
  // std::cout << c->right;

  auto p = Pool::with(&*c);

  auto h = c->res.height;
  auto w = c->res.width;

  auto start = std::chrono::high_resolution_clock::now();

  for (size_t i = 0; i < h; i += stride) {
    for (size_t j = 0; j < w; j += stride) {
      auto d = Block{.i_min = i,
                     .i_max = std::min(i + stride, h),
                     .j_min = j,
                     .j_max = std::min(j + stride, w)};
      p.spawn(d);
    }
  }
  p.wait();

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  printf("Multiplied %lux%lu by %lux%lu (%lu batches with side %lu)\n", n, k, k,
         m, p.threads.size(), stride);

  printf(
      "Time: %.03f (ms) including overheads on spawning and joining threads\n",
      (float)duration.count() / 1000);

  // std::cout << "Result:\n";
  // std::cout << c->res;

#ifdef THREAD_MATRIX
  std::cout << "Thread matrix:\n";
  std::cout << p.thread_matrix;
#endif

  // Sanity check
  auto res = c->res;
  for (auto &e : c->res.inner) {
    e = 0;
  }
  auto d = Block{
      .i_min = 0, .i_max = c->res.height, .j_min = 0, .j_max = c->res.width};
  c->compute_block(d);
  assert(res.inner == c->res.inner);
}
