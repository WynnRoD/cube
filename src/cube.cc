#include "cube.cxx.hh"

namespace cube {
namespace detail {

template struct basic_board_layout<diags_min_right, true>;
template struct basic_board_layout<diags_max_right, true>;

} // namespace detail
} // namespace cube
