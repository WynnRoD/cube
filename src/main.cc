#include "cube.cxx.hh"

#include <fstream>
#include <string>

namespace cube {

using diagonals = int const (&)[n_diagonals];

void append_ranks(int, std::ostream&);
void append_numbers(bitboard, std::ostream&);
void append_nexts(bitboard, std::ostream&, diagonals);
void append_attacks(bitboard, std::ostream&, diagonals);
void append_betweens(bitboard, std::ostream&, diagonals);

} // namespace cube

int main(int argc, char const *const argv[]) {
  static_assert(cube::board6ds_layout::rank<9>::value ==
                    cube::rank(cube::diags_max_right, 9),
                "algorithms mismatch");

  int right, left;
  int min_delta = -1;

  if (argc > 1)
    min_delta = (int)strtol(argv[1], nullptr, 10);

  switch (min_delta) {
  case cube::diags_min:
    right = min_delta, left = min_delta + 1;
    break;

  default:
    right = cube::diags_max_right, left = cube::diags_max;
    break;
  }

  int const dirs[] = {right, left, -right, -left};

  std::string pre_name  = "diags_";
  std::string post_name = "m.txt";

  std::string filename = pre_name + std::to_string(right) + post_name;

  std::ofstream file{filename, std::ios::out | std::ios::trunc};

  if (!file.is_open()) {
    int dummy;
    std::cout << "An error occured while openeing file.\nPress any key to "
                 "exit.\n\n";
    std::cin >> dummy;
  }

  const cube::bitboard base = min_delta == cube::diags_min
                                  ? cube::board5ds_layout::dark
                                  : cube::board6ds_layout::dark;

  file << "/* valid / dark squares */\n" << base << "\n\n";

  cube::append_ranks(right, file);
  cube::append_numbers(base, file);
  cube::append_nexts(base, file, dirs);
  cube::append_attacks(base, file, dirs);
  cube::append_betweens(base, file, dirs);

  std::cout << "Contents written to \'" << filename << "\'.\n\n";

  return 0;
}

namespace cube {

void append_ranks(int _MinDiag, std::ostream& _Os) {
  _Os << "/* ranks */\n";
  for (auto __idx = 0; __idx < n_rows_or_cols; ++__idx)
    _Os << bitboard{rank(_MinDiag, __idx)}
        << ((__idx < n_rows_or_cols - 1) ? ",\n" : "\n");
  _Os << "\n\n";
}

void append_numbers(bitboard _Dark, std::ostream& _Os) {
  _Os << "/* standard notation */\n";
  for (auto __x = 0, num = 0; __x < n_total_squares; ++__x) {
    if (__x && __x < n_total_squares)
      _Os << ", ";
    if (__x && (__x % 8) == 0)
      _Os << "\n";

    _Os << std::setw(2) << std::setfill(' ')
        << (_Dark.has(1ull << __x) ? ++num : invalid);
  }
  _Os << "\n\n";
}

void append_nexts(bitboard _Dark, std::ostream& _Os, diagonals _Distances) {

  _Os << "/* next square */\n";
  for (auto __x = 0; __x < n_total_squares; ++__x) {
    bitboard __s = 1ull << __x;

    if (!_Dark.has(__s))
      _Os << invalid << ", " << invalid << ", " << invalid << ", " << invalid;
    else
      for (auto const& __dx : _Distances)
        _Os << std::setw(2) << std::setfill(' ')
            << (_Dark.has(__s.after(__dx)) ? __s.index() + __dx : invalid)
            << (((&__dx - _Distances) < n_diagonals - 1) ? ", " : "");

    _Os << ((__x < n_total_squares - 1) ? ",\n" : "\n");
  }
  _Os << "\n\n";
}

void append_attacks(bitboard _Dark, std::ostream& _Os, diagonals _Distances) {
  bitboard att[n_total_squares][n_diagonals];
  mem::initialise(att, 0, sizeof(att));

  for (const auto& __s : _Dark) {
    for (const auto& __dx : _Distances) {
      bitboard __basket{};

      for (auto __dst = __s.after(__dx); __dst.after(__dx).in(_Dark);
           __dst      = __dst.after(__dx))
        __basket.append(__dst);

      att[__s.index()][(int)(&__dx - _Distances)] = __basket;
    }
  }

  constexpr auto last = n_total_squares;
  constexpr auto n    = n_diagonals / 2;

  _Os << "/* attack squares */\n";

  for (auto __x = 0; __x < last; ++__x)
    _Os << att[__x][0] << ", " << att[__x][1] << ",\n"
        << att[__x][2] << ", " << att[__x][3]
        << ((__x < last - 1) ? ",\n" : "\n");

  _Os << "\n\n";
}

static auto ray(int _At, int _To, int _Dir) {
  auto __m = 0ull;
  while ((_At += _Dir) != _To)
    __m |= (1ull << _At);
  return __m;
}

void append_betweens(bitboard _Dark, std::ostream& _Os, diagonals _Distances) {
  bitboard btw[n_total_squares][n_total_squares];
  mem::initialise(btw, 0, sizeof(btw));

  for (const auto& __m : _Dark)
    for (auto __dx : _Distances)
      for (auto __dst = __m.after(__dx); __dst.in(_Dark);
           __dst      = __dst.after(__dx))
        btw[__m.index()][__dst.index()] = ray(__m.index(), __dst.index(), __dx);

  constexpr auto last = n_total_squares;
  constexpr auto n    = 2;

  _Os << "/* squares between */\n";

  for (auto __x = 0; __x < last; ++__x)
    for (auto __y = 0; __y < last; __y += n)
      _Os << btw[__x][__y] << ", " << btw[__x][__y + 1]
          << ((__x < last - 1 || last - n > __y) ? ",\n" : "\n");

  _Os << "\n\n";
}

} // namespace cube