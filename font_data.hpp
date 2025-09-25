#ifndef SAND_HEADER_FONT
#	define SAND_HEADER_FONT
#
#	include <array>
#	include <climits>
#	include <cstddef>
#	include <cstdint>
#	include <string_view>

namespace sand {
	inline constexpr std::uint64_t font_width = 4;
	inline constexpr std::uint64_t font_height = 5;
	inline constexpr std::array<std::string_view, (1 << CHAR_BIT)> font_data = ([] {
		std::array<std::string_view, (1 << CHAR_BIT)> font_data;
		for (std::size_t i = 0; i < font_data.size(); ++i) {
			font_data[i] = "####"
			               "####"
			               "####"
			               "####"
			               "####";
		}
		font_data[' ']  = "    "
		                  "    "
		                  "    "
		                  "    "
		                  "    ";
		font_data['!']  = " #  "
		                  " #  "
		                  "    "
		                  " #  "
		                  "    ";
		font_data['"']  = "# # "
		                  "# # "
		                  "    "
		                  "    "
		                  "    ";
		font_data['#']  = "# # "
		                  "### "
		                  "# # "
		                  "### "
		                  "# # ";
		font_data['$']  = " ## "
		                  "##  "
		                  " ## "
		                  "##  "
		                  "    ";
		font_data['%']  = "# # "
		                  "  # "
		                  " #  "
		                  "# # "
		                  "    ";
		font_data['&']  = " #  "
		                  "# # "
		                  "##  "
		                  "### "
		                  "    ";
		font_data['\''] = " #  "
		                  " #  "
		                  "    "
		                  "    "
		                  "    ";
		font_data['(']  = "  # "
		                  " #  "
		                  " #  "
		                  "  # "
		                  "    ";
		font_data[')']  = "#   "
		                  " #  "
		                  " #  "
		                  "#   "
		                  "    ";
		font_data['*']  = "# # "
		                  " #  "
		                  "# # "
		                  "    "
		                  "    ";
		font_data['+']  = "    "
		                  " #  "
		                  "### "
		                  " #  "
		                  "    ";
		font_data[',']  = "    "
		                  "    "
		                  "    "
		                  " #  "
		                  "#   ";
		font_data['-']  = "    "
		                  "    "
		                  "### "
		                  "    "
		                  "    ";
		font_data['.']  = "    "
		                  "    "
		                  "    "
		                  " #  "
		                  "    ";
		font_data['/']  = "  # "
		                  "  # "
		                  " #  "
		                  "#   "
		                  "    ";
		font_data['0']  = " #  "
		                  "# # "
		                  "# # "
		                  " #  "
		                  "    ";
		font_data['1']  = " #  "
		                  "##  "
		                  " #  "
		                  "### "
		                  "    ";
		font_data['2']  = "##  "
		                  "  # "
		                  " #  "
		                  "### "
		                  "    ";
		font_data['3']  = "##  "
		                  "  # "
		                  " ## "
		                  "##  "
		                  "    ";
		font_data['4']  = "# # "
		                  "# # "
		                  "### "
		                  "  # "
		                  "    ";
		font_data['5']  = "### "
		                  "##  "
		                  "  # "
		                  "##  "
		                  "    ";
		font_data['6']  = " ## "
		                  "##  "
		                  "# # "
		                  " #  "
		                  "    ";
		font_data['7']  = "### "
		                  "  # "
		                  " #  "
		                  " #  "
		                  "    ";
		font_data['8']  = "### "
		                  "### "
		                  "# # "
		                  "### "
		                  "    ";
		font_data['9']  = "### "
		                  "# # "
		                  "### "
		                  "  # "
		                  "    ";
		font_data[':']  = "    "
		                  " #  "
		                  "    "
		                  " #  "
		                  "    ";
		font_data[';']  = "    "
		                  " #  "
		                  "    "
		                  " #  "
		                  "#   ";
		font_data['<']  = "    "
		                  "  # "
		                  " #  "
		                  "  # "
		                  "    ";
		font_data['=']  = "    "
		                  "### "
		                  "    "
		                  "### "
		                  "    ";
		font_data['>']  = "    "
		                  "#   "
		                  " #  "
		                  "#   "
		                  "    ";
		font_data['?']  = "##  "
		                  "  # "
		                  "    "
		                  " #  "
		                  "    ";
		font_data['@']  = "### "
		                  "# # "
		                  "#   "
		                  "### "
		                  "    ";
		font_data['A']  = " #  "
		                  "# # "
		                  "### "
		                  "# # "
		                  "    ";
		font_data['B']  = "##  "
		                  "### "
		                  "# # "
		                  "### "
		                  "    ";
		font_data['C']  = " ## "
		                  "#   "
		                  "#   "
		                  " ## "
		                  "    ";
		font_data['D']  = "##  "
		                  "# # "
		                  "# # "
		                  "##  "
		                  "    ";
		font_data['E']  = "### "
		                  "##  "
		                  "#   "
		                  "### "
		                  "    ";
		font_data['F']  = "### "
		                  "#   "
		                  "##  "
		                  "#   "
		                  "    ";
		font_data['G']  = " ## "
		                  "#   "
		                  "# # "
		                  " ## "
		                  "    ";
		font_data['H']  = "# # "
		                  "### "
		                  "# # "
		                  "# # "
		                  "    ";
		font_data['I']  = "### "
		                  " #  "
		                  " #  "
		                  "### "
		                  "    ";
		font_data['J']  = " ## "
		                  "  # "
		                  "# # "
		                  "### "
		                  "    ";
		font_data['K']  = "# # "
		                  "##  "
		                  "# # "
		                  "# # "
		                  "    ";
		font_data['L']  = "#   "
		                  "#   "
		                  "#   "
		                  "### "
		                  "    ";
		font_data['M']  = "### "
		                  "### "
		                  "# # "
		                  "# # "
		                  "    ";
		font_data['N']  = "# # "
		                  "### "
		                  "### "
		                  "# # "
		                  "    ";
		font_data['O']  = "### "
		                  "# # "
		                  "# # "
		                  "### "
		                  "    ";
		font_data['P']  = "### "
		                  "# # "
		                  "### "
		                  "#   "
		                  "    ";
		font_data['Q']  = "### "
		                  "# # "
		                  "# # "
		                  "##  "
		                  "  # ";
		font_data['R']  = "### "
		                  "# # "
		                  "##  "
		                  "# # "
		                  "    ";
		font_data['S']  = " ## "
		                  "#   "
		                  " ## "
		                  "##  "
		                  "    ";
		font_data['T']  = "### "
		                  " #  "
		                  " #  "
		                  " #  "
		                  "    ";
		font_data['U']  = "# # "
		                  "# # "
		                  "# # "
		                  "### "
		                  "    ";
		font_data['V']  = "# # "
		                  "# # "
		                  "# # "
		                  " #  "
		                  "    ";
		font_data['W']  = "# # "
		                  "# # "
		                  "### "
		                  "### "
		                  "    ";
		font_data['X']  = "# # "
		                  " #  "
		                  "# # "
		                  "# # "
		                  "    ";
		font_data['Y']  = "# # "
		                  "# # "
		                  " #  "
		                  " #  "
		                  "    ";
		font_data['Z']  = "### "
		                  "  # "
		                  " #  "
		                  "### "
		                  "    ";
		font_data['[']  = " ## "
		                  " #  "
		                  " #  "
		                  " ## "
		                  "    ";
		font_data['\\'] = "#   "
		                  "#   "
		                  " #  "
		                  "  # "
		                  "    ";
		font_data[']']  = "##  "
		                  " #  "
		                  " #  "
		                  "##  "
		                  "    ";
		font_data['^']  = " #  "
		                  "# # "
		                  "    "
		                  "    "
		                  "    ";
		font_data['_']  = "    "
		                  "    "
		                  "    "
		                  "### "
		                  "    ";
		font_data['`']  = " #  "
		                  "  # "
		                  "    "
		                  "    "
		                  "    ";
		font_data['a']  = "    "
		                  " ## "
		                  "# # "
		                  " ## "
		                  "    ";
		font_data['b']  = "#   "
		                  "##  "
		                  "# # "
		                  "##  "
		                  "    ";
		font_data['c']  = "    "
		                  "### "
		                  "#   "
		                  "### "
		                  "    ";
		font_data['d']  = "  # "
		                  " ## "
		                  "# # "
		                  " ## "
		                  "    ";
		font_data['e']  = "    "
		                  " ## "
		                  "##  "
		                  " ## "
		                  "    ";
		font_data['f']  = " ## "
		                  "#   "
		                  "##  "
		                  "#   "
		                  "    ";
		font_data['g']  = "    "
		                  "##  "
		                  "### "
		                  "  # "
		                  "##  ";
		font_data['h']  = "#   "
		                  "##  "
		                  "# # "
		                  "# # "
		                  "    ";
		font_data['i']  = " #  "
		                  "    "
		                  " #  "
		                  " #  "
		                  "    ";
		font_data['j']  = "  # "
		                  "    "
		                  "# # "
		                  " ## "
		                  "    ";
		font_data['k']  = "#   "
		                  "# # "
		                  "##  "
		                  "# # "
		                  "    ";
		font_data['l']  = " #  "
		                  " #  "
		                  " #  "
		                  "  # "
		                  "    ";
		font_data['m']  = "    "
		                  "### "
		                  "### "
		                  "# # "
		                  "    ";
		font_data['n']  = "    "
		                  "##  "
		                  "# # "
		                  "# # "
		                  "    ";
		font_data['o']  = "    "
		                  "### "
		                  "# # "
		                  "### "
		                  "    ";
		font_data['p']  = "    "
		                  "##  "
		                  "# # "
		                  "##  "
		                  "#   ";
		font_data['q']  = "    "
		                  " ## "
		                  "# # "
		                  " ## "
		                  "  # ";
		font_data['r']  = "    "
		                  " ## "
		                  "#   "
		                  "#   "
		                  "    ";
		font_data['s']  = "    "
		                  " ## "
		                  " #  "
		                  "##  "
		                  "    ";
		font_data['t']  = " #  "
		                  "### "
		                  " #  "
		                  " #  "
		                  "    ";
		font_data['u']  = "    "
		                  "# # "
		                  "# # "
		                  "### "
		                  "    ";
		font_data['v']  = "    "
		                  "# # "
		                  "# # "
		                  " #  "
		                  "    ";
		font_data['w']  = "    "
		                  "# # "
		                  "### "
		                  "### "
		                  "    ";
		font_data['x']  = "    "
		                  "# # "
		                  " #  "
		                  "# # "
		                  "    ";
		font_data['y']  = "    "
		                  "# # "
		                  " ## "
		                  "  # "
		                  "##  ";
		font_data['z']  = "    "
		                  "##  "
		                  " #  "
		                  " ## "
		                  "    ";
		font_data['{']  = "  # "
		                  "##  "
		                  " #  "
		                  "  # "
		                  "    ";
		font_data['|']  = " #  "
		                  " #  "
		                  " #  "
		                  " #  "
		                  "    ";
		font_data['}']  = "#   "
		                  " #  "
		                  " ## "
		                  "#   "
		                  "    ";
		font_data['~']  = "    "
		                  " ## "
		                  "##  "
		                  "    "
		                  "    ";
		return font_data;
	})();
}

#endif
