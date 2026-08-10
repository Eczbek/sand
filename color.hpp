#ifndef SAND_HEADER_COLOR
#	define SAND_HEADER_COLOR
#
#	include <xte/util/number_types.hpp>

namespace sand {
	struct color3 {
		xte::u8 r;
		xte::u8 g;
		xte::u8 b;

		[[nodiscard]] constexpr color3(xte::u8 r, xte::u8 g, xte::u8 b) noexcept
		: r(r), g(g), b(b) {}

		[[nodiscard]] explicit(false) constexpr color3(xte::u32 x = 0) noexcept
		: r(static_cast<xte::u8>(x >> 16))
		, g(static_cast<xte::u8>(x >> 8))
		, b(static_cast<xte::u8>(x))
		{}

		[[nodiscard]] friend bool operator==(const sand::color3&, const sand::color3&) = default;

		[[nodiscard]] constexpr xte::u32 value() const noexcept {
			return (static_cast<xte::u32>(this->r) << 16)
				| (static_cast<xte::u32>(this->g) << 8) 
				| static_cast<xte::u32>(this->b);
		}
	};

	struct color4 {
		xte::u8 r;
		xte::u8 g;
		xte::u8 b;
		xte::u8 a;

		[[nodiscard]] constexpr color4(xte::u8 r, xte::u8 g, xte::u8 b, xte::u8 a = 0xFF) noexcept
		: r(r), g(g), b(b), a(a) {}

		[[nodiscard]] explicit(false) constexpr color4(xte::u32 x = 0) noexcept
		: r(static_cast<xte::u8>(x >> 24))
		, g(static_cast<xte::u8>(x >> 16))
		, b(static_cast<xte::u8>(x >> 8))
		, a(static_cast<xte::u8>(x))
		{}

		[[nodiscard]] friend bool operator==(const sand::color4&, const sand::color4&) = default;

		[[nodiscard]] constexpr xte::u32 value() const noexcept {
			return (static_cast<xte::u32>(this->r) << 24)
				| (static_cast<xte::u32>(this->g) << 16)
				| (static_cast<xte::u32>(this->b) << 8)
				| static_cast<xte::u32>(this->a);
		}
	};
}

#endif
