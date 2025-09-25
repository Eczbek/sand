#ifndef SAND_HEADER_TEXTURE
#	define SAND_HEADER_TEXTURE
#
#	include <array>
#	include <cstdint>

namespace sand {
	struct texture {
		std::uint64_t frames_count = 1;
		std::array<std::uint64_t, 8> frame_indices;
	};

	inline constexpr std::array<sand::texture, 0x20> textures = { {
		/* 00 void */                             { 1, { 0x00 } },
		/* 01 stone */                            { 1, { 0x01 } },
		/* 02 cobbled stone */                    { 1, { 0x02 } },
		/* 03 dark blue */                        { 1, { 0x05 } },
		/* 04 light blue */                       { 1, { 0x06 } },
		/* 05 rainbow */                          { 8, { { 0x07, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 } } },
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
}

#endif
