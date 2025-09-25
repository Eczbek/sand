#ifndef SAND_HEADER_TILE
#	define SAND_HEADER_TILE
#
#	include <array>
#	include <cstdint>

namespace sand {
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
}

#endif
