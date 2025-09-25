#include <array>
#include <chrono> // IWYU pragma: keep
#include <climits>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <print>
#include <stdio.h>
#include <string>
#include <string_view>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <xieite/io/file.hpp>
#include <xieite/io/read.hpp>
#include <xieite/math/color3.hpp>
#include <xieite/math/color4.hpp>

using namespace std::literals;

namespace sand {
	constexpr std::uint64_t texture_width = 8;
	constexpr std::uint64_t texture_height = 8;
	constexpr std::array<std::string_view, 0x21> texture_frames = { {
		"        " // 00 void
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"22222222" // 01 stone
		"25554432"
		"24545441"
		"24444331"
		"23443431"
		"14444331"
		"13433331"
		"11111111"
		,
		"14325413" // 02 cobble
		"32142521"
		"15433254"
		"52321143"
		"43154323"
		"32543251"
		"14112542"
		"25431424"
		,
		"        " // 03 cursor (top left corner)
		"        "
		"        "
		"        "
		"        "
		"        "
		"      99"
		"      ^$"
		,
		"        " // 04 cursor (top left horizontal)
		"        "
		"        "
		"        "
		"        "
		"        "
		"9^      "
		"@@      "
		,
		"llll...." // 05 blue (dark)
		"llll...."
		"llll...."
		"llll...."
		"....llll"
		"....llll"
		"....llll"
		"....llll"
		,
		"llll<<<<" // 06 blue (light)
		"llll<<<<"
		"llll<<<<"
		"llll<<<<"
		"<<<<llll"
		"<<<<llll"
		"<<<<llll"
		"<<<<llll"
		,
		"asdfbk<;" // 07 rainbow (frame 0)
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		,
		"QWETEREW" // 08 dirt
		"WERQWETE"
		"ETWERTET"
		"WERTEWQW"
		"REWETETE"
		"EWQREQWR"
		"WQRWEWET"
		"ETERTERT"
		,
		"35243214" // 09 rock
		"23435345"
		"35543532"
		"43355442"
		"32453534"
		"24534321"
		"43355434"
		"12424523"
		,
		"BNHBNbGH" // 0A leaves
		"NgbNHGBN"
		"BbGBNNGH"
		"HGBHBNHB"
		"BNNHgbBN"
		"gHBbGbGN"
		"bGHGBGBH"
		"GBNNGBHB"
		,
		"QEETWRQR" // 0B wood
		"QRWEWTWE"
		"WRQRETER"
		"ETWEERWE"
		"RTETWEQR"
		"EETRERQT"
		"WREQETET"
		"ETWQTEWE"
		,
		"GNGbHgHB" // 0C grass
		"gHBbGNBG"
		"NBGGBNGg"
		"NBgHBGBb"
		"BHbBNGBG"
		"bHGHbGNH"
		"BGBgNBNG"
		"NGNbGNHB"
		,
		"cdxkfknh" // 0D flowers
		"dfs,AaAf"
		"xsxNafzh"
		"zA,kAzAZ"
		"fakfkcdx"
		"axsc,dfs"
		"JdfdNxsc"
		"mcsxk,jf"
		,
		"      ^$" // 0E cursor (top left vertical)
		"      $@"
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"        " // 0F cursor (top right corner)
		"        "
		"        "
		"        "
		"        "
		"        "
		"9^      "
		"^$      "
		,
		"{[[]]{[]" // 10 ice
		"[[]{[]]}"
		"{][]][{}"
		"][{][{{]"
		"}{][}]]{"
		"][]{]]{}"
		"}]{]{}[{"
		"{}{}}{}}"
		,
		"[[[[[[[{" // 11 chiseled ice
		"[{{{{{]}"
		"[{]]]][}"
		"[{]]]][}"
		"[{]]]][}"
		"[{]]]][}"
		"[][[[[[}"
		"{}}}}}}}"
		,
		"sdfbk<;a" // 12 rainbow (frame 1)
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		,
		"dfbk<;as" // 13 rainbow (frame 2)
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		,
		"fbk<;asd" // 14 rainbow (frame 3)
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		,
		"bk<;asdf" // 15 rainbow (frame 4)
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		,
		"k<;asdfb" // 16 rainbow (frame 5)
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		,
		"<;asdfbk" // 17 rainbow (frame 6)
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		,
		";asdfbk<" // 18 rainbow (frame 7)
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		,
		"        " // 19 cursor (top right horizontal)
		"        "
		"        "
		"        "
		"        "
		"        "
		"      99"
		"      ^$"
		,
		"9$      " // 1A cursor (top right vertical)
		"^@      "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"      $^" // 1B cursor (bottom left corner)
		"      @@"
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"9^      " // 1C cursor (bottom left horizontal)
		"$@      "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"        " // 1D cursor (bottom left vertical)
		"        "
		"        "
		"        "
		"        "
		"        "
		"      99"
		"      ^$"
		,
		"^@      " // 1E cursor (bottom right corner)
		"@$      "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"      9^" // 1F cursor (bottom right horizontal)
		"      $$"
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"        " // 20 cursor (bottom right vertical)
		"        "
		"        "
		"        "
		"        "
		"        "
		"9^      "
		"$@      "
	} };

