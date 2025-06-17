#include <array>
#include <chrono> // IWYU pragma: keep
#include <cstddef>
#include <cstdio>
#include <print>
#include <thread>
#include <vector>
#include <xieite/io/term.hpp>
#include <xieite/io/keys.hpp>
#include <xieite/math/color3.hpp>
#include <xieite/math/color4.hpp>
#include <xieite/math/ssize_t.hpp>

namespace gt {
	constexpr std::size_t texture_w = 8;
	constexpr std::size_t texture_h = 8;

	constexpr std::array<std::string_view, 5> textures = {
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		"        "
		,
		"22222222"
		"25554432"
		"24545441"
		"24444331"
		"23443431"
		"14444331"
		"13433331"
		"11111111"
		,
		"14314523"
		"32142421"
		"15433254"
		"52321143"
		"43154323"
		"32543251"
		"15112542"
		"14331433"
		,
		"rr    rr"
		"r      r"
		"        "
		"        "
		"        "
		"        "
		"r      r"
		"rr    rr"
		,
		" r    r "
		"rr    rr"
		"        "
		"        "
		"        "
		"        "
		"rr    rr"
		" r    r "
	};
}

using namespace std::literals;

int main() {
	constexpr std::size_t world_w = 16;
	constexpr std::size_t world_h = 16;

	auto world = std::array<std::array<std::size_t, world_h>, world_w>();
	for (std::size_t i = 0; i < 16; ++i) {
		world[i][0] = 1;
		world[i][1] = 1;
	}

	xieite::term term;
	term.echo(false);
	term.canon(false);
	term.signal(false);
	term.proc(false);
	term.cursor_alt(true);
	term.screen_alt(true);
	term.cursor_invis(true);
	term.clear_screen();

	xieite::ssize_t player_x = 0;
	xieite::ssize_t player_y = 0;
	std::size_t held_tile = 0;

	xieite::ssize_t prev_w = 0;
	xieite::ssize_t prev_h = 0;
	std::vector<std::array<xieite::color3, 2>> prev_buf;

	std::uint64_t i = 0;
	while (true) {
		const auto [curr_h, curr_w] = term.screen_size();
		std::vector<std::array<xieite::color3, 2>> curr_buf;
		curr_buf.resize(static_cast<std::size_t>(curr_w) * static_cast<std::size_t>(curr_h));

		auto draw_texture = [&](std::size_t id, xieite::ssize_t x, xieite::ssize_t y) -> void {
			const xieite::ssize_t texture_w = static_cast<xieite::ssize_t>(gt::texture_w);
			const xieite::ssize_t texture_h = static_cast<xieite::ssize_t>(gt::texture_h);
			const xieite::ssize_t offset_x = curr_w / 2 + x * texture_w - player_x * texture_w - texture_w / 2;
			const xieite::ssize_t offset_y = curr_h - y * texture_h + player_y * texture_h - texture_h / 2;
			for (xieite::ssize_t x = 0; x < static_cast<xieite::ssize_t>(gt::texture_w); ++x) {
				for (xieite::ssize_t y = 0; y < static_cast<xieite::ssize_t>(gt::texture_h); ++y) {
					const xieite::ssize_t idx = (offset_y + y) / 2 * curr_w + offset_x + x;
					if ((idx < 0) || (((offset_y + y) / 2) >= curr_h) || ((offset_x + x) >= curr_w)) {
						continue;
					}
					if (const auto [r, g, b, a] = ([=] -> xieite::color4 {
						switch (gt::textures[id][static_cast<std::size_t>(y) * gt::texture_w + static_cast<std::size_t>(x)]) {
							case ' ':
								return 0x00000000;
							case '0':
								return 0x000000FF;
							case '1':
								return 0x373737FF;
							case '2':
								return 0x4C4C4CFF;
							case '3':
								return 0x707070FF;
							case '4':
								return 0x8D8D8DFF;
							case '5':
								return 0xC0C0C0FF;
							case 'r':
								return 0xFF0000FF;
						}
						return 0xFF00FFFF;
					})(); a) {
						curr_buf[static_cast<std::size_t>(idx)][!!(offset_y % 2) != !!(y % 2)] = xieite::color3(r, g, b);
					}
				}
			}
		};

		for (std::size_t x = 0; x < world_w; ++x) {
			for (std::size_t y = 0; y < world_h; ++y) {
				draw_texture(world[x][y], static_cast<xieite::ssize_t>(x), static_cast<xieite::ssize_t>(y));
			}
		}
		if (held_tile) {
			draw_texture(held_tile, player_x, player_y);
			draw_texture(4, player_x, player_y);
		} else {
			draw_texture(3, player_x, player_y);
		}

		if (prev_buf != curr_buf) {
			const bool skippable = (prev_w == curr_w) && (prev_h == curr_h);
			prev_buf.resize(curr_buf.size());
			std::string display;
			for (xieite::ssize_t y = 0; y < curr_h; ++y) {
				for (xieite::ssize_t x = 0; x < curr_w; ++x) {
					if (skippable && (prev_buf[static_cast<std::size_t>(y * curr_w + x)] == curr_buf[static_cast<std::size_t>(y * curr_w + x)])) {
						continue;
					}
					display += term.set_cursor_code(y, x);
					display += term.fg_code(curr_buf[static_cast<std::size_t>(y * curr_w + x)][0]);
					display += term.bg_code(curr_buf[static_cast<std::size_t>(y * curr_w + x)][1]);
					display += "▀";
					display += term.reset_style_code();
				}
			}
			prev_buf = curr_buf;
			std::fputs(display.c_str(), term.out);
			std::fflush(term.out);
		}

		std::print(term.out, "\x1B[H\x1B[0m");
		std::print(term.out, "tick: {}\r\n", i++);
		std::print(term.out, "x: {}, y: {}\r\n", player_x, player_y);
		std::print(term.out, "held tile: {}\r\n", held_tile);
		std::fflush(term.out);
		std::this_thread::sleep_for(1000ms / 30);

		term.block(false);
		if (([&] -> bool {
			while (true) {
				switch (term.read_key()) {
				case xieite::keys::ctrl_q:
					return true;
				case xieite::keys::right:
					++player_x;
					break;
				case xieite::keys::left:
					--player_x;
					break;
				case xieite::keys::up:
					++player_y;
					break;
				case xieite::keys::down:
					--player_y;
					break;
				case xieite::keys::space:
					if ((player_x >= 0) && (static_cast<std::size_t>(player_x) < world_w) && (player_y >= 0) && (static_cast<std::size_t>(player_y) < world_h)) {
						std::swap(held_tile, world[static_cast<std::size_t>(player_x)][static_cast<std::size_t>(player_y)]);
					}
					break;
				default:
					while (std::fgetc(term.in) > 0);
					return false;
				}
			}
		})()) {
			break;
		}
		term.block(true);
	}
}
