#ifndef _UNUSED_
#define _UNUSED_

#if _UNUSED_PIMPL

  #include <memory>

// stream.h

class StreamBase {
  struct impl;
  std::unique_ptr<impl> pImpl;

public:
  ~StreamBase() noexcept;
  StreamBase() noexcept;

  void add(int) noexcept;
  int fetch() const noexcept;

  StreamBase(StreamBase&&) noexcept;
  StreamBase& operator=(StreamBase&&) noexcept;

  StreamBase(const StreamBase&)            = delete;
  StreamBase& operator=(const StreamBase&) = delete;
};

// stream.cc

struct StreamBase::impl {
  int value_;
};

StreamBase::~StreamBase() noexcept = default;
StreamBase::StreamBase() noexcept : pImpl(std::make_unique<impl>(0)) {}
StreamBase::StreamBase(StreamBase&& other) noexcept {
  *this = std::move(other);
}

StreamBase& StreamBase::operator=(StreamBase&& other) noexcept {
  if (this != &other) {
    pImpl->value_       = other.pImpl->value_;
    other.pImpl->value_ = 0;
  }
  return *this;
}

void StreamBase::add(int num) noexcept { pImpl->value_ += num; }
int StreamBase::fetch() const noexcept { return pImpl->value_; }

#endif

#if _UNUSED_INTRIN
static int bitcount(unsigned long long _X) {
  auto n = 0;
  while (_X)
    ++n, _X &= (_X - 1);
  return n;
}

static int lsb(unsigned long long _X) {
  auto i = 0;
  while ((_X & 1ULL) == 0)
    _X >>= 1, ++i;
  return i;
}

static int msb(unsigned long long _X) {
  auto i = 63;
  while (i > 0 && (_X & (1ULL << i)) == 0)
    --i;
  return i;
}
#endif

#if _UNUSED_VARG

  #include <cstdarg>
  #include <cstdio>

int write(char const *const format, ...) noexcept {
  va_list args;
  va_start(args, format);
  auto written = vfprintf(stdout, format, args);
  va_end(args);
  return written;
}

static unsigned long long masked(int min, int id) {
  static const unsigned long long base = 0x1f;
  #if OPTION_ALL
  if (id == 0)
    return base;
  const int prev_id = id - 1;
  return masked(min, prev_id) << min + (prev_id % 2);
  #else
  if (id == 0)
    return base;
  return masked(min, --id) << min + (id % 2);
  #endif
}

#endif

#endif