	constexpr std::uint64_t font_width = 4;
	constexpr std::uint64_t font_height = 5;
	constexpr std::array<std::string_view, (1 << CHAR_BIT)> font = ([] {
		std::array<std::string_view, (1 << CHAR_BIT)> font;
		for (std::size_t i = 0; i < font.size(); ++i) {
			font[i] = "####"
			          "####"
			          "####"
			          "####"
			          "####";
		}
		font[' ']  = "    "
		             "    "
		             "    "
		             "    "
		             "    ";
		font['!']  = " #  "
		             " #  "
		             "    "
		             " #  "
		             "    ";
		font['"']  = "# # "
		             "# # "
		             "    "
		             "    "
		             "    ";
		font['#']  = "# # "
		             "### "
		             "# # "
		             "### "
		             "# # ";
		font['$']  = " ## "
		             "##  "
		             " ## "
		             "##  "
		             "    ";
		font['%']  = "# # "
		             "  # "
		             " #  "
		             "# # "
		             "    ";
		font['&']  = " #  "
		             "# # "
		             "##  "
		             "### "
		             "    ";
		font['\''] = " #  "
		             " #  "
		             "    "
		             "    "
		             "    ";
		font['(']  = "  # "
		             " #  "
		             " #  "
		             "  # "
		             "    ";
		font[')']  = "#   "
		             " #  "
		             " #  "
		             "#   "
		             "    ";
		font['*']  = "# # "
		             " #  "
		             "# # "
		             "    "
		             "    ";
		font['+']  = "    "
		             " #  "
		             "### "
		             " #  "
		             "    ";
		font[',']  = "    "
		             "    "
		             "    "
		             " #  "
		             "#   ";
		font['-']  = "    "
		             "    "
		             "### "
		             "    "
		             "    ";
		font['.']  = "    "
		             "    "
		             "    "
		             " #  "
		             "    ";
		font['/']  = "  # "
		             "  # "
		             " #  "
		             "#   "
		             "    ";
		font['0']  = " #  "
		             "# # "
		             "# # "
		             " #  "
		             "    ";
		font['1']  = " #  "
		             "##  "
		             " #  "
		             "### "
		             "    ";
		font['2']  = "##  "
		             "  # "
		             " #  "
		             "### "
		             "    ";
		font['3']  = "##  "
		             "  # "
		             " ## "
		             "##  "
		             "    ";
		font['4']  = "# # "
		             "# # "
		             "### "
		             "  # "
		             "    ";
		font['5']  = "### "
		             "##  "
		             "  # "
		             "##  "
		             "    ";
		font['6']  = " ## "
		             "##  "
		             "# # "
		             " #  "
		             "    ";
		font['7']  = "### "
		             "  # "
		             " #  "
		             " #  "
		             "    ";
		font['8']  = "### "
		             "### "
		             "# # "
		             "### "
		             "    ";
		font['9']  = "### "
		             "# # "
		             "### "
		             "  # "
		             "    ";
		font[':']  = "    "
		             " #  "
		             "    "
		             " #  "
		             "    ";
		font[';']  = "    "
		             " #  "
		             "    "
		             " #  "
		             "#   ";
		font['<']  = "    "
		             "  # "
		             " #  "
		             "  # "
		             "    ";
		font['=']  = "    "
		             "### "
		             "    "
		             "### "
		             "    ";
		font['>']  = "    "
		             "#   "
		             " #  "
		             "#   "
		             "    ";
		font['?']  = "##  "
		             "  # "
		             "    "
		             " #  "
		             "    ";
		font['@']  = "### "
		             "# # "
		             "#   "
		             "### "
		             "    ";
		font['A']  = " #  "
		             "# # "
		             "### "
		             "# # "
		             "    ";
		font['B']  = "##  "
		             "### "
		             "# # "
		             "### "
		             "    ";
		font['C']  = " ## "
		             "#   "
		             "#   "
		             " ## "
		             "    ";
		font['D']  = "##  "
		             "# # "
		             "# # "
		             "##  "
		             "    ";
		font['E']  = "### "
		             "##  "
		             "#   "
		             "### "
		             "    ";
		font['F']  = "### "
		             "#   "
		             "##  "
		             "#   "
		             "    ";
		font['G']  = " ## "
		             "#   "
		             "# # "
		             " ## "
		             "    ";
		font['H']  = "# # "
		             "### "
		             "# # "
		             "# # "
		             "    ";
		font['I']  = "### "
		             " #  "
		             " #  "
		             "### "
		             "    ";
		font['J']  = " ## "
		             "  # "
		             "# # "
		             "### "
		             "    ";
		font['K']  = "# # "
		             "##  "
		             "# # "
		             "# # "
		             "    ";
		font['L']  = "#   "
		             "#   "
		             "#   "
		             "### "
		             "    ";
		font['M']  = "### "
		             "### "
		             "# # "
		             "# # "
		             "    ";
		font['N']  = "# # "
		             "### "
		             "### "
		             "# # "
		             "    ";
		font['O']  = "### "
		             "# # "
		             "# # "
		             "### "
		             "    ";
		font['P']  = "### "
		             "# # "
		             "### "
		             "#   "
		             "    ";
		font['Q']  = "### "
		             "# # "
		             "# # "
		             "##  "
		             "  # ";
		font['R']  = "### "
		             "# # "
		             "##  "
		             "# # "
		             "    ";
		font['S']  = " ## "
		             "#   "
		             " ## "
		             "##  "
		             "    ";
		font['T']  = "### "
		             " #  "
		             " #  "
		             " #  "
		             "    ";
		font['U']  = "# # "
		             "# # "
		             "# # "
		             "### "
		             "    ";
		font['V']  = "# # "
		             "# # "
		             "# # "
		             " #  "
		             "    ";
		font['W']  = "# # "
		             "# # "
		             "### "
		             "### "
		             "    ";
		font['X']  = "# # "
		             " #  "
		             "# # "
		             "# # "
		             "    ";
		font['Y']  = "# # "
		             "# # "
		             " #  "
		             " #  "
		             "    ";
		font['Z']  = "### "
		             "  # "
		             " #  "
		             "### "
		             "    ";
		font['[']  = " ## "
		             " #  "
		             " #  "
		             " ## "
		             "    ";
		font['\\'] = "#   "
		             "#   "
		             " #  "
		             "  # "
		             "    ";
		font[']']  = "##  "
		             " #  "
		             " #  "
		             "##  "
		             "    ";
		font['^']  = " #  "
		             "# # "
		             "    "
		             "    "
		             "    ";
		font['_']  = "    "
		             "    "
		             "    "
		             "### "
		             "    ";
		font['`']  = " #  "
		             "  # "
		             "    "
		             "    "
		             "    ";
		font['a']  = "    "
		             " ## "
		             "# # "
		             " ## "
		             "    ";
		font['b']  = "#   "
		             "##  "
		             "# # "
		             "##  "
		             "    ";
		font['c']  = "    "
		             "### "
		             "#   "
		             "### "
		             "    ";
		font['d']  = "  # "
		             " ## "
		             "# # "
		             " ## "
		             "    ";
		font['e']  = "    "
		             " ## "
		             "##  "
		             " ## "
		             "    ";
		font['f']  = " ## "
		             "#   "
		             "##  "
		             "#   "
		             "    ";
		font['g']  = "    "
		             "##  "
		             "### "
		             "  # "
		             "##  ";
		font['h']  = "#   "
		             "##  "
		             "# # "
		             "# # "
		             "    ";
		font['i']  = " #  "
		             "    "
		             " #  "
		             " #  "
		             "    ";
		font['j']  = "  # "
		             "    "
		             "# # "
		             " ## "
		             "    ";
		font['k']  = "#   "
		             "# # "
		             "##  "
		             "# # "
		             "    ";
		font['l']  = " #  "
		             " #  "
		             " #  "
		             "  # "
		             "    ";
		font['m']  = "    "
		             "### "
		             "### "
		             "# # "
		             "    ";
		font['n']  = "    "
		             "##  "
		             "# # "
		             "# # "
		             "    ";
		font['o']  = "    "
		             "### "
		             "# # "
		             "### "
		             "    ";
		font['p']  = "    "
		             "##  "
		             "# # "
		             "##  "
		             "#   ";
		font['q']  = "    "
		             " ## "
		             "# # "
		             " ## "
		             "  # ";
		font['r']  = "    "
		             " ## "
		             "#   "
		             "#   "
		             "    ";
		font['s']  = "    "
		             " ## "
		             " #  "
		             "##  "
		             "    ";
		font['t']  = " #  "
		             "### "
		             " #  "
		             " #  "
		             "    ";
		font['u']  = "    "
		             "# # "
		             "# # "
		             "### "
		             "    ";
		font['v']  = "    "
		             "# # "
		             "# # "
		             " #  "
		             "    ";
		font['w']  = "    "
		             "# # "
		             "### "
		             "### "
		             "    ";
		font['x']  = "    "
		             "# # "
		             " #  "
		             "# # "
		             "    ";
		font['y']  = "    "
		             "# # "
		             " ## "
		             "  # "
		             "##  ";
		font['z']  = "    "
		             "##  "
		             " #  "
		             " ## "
		             "    ";
		font['{']  = "  # "
		             "##  "
		             " #  "
		             "  # "
		             "    ";
		font['|']  = " #  "
		             " #  "
		             " #  "
		             " #  "
		             "    ";
		font['}']  = "#   "
		             " #  "
		             " ## "
		             "#   "
		             "    ";
		font['~']  = "    "
		             " ## "
		             "##  "
		             "    "
		             "    ";
		return font;
	})();

