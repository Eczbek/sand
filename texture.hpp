#ifndef SAND_HEADER_TEXTURE
#	define SAND_HEADER_TEXTURE
#
#	include <xte/trait/is_castable_implicit_noex.hpp>
#	include <xte/util/init_list.hpp>
#	include <xte/util/number_types.hpp>
#
#	include <meta>

namespace sand {
	struct texture {
		const xte::u64* frames;
		xte::u64 frames_count;

		[[nodiscard]] explicit(false) consteval texture(xte::is_castable_implicit_noex<xte::u64> auto&&... frames) noexcept
		: frames(std::define_static_array(typename[:^^xte::u64[]:] { static_cast<xte::u64>(frames)... }).data()), frames_count(sizeof...(frames)) {}
	};

	inline constexpr auto textures = std::define_static_array(typename[:^^sand::texture[]:] {
		/* 0x00: empty */                            { 0x23 },
		/* 0x01: stone */                            { 0x01 },
		/* 0x02: cobbled stone */                    { 0x02 },
		/* 0x03: dark blue */                        { 0x05 },
		/* 0x04: light blue */                       { 0x06 },
		/* 0x05: rainbow */                          { 0x07, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 },
		/* 0x06: dirt */                             { 0x08 },
		/* 0x07: rock */                             { 0x09 },
		/* 0x08: leaves */                           { 0x0A },
		/* 0x09: wood */                             { 0x0B },
		/* 0x0A: grass */                            { 0x0C },
		/* 0x0B: flowers */                          { 0x0D },
		/* 0x0C: ice */                              { 0x10 },
		/* 0x0D: chiseled ice */                     { 0x11 },
		/* 0x0E: cursor (top left corner) */         { 0x03 },
		/* 0x0F: cursor (top left horizontal) */     { 0x04 },
		/* 0x10: cursor (top left vertical) */       { 0x0E },
		/* 0x11: cursor (top right corner) */        { 0x0F },
		/* 0x12: cursor (top right horizonal) */     { 0x19 },
		/* 0x13: cursor (top right vertical) */      { 0x1A },
		/* 0x14: cursor (bottom left corner) */      { 0x1B },
		/* 0x15: cursor (bottom left horizontal) */  { 0x1C },
		/* 0x16: cursor (bottom left vertical) */    { 0x1D },
		/* 0x17: cursor (bottom right corner) */     { 0x1E },
		/* 0x18: cursor (bottom right horizontal) */ { 0x1F },
		/* 0x19: cursor (bottom right vertical) */   { 0x20 },
		/* 0x1A: missing texture */                  { 0x21 },
		/* 0x1B: slate */                            { 0x22 }
	});
}

#endif
