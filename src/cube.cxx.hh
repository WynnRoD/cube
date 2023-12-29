#ifndef _CUBE_CXX_INCLUDED
#define _CUBE_CXX_INCLUDED

#include <iomanip>
#include <iostream>

#if defined(__clang__)
  #define __CUBE_CXX_FN_ATTR __attribute__((__always_inline__, __nodebug__))
#elif defined(__GNUC__)
  #define __CUBE_CXX_FN_ATTR __attribute__((__always_inline__, __artificial__))
#else
  #define __CUBE_CXX_FN_ATTR
#endif

namespace cube {

constexpr auto n_diagonals       = 4;
constexpr auto n_squares_per_row = 5;
constexpr auto n_rows_or_cols    = 10;
constexpr auto n_dark_squares    = 50;
constexpr auto n_total_squares   = 64;

/** \brief Diagonal shift delta */
enum diags {
  diags_min       = n_squares_per_row,
  diags_min_right = diags_min,
  diags_max_right,
  diags_max
};

#if 0
/** \brief Interval [_Min, _Max) */
template <unsigned _Val, unsigned _Max, unsigned _Min>
constexpr auto is_interval = _Val >= _Min && _Max > _Val;
#endif

/** \brief Bound interval within [7, 5) */
template <unsigned _Val>
constexpr auto is_adjacent_diagonal = _Val >= diags_min && diags_max > _Val;

namespace detail {
class basic_bits_wrapper;

template <int _Min, bool _Expand = is_adjacent_diagonal<_Min>>
struct basic_board_layout;
} // namespace detail

enum square { invalid = n_total_squares - 1 };

using bitboard        = detail::basic_bits_wrapper;
using board5ds_layout = detail::basic_board_layout<diags_min_right>;
using board6ds_layout = detail::basic_board_layout<diags_max_right>;

namespace mem {

inline void *__CUBE_CXX_FN_ATTR initialise(void *_Dst, int _Val,
                                           unsigned long _Count) noexcept {
  constexpr auto SliceSz = sizeof(unsigned);

  if ((_Count % SliceSz) == 0)
    for (auto _ptr = static_cast<unsigned *>(_Dst); _Count > 0;
         _Count    = _Count - SliceSz)
      *_ptr++ = _Val;

  return _Dst;
}

} // namespace mem

namespace detail {

class basic_bits_wrapper {
  unsigned long long bits_;

public:
  constexpr basic_bits_wrapper() noexcept;
  constexpr basic_bits_wrapper(unsigned long long) noexcept;
  explicit operator unsigned long long() const noexcept;

  int index() const noexcept;
#if _WRAP_MSB_COUNT
  int lsb() const noexcept;
  int msb() const noexcept;
  int count() const noexcept;
#endif

  bool in(const basic_bits_wrapper&) const noexcept;
  bool has(const basic_bits_wrapper&) const noexcept;
  basic_bits_wrapper after(int) const noexcept;
  void append(const basic_bits_wrapper&) noexcept;

  const basic_bits_wrapper& begin() const noexcept;
  const basic_bits_wrapper& end() const noexcept;

  const basic_bits_wrapper& operator++() noexcept;
  basic_bits_wrapper operator*() const noexcept;