	struct texture {
		std::uint64_t frames_count = 1;
		std::uint64_t frame_indices[8];
	};
	constexpr std::array<sand::texture, 0x20> textures = { {
		/* 00 void */                             { 1, { 0x00 } },
		/* 01 stone */                            { 1, { 0x01 } },
		/* 02 cobbled stone */                    { 1, { 0x02 } },
		/* 03 dark blue */                        { 1, { 0x05 } },
		/* 04 light blue */                       { 1, { 0x06 } },
		/* 05 rainbow */                          { 8, { 0x07, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 } },
		/* 06 dirt */                             { 1, { 0x08 } },
		/* 07 rock */                             { 1, { 0x09 } },
		/* 08 leaves */                           { 1, { 0x0A } },
		/* 09 wood */                             { 1, { 0x0B } },
		/* 0A grass */                            { 1, { 0x0C } },
		/* 0B flowers */                          { 1, { 0x0D } },
		/* 0C ice */                              { 1, { 0x10 } },
		/* 0D chiseled ice */                     { 1, { 0x11 } },
		/* 0E cursor (top left corner) */         { 1, { 0x03 } },
		/* 0F cursor (top left horizontal) */     { 1, { 0x04 } },
		/* 10 cursor (top left vertical) */       { 1, { 0x0E } },
		/* 11 cursor (top right corner) */        { 1, { 0x0F } },
		/* 12 cursor (top right horizonal) */     { 1, { 0x19 } },
		/* 13 cursor (top right vertical) */      { 1, { 0x1A } },
		/* 14 cursor (bottom left corner) */      { 1, { 0x1B } },
		/* 15 cursor (bottom left horizontal) */  { 1, { 0x1C } },
		/* 16 cursor (bottom left vertical) */    { 1, { 0x1D } },
		/* 17 cursor (bottom right corner) */     { 1, { 0x1E } },
		/* 18 cursor (bottom right horizontal) */ { 1, { 0x1F } },
		/* 19 cursor (bottom right vertical) */   { 1, { 0x20 } }
	} };

