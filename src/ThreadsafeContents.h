#include <shared_mutex>
#include <mutex>
#include <utility>

template <typename MutexT, typename ContentsT>
class WriteAccessor {
  // This class represents exclusive access to the contents inside.
  // The contents_ ust be protected by the mutex associated with lock_.
public:
  WriteAccessor() {}
  WriteAccessor(const WriteAccessor&) = delete;
  WriteAccessor(ContentsT* t, std::unique_lock<MutexT>&& lock) :
    contents_(t),
    lock_(std::move(lock)) {}
  WriteAccessor(WriteAccessor&& other) :
    contents_(std::exchange(other.contents_, nullptr)),
    lock_(std::move(other.lock_)) {}
  WriteAccessor& operator=(const WriteAccessor&) = delete;
  WriteAccessor& operator=(WriteAccessor&& other) {
    contents_ = std::exchange(other.contents_, nullptr);
    lock_ = std::move(other.lock_);
    return *this;
  }

  ContentsT* operator->() { return contents_; }
  const ContentsT* operator->() const { return contents_; }

  ContentsT& operator*() { return *contents_; }
  const ContentsT& operator*() const { return *contents_; }

  bool IsEmpty() const { return contents_ == nullptr; }
  operator bool() const { contents_ != nullptr; }
private:
  ContentsT* contents_ = nullptr;
  std::unique_lock<MutexT>&& lock_;
};


template <typename T>
class ReadAccessor {
  // This class represents shared access that is read only to the contents.
  // The contents_ ust be protected by the mutex associated with lock_.
public:
  ReadAccessor() {}
  ReadAccessor(const ReadAccessor&) = delete;
  ReadAccessor(T* t, std::shared_lock<std::shared_mutex>&& lock) :
    contents_(t),
    lock_(std::move(lock)) {}
  ReadAccessor(ReadAccessor&& other) :
    contents_(std::exchange(other.contents_, nullptr)),
    lock_(std::move(other.lock_)) {}
  ReadAccessor& operator=(const ReadAccessor&) = delete;
  ReadAccessor& operator=(ReadAccessor&& other) {
    contents_ = std::exchange(other.contents_, nullptr);
    lock_ = std::move(other.lock_);
    return *this;
  }

  const T* operator->() const { return contents_; }

  const T& operator*() const { return *contents_; }

  bool IsEmpty() const { return contents_ == nullptr; }
  operator bool() const { contents_ != nullptr; }
private:
  T* contents_ = nullptr;
  std::shared_lock<std::shared_mutex>&& lock_;
};

// TODO: Add PROTECTED_BY(mtx_) annotations.

template <typename T>
class RwLock {
  // Class which allows shared access across threads for reading, and
  // exclusive access for writing. Possible states:
  // a) 1 Writer and 0 Readers
  // b) 0 Writers and N Readers
  // Works by returning Writer and Reader classes that carry the relevant locks
  // for contents_. When the returned accessor objects are destroyed they
  // give up ownership of the contents_ automatically so the caller doesn't
  // need to worry about handing the mutex.
public:
  // Prevents creation of Writers and Readers and is prevented from being
  // created by existence of a Writer or a Reader.
  using Writer = WriteAccessor<std::shared_mutex, T>;
  // Prevents creation of a Writer and is prevented from existing by a Writer's
  // existence.
  using Reader = ReadAccessor<T>;

  RwLock(const T& t) : contents_(t) {}
  RwLock(T&& t) : contents_(std::move(t)) {}
  template <typename... Args>
  RwLock(Args&&... args) :
    contents_(std::forward<Args>(args)...) {}
  // No copying or moving.
  RwLock(const RwLock&) = delete;
  RwLock(RwLock&&) = delete;
  RwLock& operator=(const RwLock&) = delete;
  RwLock& operator=(RwLock&&) = delete;

  Reader GetReader() {
    // Returns a reader that gives read access to the thread. Blocks until
    // there are no Writers.
    std::shared_lock<std::shared_mutex> lock(mtx_);
    return Reader(&contents_, std::move(lock));
  }

  Reader MaybeGetReader() {
    // Returns a reader that gives read access to the thread. If there is a
    // Writer, returns an empty Reader.
    std::shared_lock<std::shared_mutex> lock(mtx_, std::try_to_lock);
    if (lock.owns_lock()) {
      return Reader(&contents_, std::move(lock));
    }
    return Reader();
  }

  Writer GetWriter() {
    // Returns a Writer giving exclusive ownership of contents_ to the caller.
    // Blocks until ownership can be attained.
    std::unique_lock<std::shared_mutex> lock(mtx_);
    return Writer(&contents_, std::move(lock));
  }

  Writer MaybeGetWriter() {
    // Returns a Writer giving exclusive ownership of contents_ to the caller.
    // If cannot gain exclusive access returns an empty Writer.
    std::unique_lock<std::shared_mutex> lock(mtx_, std::try_to_lock);
    if (lock.owns_lock()) {
      return Writer(&contents_, std::move(lock));
    }
    return Writer();
  }

private:
  T contents_;
  std::shared_mutex mtx_;
};