  bool operator==(const basic_bits_wrapper&) const noexcept;
  bool operator!=(const basic_bits_wrapper&) const noexcept;
};

constexpr __CUBE_CXX_FN_ATTR basic_bits_wrapper::basic_bits_wrapper() noexcept
    : bits_(0) {}

constexpr __CUBE_CXX_FN_ATTR
basic_bits_wrapper::basic_bits_wrapper(unsigned long long _Bits) noexcept
    : bits_(_Bits) {}

inline __CUBE_CXX_FN_ATTR
    basic_bits_wrapper::operator unsigned long long() const noexcept {
  return bits_;
}

inline int __CUBE_CXX_FN_ATTR basic_bits_wrapper::index() const noexcept {
#if defined(__GNUC__) || defined(__clang__)
  return (int)__builtin_ctzll(bits_);
#elif defined(_MSC_VER)
#endif
}

#if _WRAP_MSB_COUNT

inline int __CUBE_CXX_FN_ATTR basic_bits_wrapper::lsb() const noexcept {
  return (int)index();
}

inline int __CUBE_CXX_FN_ATTR basic_bits_wrapper::msb() const noexcept {
  #if defined(__GNUC__) || defined(__clang__)
  return 63 - __builtin_clzll(bits_);
  #elif defined(_MSC_VER)
  #endif
}

inline int __CUBE_CXX_FN_ATTR basic_bits_wrapper::count() const noexcept {
  #if defined(__GNUC__) || defined(__clang__)
  return __builtin_popcountll(bits_);
  #elif defined(_MSC_VER)
  #endif
}

#endif

inline bool __CUBE_CXX_FN_ATTR
basic_bits_wrapper::in(const basic_bits_wrapper& _Other) const noexcept {
  return (bits_ & _Other.bits_) /* != 0 */;
}

inline bool __CUBE_CXX_FN_ATTR
basic_bits_wrapper::has(const basic_bits_wrapper& _Other) const noexcept {
  return _Other.bits_ && ((bits_ & _Other.bits_) == _Other.bits_);
}

inline basic_bits_wrapper __CUBE_CXX_FN_ATTR
basic_bits_wrapper::after(int _Ds) const noexcept {
  return basic_bits_wrapper{(_Ds > 0 ? (bits_ << _Ds) : (bits_ >> -_Ds))};
}

inline void __CUBE_CXX_FN_ATTR
basic_bits_wrapper::append(const basic_bits_wrapper& _Other) noexcept {
  bits_ |= _Other.bits_;
}

inline const basic_bits_wrapper& __CUBE_CXX_FN_ATTR
basic_bits_wrapper::begin() const noexcept {
  return *this;
}

inline const basic_bits_wrapper& __CUBE_CXX_FN_ATTR
basic_bits_wrapper::end() const noexcept {
  static basic_bits_wrapper none = 0;
  return none;
}

inline const basic_bits_wrapper& __CUBE_CXX_FN_ATTR
basic_bits_wrapper::operator++() noexcept {
  return bits_ &= (bits_ - 1), *this;
}

inline basic_bits_wrapper __CUBE_CXX_FN_ATTR
basic_bits_wrapper::operator*() const noexcept {
  return basic_bits_wrapper{bits_ & -bits_};
}

inline bool __CUBE_CXX_FN_ATTR basic_bits_wrapper::operator==(
    const basic_bits_wrapper& _Other) const noexcept {
  return bits_ == _Other.bits_;
}

inline bool __CUBE_CXX_FN_ATTR basic_bits_wrapper::operator!=(
    const basic_bits_wrapper& _Other) const noexcept {
  return !(*this == _Other);
}

inline std::ostream& __CUBE_CXX_FN_ATTR
operator<<(std::ostream& _Os, basic_bits_wrapper _Val) noexcept {
  auto flags = _Os.flags();
#if ZERO_HEX_NON_EXPAND
  if (_Val == 0)
    _Os << std::setfill(' ') << std::setw(18) << "0";
  else
#endif
    _Os << "0x" << std::setfill('0') << std::setw(16) << std::hex
        << std::uppercase << static_cast<unsigned long long>(_Val);

  _Os.flags(flags);

  return _Os;
}

} // namespace detail

static constexpr unsigned long long rank(int _MinDiag, int _Id) noexcept {
  if (_Id-- <= 0)
    return 0x1full; // base_val$
  return rank(_MinDiag, _Id) << ((_Id % 2) + _MinDiag);
}

namespace detail {

template <int _Min, int _Num> struct basic_rank_maker {
  using prev_t = basic_rank_maker<_Min, _Num - 1>;
  enum : int { next = _Min + (_Num % 2) };
  enum : unsigned long long { value = prev_t::value << prev_t::next };
};

template <int _Min> struct basic_rank_maker<_Min, 0> {
  enum : int { next = _Min };
  enum : unsigned long long { value = 0x1F };
};

template <int _Min> struct basic_board_layout<_Min, true> {
  template <int _Num> using rank = basic_rank_maker<_Min, _Num>;
  /** \brief Field masks */
#if 0
  enum struct field: unsigned long long {
#else
  enum : unsigned long long{
#endif
      black = rank<0>::value | rank<1>::value | rank<2>::value | rank<3>::value,
      white = rank<9>::value | rank<8>::value | rank<7>::value | rank<6>::value,
      middle = rank<4>::value | rank<5>::value,
      /** \brief Base squares */
      dark = black | middle | white};
};

extern template struct basic_board_layout<diags_min_right, true>;
extern template struct basic_board_layout<diags_max_right, true>;

} // namespace detail
} // namespace cube

#endif