	struct tile {
		std::uint64_t texture_index;

		[[nodiscard]] friend bool operator==(const sand::tile&, const sand::tile&) = default;
	};
	constexpr std::array<sand::tile, 0x0E> tiles = { {
		/* 00 void */          { 0x00 },
		/* 01 stone */         { 0x01 },
		/* 02 cobbled stone */ { 0x02 },
		/* 03 dark blue */     { 0x03 },
		/* 04 light blue */    { 0x04 },
		/* 05 rainbow */       { 0x05 },
		/* 06 dirt */          { 0x06 },
		/* 07 rock */          { 0x07 },
		/* 08 leaves */        { 0x08 },
		/* 09 wood */          { 0x09 },
		/* 0A grass */         { 0x0A },
		/* 0B flowers */       { 0x0B },
		/* 0C ice */           { 0x0C },
		/* 0D chiseled ice */  { 0x0D }
	} };

	constexpr std::uint64_t chunk_width = 32;
	constexpr std::uint64_t chunk_height = 32;

	std::uint64_t tick = 0;
	std::uint64_t camera_chunk_x = 0;
	std::uint64_t camera_chunk_y = 0;
	std::uint64_t camera_tile_x = 0;
	std::uint64_t camera_tile_y = 0;
	std::uint64_t selection_index = 0;
	std::uint64_t selection_start_x = 0;
	std::uint64_t selection_start_y = 0;
	std::uint64_t selection_end_x = 0;
	std::uint64_t selection_end_y = 0;

	std::unordered_map<std::uint64_t, std::unordered_map<std::uint64_t, std::array<std::array<sand::tile, sand::chunk_height>, sand::chunk_width>>> world;

	struct display_char {
		xieite::color3 pixels[2];

		[[nodiscard]] friend bool operator==(const sand::display_char&, const sand::display_char&) = default;
	};

	std::uint64_t screen_width = 0;
	std::uint64_t screen_height = 0;
	std::vector<sand::display_char> screen;

	[[nodiscard]] constexpr std::uint64_t parse_u64(std::string_view strv, std::uint64_t& i) noexcept {
		while (" \n"sv.contains(strv[i])) {
			++i;
		}
		const bool neg = strv[i] == '-';
		std::uint64_t result = 0;
		for (i += neg; i < strv.size(); ++i) {
			const std::uint64_t digit = "0123456789ABCDEF"sv.find(strv[i]);
			if (digit == std::string::npos) {
				break;
			}
			result = result * 16 + digit;
		}
		++i;
		return neg ? -result : result;
	}

