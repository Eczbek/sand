#ifndef SAND_HEADER_TILE
#	define SAND_HEADER_TILE
#
#	include <xte/util/number_types.hpp>
#
#	include <meta>

namespace sand {
	struct tile {
		xte::u64 texture_index = 0x00;
		bool background = false;
		bool transparent = false;

		[[nodiscard]] friend constexpr bool operator==(const sand::tile& lhs, const sand::tile& rhs) noexcept {
			return lhs.texture_index == rhs.texture_index;
		}
	};
	
	inline constexpr auto tiles = std::define_static_array(typename[:^^sand::tile[]:] {
		/* 0x00: void */           { 0x00, true },
		/* 0x01: stone */          { 0x01 },
		/* 0x02: cobbled stone */  { 0x02 },
		/* 0x03: dark blue */      { 0x03, true },
		/* 0x04: light blue */     { 0x04, true },
		/* 0x05: rainbow */        { 0x05 },
		/* 0x06: dirt */           { 0x06 },
		/* 0x07: rock */           { 0x07 },
		/* 0x08: leaves */         { 0x08 },
		/* 0x09: wood */           { 0x09 },
		/* 0x0A: grass */          { 0x0A },
		/* 0x0B: flowers */        { 0x0B },
		/* 0x0C: ice */            { 0x0C },
		/* 0x0D: chiseled ice */   { 0x0D },
		/* 0x0E: slate */          { 0x1B },
		/* 0x0F: wood planks */    { 0x1C },
		/* 0x10: stone bricks */   { 0x1D },
		/* 0x11: glass */          { 0x1E, false, true },
		/* 0x12: conveyor right */ { 0x1F },
		/* 0x13: conveyor right */ { 0x20 },
		/* 0x14: conveyor up */    { 0x21 },
		/* 0x15: conveyor down */  { 0x22 }
	});
}

#endif
