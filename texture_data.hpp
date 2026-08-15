#ifndef SAND_HEADER_TEXTURE_DATA
#	define SAND_HEADER_TEXTURE_DATA
#
#	include <xte/data/static_string_view.hpp>
#	include <xte/util/number_types.hpp>
#
#	include <meta>

namespace sand {
	inline constexpr xte::u64 texture_w = 8;
	inline constexpr xte::u64 texture_h = 8;
	inline constexpr auto texture_data = std::define_static_array(typename[:^^xte::static_string_view[]:] {
		"        " // 0x00: empty
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"22222222" // 0x01: stone
		"25554432"
		"24545441"
		"24444331"
		"23443431"
		"14444331"
		"13433331"
		"11111111"
		,
		"14325413" // 0x02: cobble
		"32142521"
		"15433254"
		"52321143"
		"43154323"
		"32543251"
		"14112542"
		"25431424"
		,
		"        " // 0x03: cursor (top left corner)
		"        "
		"        "
		"        "
		"        "
		"        "
		"      99"
		"      9&"
		,
		"        " // 0x04: cursor (top left horizontal)
		"        "
		"        "
		"        "
		"        "
		"        "
		"99      "
		"&*      "
		,
		"llll...." // 0x05: blue (dark)
		"llll...."
		"llll...."
		"llll...."
		"....llll"
		"....llll"
		"....llll"
		"....llll"
		,
		"llll<<<<" // 0x06: blue (light)
		"llll<<<<"
		"llll<<<<"
		"llll<<<<"
		"<<<<llll"
		"<<<<llll"
		"<<<<llll"
		"<<<<llll"
		,
		"asdfbk<;" // 0x07: rainbow (frame 0)
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		,
		"QWETEREW" // 0x08: dirt
		"WERQWETE"
		"ETWERTET"
		"WERTEWQW"
		"REWETETE"
		"EWQREQWR"
		"WQRWEWET"
		"ETERTERT"
		,
		"35243214" // 0x09: rock
		"23435345"
		"35543532"
		"43355442"
		"32453534"
		"24534321"
		"43355434"
		"12424523"
		,
		"BNHBNbGH" // 0x0A: leaves
		"NgbNHGBN"
		"BbGBNNGH"
		"HGBHBNHB"
		"BNNHgbBN"
		"gHBbGbGN"
		"bGHGBGBH"
		"GBNNGBHB"
		,
		"QEETWRQR" // 0x0B: wood
		"QRWEWTWE"
		"WRQRETER"
		"ETWEERWE"
		"RTETWEQR"
		"EETRERQT"
		"WREQETET"
		"ETWQTEWE"
		,
		"GNGbHgHB" // 0x0C: grass
		"gHBbGNBG"
		"NBGGBNGg"
		"NBgHBGBb"
		"BHbBNGBG"
		"bHGHbGNH"
		"BGBgNBNG"
		"NGNbGNHB"
		,
		"cdxkfknh" // 0x0D: flowers
		"dfs,AaAf"
		"xsxNafzh"
		"zA,kAzAZ"
		"fakfkcdx"
		"axsc,dfs"
		"JdfdNxsc"
		"mcsxk,jf"
		,
		"      9&" // 0x0E: cursor (top left vertical)
		"      *&"
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"        " // 0x0F: cursor (top right corner)
		"        "
		"        "
		"        "
		"        "
		"        "
		"99      "
		"&&      "
		,
		"{[[]]{[]" // 0x10: ice
		"[[]{[]]}"
		"{][]][{}"
		"][{][{{]"
		"}{][}]]{"
		"][]{]]{}"
		"}]{]{}[{"
		"{}{}}{}}"
		,
		"[[[[[[[{" // 0x11: chiseled ice
		"[{{{{{]}"
		"[{]]]][}"
		"[{]]]][}"
		"[{]]]][}"
		"[{]]]][}"
		"[][[[[[}"
		"{}}}}}}}"
		,
		"sdfbk<;a" // 0x12: rainbow (frame 1)
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		,
		"dfbk<;as" // 0x13: rainbow (frame 2)
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		,
		"fbk<;asd" // 0x14: rainbow (frame 3)
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		,
		"bk<;asdf" // 0x15: rainbow (frame 4)
		"k<;asdfb"
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		,
		"k<;asdfb" // 0x16: rainbow (frame 5)
		"<;asdfbk"
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		,
		"<;asdfbk" // 0x17: rainbow (frame 6)
		";asdfbk<"
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		,
		";asdfbk<" // 0x18: rainbow (frame 7)
		"asdfbk<;"
		"sdfbk<;a"
		"dfbk<;as"
		"fbk<;asd"
		"bk<;asdf"
		"k<;asdfb"
		"<;asdfbk"
		,
		"        " // 0x19: cursor (top right horizontal)
		"        "
		"        "
		"        "
		"        "
		"        "
		"      99"
		"      *&"
		,
		"9&      " // 0x1A: cursor (top right vertical)
		"9&      "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"      9&" // 0x1B: cursor (bottom left corner)
		"      *&"
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"99      " // 0x1C: cursor (bottom left horizontal)
		"&&      "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"        " // 0x1D: cursor (bottom left vertical)
		"        "
		"        "
		"        "
		"        "
		"        "
		"      99"
		"      9&"
		,
		"9&      " // 0x1E: cursor (bottom right corner)
		"&&      "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"      99" // 0x1F: cursor (bottom right horizontal)
		"      9*"
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"        " // 0x20: cursor (bottom right vertical)
		"        "
		"        "
		"        "
		"        "
		"        "
		"99      "
		"*&      "
		,
		"::::0000" // 0x21: missing texture
		"::::0000"
		"::::0000"
		"::::0000"
		"0000::::"
		"0000::::"
		"0000::::"
		"0000::::"
		,
		"!!@!112#" // 0x22: slate
		"$@!1$#@!"
		"!#1$@!@$"
		"#1$@@2$#"
		"2$@!1$1!"
		"1!@1$1!@"
		"!@1$@!@2"
		"#$#@@!!!"
		,
		"0TRT000T" // 0x23: background
		"0RT00TR0"
		"RT000R00"
		"T00TR00T"
		"00RT0RT0"
		"TRT0RT00"
		"RT0RT00T"
		"00TT00T0"
		,
		"qqwwrqwq" // 0x24: wood planks
		"erreerrr"
		"qweqwwqq"
		"ereerrre"
		"qwqwqweq"
		"eeerrerr"
		"rqqwwqqw"
		"rreeeree"
		,
		"^&&&^%!&" // 0x25: stone bricks
		"%^^%%$#&"
		"$%$%$%#%"
		"!###!!!!"
		"^%!&&&^%"
		"$$#^%^%^"
		"%$!%^$$$"
		"!#!#!#!!"
		,
		"979  977" // 0x26: glass
		"   9   9"
		"9 7     "
		"7      9"
		"9      7"
		"      79"
		"7    7  "
		"79 7  97"
		,
		"*&*&&**&" // 0x27: conveyor right (frame 0)
		"&&%%&%&%"
		"##@!##@!"
		"!##@!##@"
		"!##@!##@"
		"##@!##@!"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&*&&**&" // 0x28: conveyor right (frame 1)
		"&&%%&%&%"
		"!##@!##@"
		"@!##@!##"
		"@!##@!##"
		"!##@!##@"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&*&&**&" // 0x29: conveyor right (frame 2)
		"&&%%&%&%"
		"@!##@!##"
		"#@!##@!#"
		"#@!##@!#"
		"@!##@!##"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&*&&**&" // 0x2A: conveyor right (frame 3)
		"&&%%&%&%"
		"#@!##@!#"
		"##@!##@!"
		"##@!##@!"
		"#@!##@!#"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&*&&**&" // 0x2B: conveyor left (frame 0)
		"&&%%&%&%"
		"##!@##!@"
		"#!@##!@#"
		"#!@##!@#"
		"##!@##!@"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&*&&**&" // 0x2C: conveyor left (frame 1)
		"&&%%&%&%"
		"#!@##!@#"
		"!@##!@##"
		"!@##!@##"
		"#!@##!@#"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&*&&**&" // 0x2D: conveyor left (frame 2)
		"&&%%&%&%"
		"!@##!@##"
		"@##!@##!"
		"@##!@##!"
		"!@##!@##"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&*&&**&" // 0x2E: conveyor left (frame 3)
		"&&%%&%&%"
		"@##!@##!"
		"##!@##!@"
		"##!@##!@"
		"@##!@##!"
		"***&**&&"
		"*&&%%&%%"
		,
		"*&#!!#**" // 0x2F: conveyor up (frame 0)
		"*&!@@!*&"
		"*%@##@&&"
		"&%####&%"
		"*&#!!#*&"
		"&&!@@!*%"
		"&%@##@&%"
		"%&####*&"
		,
		"*&!@@!**" // 0x30: conveyor up (frame 1)
		"*&@##@*&"
		"*%####&&"
		"&%#!!#&%"
		"*&!@@!*&"
		"&&@##@*%"
		"&%####&%"
		"%&#!!#*&"
		,
		"*&@##@**" // 0x31: conveyor up (frame 2)
		"*&####*&"
		"*%#!!#&&"
		"&%!@@!&%"
		"*&@##@*&"
		"&&####*%"
		"&%#!!#&%"
		"%&!@@!*&"
		,
		"*&####**" // 0x32: conveyor up (frame 3)
		"*&#!!#*&"
		"*%!@@!&&"
		"&%@##@&%"
		"*&####*&"
		"&&#!!#*%"
		"&%!@@!&%"
		"%&@##@*&"
		,
		"*&!@@!**" // 0x33: conveyor down (frame 0)
		"*&#!!#*&"
		"*%####&&"
		"&%@##@&%"
		"*&!@@!*&"
		"&&#!!#*%"
		"&%####&%"
		"%&@##@*&"
		,
		"*&@##@**" // 0x34: conveyor down (frame 1)
		"*&!@@!*&"
		"*%#!!#&&"
		"&%####&%"
		"*&@##@*&"
		"&&!@@!*%"
		"&%#!!#&%"
		"%&####*&"
		,
		"*&####**" // 0x35: conveyor down (frame 1)
		"*&@##@*&"
		"*%!@@!&&"
		"&%#!!#&%"
		"*&####*&"
		"&&@##@*%"
		"&%!@@!&%"
		"%&#!!#*&"
		,
		"*&#!!#**" // 0x36: conveyor down (frame 1)
		"*&####*&"
		"*%@##@&&"
		"&%!@@!&%"
		"*&#!!#*&"
		"&&####*%"
		"&%@##@&%"
		"%&!@@!*&"
	});
}

#endif