	[[nodiscard]] constexpr xieite::color4 texture_at(std::uint64_t index, std::uint64_t pixel_x, std::uint64_t pixel_y) noexcept {
		switch (sand::texture_frames[sand::textures[index].frame_indices[sand::tick % sand::textures[index].frames_count]][pixel_y * sand::texture_width + pixel_x]) {
		case ' ':
			return 0x00000000;
		case '0':
			return 0x000000FF;
		case '1':
			return 0x3F3B37FF;
		case '2':
			return 0x5B5753FF;
		case '3':
			return 0x776F67FF;
		case '4':
			return 0xA3978FFF;
		case '5':
			return 0xCFC7BFFF;
		case '6':
			return 0x9F9F9FFF;
		case '7':
			return 0xBFBFBFFF;
		case '8':
			return 0xDFDFDFFF;
		case '9':
			return 0xFFFFFFFF;
		case '!':
			return 0x1F1F1FFF;
		case '@':
			return 0x2F2F2FFF;
		case '#':
			return 0x3F3F3FFF;
		case '$':
			return 0x5F5F5FFF;
		case '%':
			return 0x7F7F7FFF;
		case '^':
			return 0x9F9F9FFF;
		case '&':
			return 0xBFBFBFFF;
		case '*':
			return 0xDFDFDFFF;
		case 'q':
			return 0xCFB777FF;
		case 'w':
			return 0xA78F57FF;
		case 'e':
			return 0x7F6737FF;
		case 'r':
			return 0x574727FF;
		case 'Q':
			return 0x775737FF;
		case 'W':
			return 0x573F2FFF;
		case 'E':
			return 0x37271FFF;
		case 'R':
			return 0x271F17FF;
		case 'T':
			return 0x1B130FFF;
		case 'a':
			return 0xFF0000FF;
		case 'z':
			return 0xBF0000FF;
		case 'A':
			return 0x7F0000FF;
		case 'Z':
			return 0x3F0000FF;
		case 's':
			return 0xFF7F00FF;
		case 'x':
			return 0xCF5F00FF;
		case 'd':
			return 0xFFBF00FF;
		case 'c':
			return 0xCF7F00FF;
		case 'f':
			return 0xFFFF00FF;
		case 'v':
			return 0xCF9F1FFF;
		case 'g':
			return 0x7FFF00FF;
		case 'b':
			return 0x00F700FF;
		case 'G':
			return 0x00BF00FF;
		case 'B':
			return 0x007F00FF;
		case 'h':
			return 0x00BF7FFF;
		case 'n':
			return 0x007F5FFF;
		case 'H':
			return 0x076F35FF;
		case 'N':
			return 0x075727FF;
		case 'j':
			return 0x00FFFFFF;
		case 'm':
			return 0x00DFFFFF;
		case 'J':
			return 0x00BFFFFF;
		case 'M':
			return 0x009FFFFF;
		case 'k':
			return 0x007FFFFF;
		case ',':
			return 0x005FFFFF;
		case 'K':
			return 0x003FFFFF;
		case '<':
			return 0x0000FFFF;
		case 'l':
			return 0x00007FFF;
		case '.':
			return 0x00003FFF;
		case ';':
			return 0x7F00FFFF;
		case ':':
			return 0xFF00FFFF;
		case '?':
			return 0xFF007FFF;
		case '[':
			return 0xBFFFFFFF;
		case ']':
			return 0x9FDFFFFF;
		case '{':
			return 0x7FBFFFFF;
		case '}':
			return 0x6F9FFFFF;
		}
		return 0xFF00FFFF;
	}

	[[nodiscard]] constexpr bool font_at(char index, std::uint64_t pixel_x, std::uint64_t pixel_y) noexcept {
		return (pixel_x < sand::font_width) && (pixel_y < sand::font_height) && (sand::font[static_cast<std::size_t>(index)][pixel_y * sand::font_width + pixel_x] == '#');
	}

	[[nodiscard]] xieite::color3& screen_at(std::uint64_t screen_x, std::uint64_t screen_y) noexcept {
		static xieite::color3 dummy;
		return ((screen_x < screen_width) && (screen_y < (screen_height * 2)))
			? screen[screen_y / 2 * screen_width + screen_x].pixels[!!(screen_y % 2)]
			: dummy;
	}

