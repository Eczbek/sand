#ifndef SAND_HEADER_POS
#	define SAND_HEADER_POS
#
#	include <xte/util/number_types.hpp>

namespace sand {
	static constexpr xte::u64 chunk_w = 32;
	static constexpr xte::u64 chunk_h = 32;

	struct pos {
		xte::u64 chunk_x;
		xte::u64 chunk_y;
		xte::u64 tile_x;
		xte::u64 tile_y;

		[[nodiscard]] friend constexpr bool operator==(const sand::pos&, const sand::pos&) noexcept = default;

		constexpr sand::pos& operator+=(const sand::pos& rhs) noexcept {
			this->chunk_x += rhs.chunk_x + ((sand::chunk_w - rhs.tile_x) <= this->tile_x);
			this->chunk_y += rhs.chunk_y + ((sand::chunk_h - rhs.tile_y) <= this->tile_y);
			(this->tile_x += rhs.tile_x) %= sand::chunk_w;
			(this->tile_y += rhs.tile_y) %= sand::chunk_h;
			return *this;
		}

		[[nodiscard]] friend constexpr sand::pos operator+(sand::pos lhs, const sand::pos& rhs) noexcept {
			return lhs += rhs;
		}

		constexpr sand::pos& operator-=(const sand::pos& rhs) noexcept {
			this->chunk_x -= rhs.chunk_x + (rhs.tile_x > this->tile_x);
			this->chunk_y -= rhs.chunk_y + (rhs.tile_y > this->tile_y);
			(this->tile_x += sand::chunk_w - rhs.tile_x) %= sand::chunk_w;
			(this->tile_y += sand::chunk_h - rhs.tile_y) %= sand::chunk_h;
			return *this;
		}

		[[nodiscard]] friend constexpr sand::pos operator-(sand::pos lhs, const sand::pos& rhs) noexcept {
			return lhs -= rhs;
		}
	};
}

#endif
