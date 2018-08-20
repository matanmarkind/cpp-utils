#include <mutex>
#include <atomic>
#include <utility>
#include <array>

#include "ThreadsafeContents.h"

template <typename T>
class RwActiveStandby {
  // This class is designed for a usage case where a single thread, the
  // manager, sets a state that multiple threads, workers, can read from.
  // There are 2 copies of this state. While the manager updates one version,
  // no workers can access it. When the manger thread is done, it "returns" the
  // contents so that the workers can switch to reading from the updated state.
  // When all workers have switched to the newer state, the manger can then
  // take exclusive control of the other copy and update it.
  //
  // The manager must destroy the old Writer it has before attempting to get
  // a new one. Only once a Writer is destroyed can the workers switch to
  // reading from the newly updated state.
  //
  // Holding both an active and a standby version doubles the memory needed
  // to hold the state the workers read from, but it means that they will never
  // be blocked by the manager updating the state.
  
  // Constructors.
  RwActiveStandby() : contents_({T(), T()}) {}
  RwActiveStandby(const T& t) {
    contents_[0] = t;
    contents_[1] = t;
  }
  RwActiveStandby(T&& t1, T&& t2) {
    // If the contents themselves are not copyable the user can move in 2
    // versions. They should the same, and no promise is made as to which
    // starts off as standby or active.
    contents_[0] = std::move(t1);
    contents_[1] = std::move(t2);
  }
  template <typename... Args>
  RwActiveStandby(const Args&&... args) {
    // The caller can have both copies created automatically by passing in the
    // arguments to create the contents. Takes const parameters so that both
    // versions will be initialized to be the same.
    new (&(contents_[0])) T(std::forward<Args>(args)...);
    new (&(contents_[1])) T(std::forward<Args>(args)...);
  }
  // No copying or moving.
  RwActiveStandby(const RwActiveStandby&) = delete;
  RwActiveStandby(RwActiveStandby) = delete;
  RwActiveStandby& operator=(const RwActiveStandby&) = delete;
  RwActiveStandby& operator=(RwActiveStandby&&) = delete;

  // Return a Reader which allows multiple threads to read the contents. Should
  // never block.
  RwLock::Reader GetReader() {
    // Try to get the standby version. If available it means the manager has
    // finished updating it, and it will be more up to date than the other
    // version. Once all workers have switched off of the active version, it
    // will be free for the manager to take and update.
    auto reader = contents_[standby_version_].MaybeGetReader();
    if (reader) {
      return std::move(reader);
    }
    // If the manager is still updating the standby version, return the
    // the active state, which should never be blocked.
    return contents_[!standby_version].GetReader();
  }

  // Returns a Writer which represents exclusive access to the underlying
  // contents. Will block until the caller can get exclusive access. The
  // calling thread must be sure to destroy any previous Writer it got from
  // this object before getting another (preferably sooner rather than later).
  RwLock::Writer GetWriter() {
    auto writer = contents_[!standby_version_].GetWriter();
    standby_version_ = !standby_version_;
    return std::move(writer);
  }

  // Returns a Writer which represents exclusive access to the underlying
  // contents. If cannot gain exclusive access will return an empty Writer
  // instead of waiting for access. The calling thread must be sure to destroy
  // any previous Writer it got from this object before getting another
  // (preferably sooner rather than later).
  RwLock::Writer MaybeGetWriter() {
    auto writer = contents_[!standby_version_].MaybeGetWriter();
    if (writer) {
      standby_version_ = !standby_version_;
      return std::move(writer);
    }
    // If the active version is still being read from we can't get exclusive
    // access to it to write to.
    return Writer();
  }

public:
private:
  std::array<RwLock<T>, 2> contents_;
  // Version that the manager most recently checked out to update.
  std::atomic_bool standby_version_{0};
};