	constexpr void draw_texture(std::uint64_t texture_index, std::uint64_t chunk_x, std::uint64_t chunk_y, std::uint64_t tile_x, std::uint64_t tile_y) noexcept {
		const std::uint64_t offset_x = sand::screen_width / 2 - sand::texture_width / 2 + ((chunk_x - sand::camera_chunk_x) * sand::chunk_width + tile_x - sand::camera_tile_x) * sand::texture_width;
		const std::uint64_t offset_y = sand::screen_height - sand::texture_height / 2 - ((chunk_y - sand::camera_chunk_y) * sand::chunk_height + tile_y - sand::camera_tile_y) * sand::texture_height;
		for (std::uint64_t texture_pixel_x = 0; texture_pixel_x < sand::texture_width; ++texture_pixel_x) {
			for (std::uint64_t texture_pixel_y = 0; texture_pixel_y < sand::texture_height; ++texture_pixel_y) {
				if (const auto [r, g, b, a] = sand::texture_at(texture_index, texture_pixel_x, texture_pixel_y); a) {
					sand::screen_at(offset_x + texture_pixel_x, offset_y + texture_pixel_y) = xieite::color3(r, g, b);
				}
			}
		}
	}

	constexpr void write_text(std::string_view text, const xieite::color3& fg, const xieite::color3& bg, std::uint64_t screen_x, std::uint64_t screen_y) noexcept {
		std::uint64_t row = 0;
		std::uint64_t col = 0;
		for (char c : text) {
			if (c == '\n') {
				++row;
				col = 0;
				continue;
			}
			for (std::uint64_t x = 0; x < sand::font_width; ++x) {
				for (std::uint64_t y = 0; y < sand::font_height; ++y) {
					if (sand::font_at(c, x, y)) {
						const std::uint64_t pixel_x = screen_x + col * sand::font_width + x;
						const std::uint64_t pixel_y = screen_y + row * sand::font_height + y;
						screen_at(pixel_x, pixel_y) = fg;
						screen_at(pixel_x + 1, pixel_y + 1) = bg;
					}
				}
			}
			++col;
		}
	}
}

