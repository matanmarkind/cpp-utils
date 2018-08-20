#include <mutex>
#include <atomic>
#include <array>

template <typename T>
class ProducerConsumer {
  // Class designed for 2 threads to communicate. One produces output that the
  // other consumes.
public:
  T* GetContentsForProduction() {
    std::unique_lock<std::mutex> lock(mtx_);

    
  }
  T* GetContentsForConsumption() {
  }
private:
  std::mutex mtx_;
  struct LockedContents {
    T* GetContents() {
      std::unique_lock<std::mutex> lock(mtx_);
    }
    T contents_;
    std::mutex mtx_;
  };
  std::array<Contents, 2> contents_;
  std::atomic_bool consumer_version_{0};
};
