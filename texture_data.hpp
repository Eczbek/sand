#ifndef SAND_HEADER_TEXTURE_DATA
#	define SAND_HEADER_TEXTURE_DATA
#
#	include <array>
#	include <cstdint>
#	include <string_view>

namespace sand {
	inline constexpr std::uint64_t texture_width = 8;
	inline constexpr std::uint64_t texture_height = 8;
	inline constexpr std::array<std::string_view, 0x21> texture_data = { {
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
}

#endif
