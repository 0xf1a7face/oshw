#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <condition_variable>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <vector>

template<class T>
class BufferedChannel {
public:
  explicit BufferedChannel(int size) : m_cap(size) {}

  void Send(T value) {
    std::unique_lock guard(m_mutex);
    m_send_cv.wait(guard, [&]() {
      if (m_closed) {
        throw std::runtime_error("Sending to closed channel");
      }
      return m_buf.size() != m_cap;
    });

    m_buf.push_back(std::move(value));
    guard.unlock();
    m_recv_cv.notify_one();
  }

  T Recv() {
    std::unique_lock guard(m_mutex);
    m_recv_cv.wait(guard, [&]() {
      if (!m_buf.empty()) {
        return true;
      }
      if (m_closed) {
        throw std::runtime_error("Reading from closed channel");
      }
      return false;
    });

    auto val = std::move(m_buf.front());
    m_buf.pop_front();
    guard.unlock();
    m_send_cv.notify_one();

    return val;
  }

  bool IsEmpty() {
    std::unique_lock guard(m_mutex);
    return m_buf.empty();
  }

  bool IsFull() {
    std::unique_lock guard(m_mutex);
    return m_buf.size() == m_cap;
  }

  void Close() {
    std::unique_lock guard(m_mutex);

    m_closed = true;
    m_recv_cv.notify_all();
    m_send_cv.notify_all();
  }

private:
  std::mutex m_mutex;
  std::condition_variable m_recv_cv;
  std::condition_variable m_send_cv;

  // Guarded by `m_mutex`
  bool m_closed = false;
  std::deque<T> m_buf;
  size_t m_cap;
};

#endif // BUFFERED_CHANNEL_H_