int main() {
	const int terminal_blocking = ::fcntl(STDIN_FILENO, F_GETFL);
	const ::termios terminal_cooked = ([] -> ::termios {
		::termios terminal_cooked;
		::tcgetattr(STDIN_FILENO, &terminal_cooked);
		return terminal_cooked;
	})();
	{
		::termios terminal_raw = terminal_cooked;
		terminal_raw.c_iflag &= ~static_cast<::tcflag_t>(ICRNL | IXON);
		terminal_raw.c_lflag &= ~static_cast<::tcflag_t>(ECHO | ICANON | IEXTEN | ISIG);
		terminal_raw.c_oflag &= ~static_cast<::tcflag_t>(OPOST);
		::tcsetattr(STDIN_FILENO, TCSANOW, &terminal_raw);
	}
	std::print("\x1B[?47h\x1B[s\x1B[?25l\x1B[2J\x1B[0m");

	if (std::filesystem::exists("save.txt")) {
		const std::string data = xieite::read(xieite::file("save.txt", "r").get());
		std::uint64_t i = 0;

		sand::tick = sand::parse_u64(data, i);
		sand::camera_chunk_x = sand::parse_u64(data, i);
		sand::camera_chunk_y = sand::parse_u64(data, i);
		sand::camera_tile_x = sand::parse_u64(data, i);
		sand::camera_tile_y = sand::parse_u64(data, i);
		sand::selection_index = sand::parse_u64(data, i);
		sand::selection_start_x = sand::parse_u64(data, i);
		sand::selection_start_y = sand::parse_u64(data, i);
		sand::selection_end_x = sand::parse_u64(data, i);
		sand::selection_end_y = sand::parse_u64(data, i);

		while (i < data.size()) {
			std::uint64_t chunk_x = sand::parse_u64(data, i);
			std::uint64_t chunk_y = sand::parse_u64(data, i);
			for (std::uint64_t tile_y = sand::chunk_height; tile_y--;) {
				for (std::uint64_t tile_x = 0; tile_x < sand::chunk_width; ++tile_x) {
					sand::world[chunk_x][chunk_y][tile_x][tile_y] = sand::tiles[sand::parse_u64(data, i)];
				}
			}
		}
		
		std::uint64_t previous_screen_width = 0;
		std::uint64_t previous_screen_height = 0;
		std::vector<sand::display_char> previous_screen;
		for (;; ++sand::tick) {
			::winsize screen_size;
			::ioctl(::fileno(stdin), TIOCGWINSZ, &screen_size);
			sand::screen_width = screen_size.ws_col;
			sand::screen_height = screen_size.ws_row;

			sand::screen.clear();
			sand::screen.resize(sand::screen_width * sand::screen_height);

			for (std::uint64_t view_chunk_x = 0; view_chunk_x < 3; ++view_chunk_x) {
				for (std::uint64_t view_chunk_y = 0; view_chunk_y < 3; ++view_chunk_y) {
					for (std::uint64_t tile_x = 0; tile_x < sand::chunk_width; ++tile_x) {
						for (std::uint64_t tile_y = 0; tile_y < sand::chunk_height; ++tile_y) {
							const std::uint64_t chunk_x = sand::camera_chunk_x + view_chunk_x - 1;
							const std::uint64_t chunk_y = sand::camera_chunk_y + view_chunk_y - 1;
							sand::draw_texture(sand::world[chunk_x][chunk_y][tile_x][tile_y].texture_index, chunk_x, chunk_y, tile_x, tile_y);
						}
					}
				}
			}

			sand::draw_texture(sand::tiles[sand::selection_index].texture_index, sand::camera_chunk_x, sand::camera_chunk_y, sand::camera_tile_x, sand::camera_tile_y);
			sand::draw_texture( // top left corner
				0x0E,
				sand::camera_chunk_x - !sand::camera_tile_x,
				sand::camera_chunk_y + (sand::camera_tile_y == (sand::chunk_height - 1)),
				(sand::camera_tile_x - 1 + sand::chunk_width) % sand::chunk_width,
				(sand::camera_tile_y + 1) % sand::chunk_height
			);
			sand::draw_texture( // top left horizontal
				0x0F,
				sand::camera_chunk_x,
				sand::camera_chunk_y + (sand::camera_tile_y == (sand::chunk_height - 1)),
				sand::camera_tile_x,
				(sand::camera_tile_y + 1) % sand::chunk_height
			);
			sand::draw_texture( // top left vertical
				0x10,
				sand::camera_chunk_x - !sand::camera_tile_x,
				sand::camera_chunk_y,
				(sand::camera_tile_x - 1 + sand::chunk_width) % sand::chunk_width,
				sand::camera_tile_y
			);
			sand::draw_texture( // top right corner
				0x11,
				sand::camera_chunk_x + (sand::camera_tile_x == (sand::chunk_width - 1)),
				sand::camera_chunk_y + (sand::camera_tile_y == (sand::chunk_height - 1)),
				(sand::camera_tile_x + 1) % sand::chunk_width,
				(sand::camera_tile_y + 1) % sand::chunk_height
			);
			sand::draw_texture( // top right horizontal
				0x12,
				sand::camera_chunk_x,
				sand::camera_chunk_y + (sand::camera_tile_y == (sand::chunk_height - 1)),
				sand::camera_tile_x,
				(sand::camera_tile_y + 1) % sand::chunk_height
			);
			sand::draw_texture( // top right vertical
				0x13,
				sand::camera_chunk_x + (sand::camera_tile_x == (sand::chunk_width - 1)),
				sand::camera_chunk_y,
				(sand::camera_tile_x + 1) % sand::chunk_width,
				sand::camera_tile_y
			);
			sand::draw_texture( // bottom left corner
				0x14,
				sand::camera_chunk_x - !sand::camera_tile_x,
				sand::camera_chunk_y - !sand::camera_tile_y,
				(sand::camera_tile_x - 1 + sand::chunk_width) % sand::chunk_width,
				(sand::camera_tile_y - 1 + sand::chunk_height) % sand::chunk_height
			);
			sand::draw_texture( // bottom left horizontal
				0x15,
				sand::camera_chunk_x,
				sand::camera_chunk_y - !sand::camera_tile_y,
				sand::camera_tile_x,
				(sand::camera_tile_y - 1 + sand::chunk_height) % sand::chunk_height
			);
			sand::draw_texture( // bottom left vertical
				0x16,
				sand::camera_chunk_x - !sand::camera_tile_x,
				sand::camera_chunk_y,
				(sand::camera_tile_x - 1 + sand::chunk_width) % sand::chunk_width,
				sand::camera_tile_y
			);
			sand::draw_texture( // bottom right corner
				0x17,
				sand::camera_chunk_x + (sand::camera_tile_x == (sand::chunk_width - 1)),
				sand::camera_chunk_y - !sand::camera_tile_y,
				(sand::camera_tile_x + 1) % sand::chunk_width,
				(sand::camera_tile_y - 1 + sand::chunk_height) % sand::chunk_height
			);
			sand::draw_texture( // bottom right horizontal
				0x18,
				sand::camera_chunk_x,
				sand::camera_chunk_y - !sand::camera_tile_y,
				sand::camera_tile_x,
				(sand::camera_tile_y - 1 + sand::chunk_height) % sand::chunk_height
			);
			sand::draw_texture( // bottom right vertical
				0x19,
				sand::camera_chunk_x + (sand::camera_tile_x == (sand::chunk_width - 1)),
				sand::camera_chunk_y,
				(sand::camera_tile_x + 1) % sand::chunk_width,
				sand::camera_tile_y
			);

			sand::write_text(std::format(
				"tick: {:X}\n"
				"X:    {:X}\n"
				"Y:    {:X}\n"
				"x:    {:X}\n"
				"y:    {:X}\n"
				"hold: {:X}",
				sand::tick,
				static_cast<std::int64_t>(sand::camera_chunk_x),
				static_cast<std::int64_t>(sand::camera_chunk_y),
				sand::camera_tile_x,
				sand::camera_tile_y,
				sand::selection_index
			), 0xFFFFFF, 0x000000, 0, 0);

			std::string display;
			if (sand::screen != previous_screen) {
				const bool skippable = (sand::screen_width == previous_screen_width) && (sand::screen_height == previous_screen_height);
				previous_screen.resize(sand::screen.size());
				for (std::uint64_t pixel_y = 0; pixel_y < sand::screen_height; ++pixel_y) {
					for (std::uint64_t pixel_x = 0; pixel_x < sand::screen_width; ++pixel_x) {
						const std::uint64_t pixel_index = pixel_y * sand::screen_width + pixel_x;
						if (skippable && (sand::screen[pixel_index] == previous_screen[pixel_index])) {
							continue;
						}
						std::format_to(
							std::back_inserter(display),
							"\x1B[{};{}H\x1B[38;2;{};{};{}m\x1B[48;2;{};{};{}m▀\x1B[0m",
							pixel_y + 1,
							pixel_x + 1,
							sand::screen[pixel_index].pixels[0].r,
							sand::screen[pixel_index].pixels[0].g,
							sand::screen[pixel_index].pixels[0].b,
							sand::screen[pixel_index].pixels[1].r,
							sand::screen[pixel_index].pixels[1].g,
							sand::screen[pixel_index].pixels[1].b
						);
					}
				}
				previous_screen = sand::screen;
			}

			std::print("{}", display);
			std::fflush(stdout);

			std::this_thread::sleep_for(1000ms / 20);

			::fcntl(STDIN_FILENO, F_SETFL, terminal_blocking | O_NONBLOCK);
			if (([&] -> bool {
				while (true) {
					switch (std::fgetc(stdin)) {
					case '%':
						return true;
					case '[':
						sand::selection_index = (sand::selection_index - 1 + sand::tiles.size()) % sand::tiles.size();
						break;
					case ']':
						++sand::selection_index %= sand::tiles.size();
						break;
					case ' ':
						sand::world[sand::camera_chunk_x][sand::camera_chunk_y][sand::camera_tile_x][sand::camera_tile_y] = sand::tiles[sand::selection_index];
						break;
					case 'D':
					case 'd':
						sand::camera_chunk_x += !(++sand::camera_tile_x %= sand::chunk_width);
						break;
					case 'A':
					case 'a':
						sand::camera_chunk_x -= (sand::camera_tile_x = (sand::camera_tile_x - 1 + sand::chunk_width) % sand::chunk_width) == (sand::chunk_width - 1);
						break;
					case 'W':
					case 'w':
						sand::camera_chunk_y += !(++sand::camera_tile_y %= sand::chunk_height);
						break;
					case 'S':
					case 's':
						sand::camera_chunk_y -= (sand::camera_tile_y = (sand::camera_tile_y - 1 + sand::chunk_height) % sand::chunk_height) == (sand::chunk_height - 1);
						break;
					default:
						while (std::fgetc(stdin) > 0);
						return false;
					}
				}
			})()) {
				break;
			}
			::fcntl(STDIN_FILENO, F_SETFL, terminal_blocking);
		}
	}

	{
		auto file = xieite::file("save.txt", "w");
		std::println(
			file.get(),
			"{:X} {:X} {:X} {:X} {:X} {:X} {:X} {:X} {:X} {:X}",
			sand::tick,
			sand::camera_chunk_x,
			sand::camera_chunk_y,
			sand::camera_tile_x,
			sand::camera_tile_y,
			sand::selection_index,
			sand::camera_chunk_x,
			sand::camera_chunk_y,
			sand::camera_tile_x,
			sand::camera_tile_y
		);
		for (auto&& [chunk_x, chunks_column] : sand::world) {
			for (auto&& [chunk_y, chunk] : chunks_column) {
				if (([&] -> bool {
					for (auto&& tiles_column : chunk) {
						for (auto&& tile : tiles_column) {
							if (tile != sand::tiles[0]) {
								return false;
							}
						}
					}
					return true;
				})()) {
					continue;
				}
				std::println(file.get(), "{:X} {:X}", chunk_x, chunk_y);
				for (std::uint64_t tile_y = sand::chunk_height; tile_y--;) {
					for (std::uint64_t tile_x = 0; tile_x < (sand::chunk_width - 1); ++tile_x) {
						std::print(file.get(), "{:0>2X} ", sand::world[chunk_x][chunk_y][tile_x][tile_y].texture_index);
					}
					std::println(file.get(), "{:0>2X}", sand::world[chunk_x][chunk_y][sand::chunk_width - 1][tile_y].texture_index);
				}
			}
		}
	}

	std::print("\x1B[0m\x1B[?25h\x1B[u\x1B[?47l");
	::tcsetattr(STDIN_FILENO, TCSANOW, &terminal_cooked);
	::fcntl(STDIN_FILENO, F_SETFL, terminal_blocking);
}
