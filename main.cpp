#include <array>
#include <chrono> // IWYU pragma: keep
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <print>
#include <string>
#include <thread>
#include <vector>
#include <xieite/io/file.hpp>
#include <xieite/io/keys.hpp>
#include <xieite/io/term.hpp>
#include <xieite/math/abs.hpp>
#include <xieite/math/color3.hpp>
#include <xieite/math/color4.hpp>

namespace sand {
	constexpr std::uint64_t texture_w = 8;
	constexpr std::uint64_t texture_h = 8;
	constexpr std::array<std::string_view, 8> textures = {
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
		,
		"BBBBbbbb"
		"BBBBbbbb"
		"BBBBbbbb"
		"BBBBbbbb"
		"bbbbBBBB"
		"bbbbBBBB"
		"bbbbBBBB"
		"bbbbBBBB"
		,
		"BBBBjjjj"
		"BBBBjjjj"
		"BBBBjjjj"
		"BBBBjjjj"
		"jjjjBBBB"
		"jjjjBBBB"
		"jjjjBBBB"
		"jjjjBBBB"
		,
		"asdfghjk"
		"sdfghjka"
		"dfghjkas"
		"fghjkasd"
		"ghjkasdf"
		"hjkasdfg"
		"jkasdfgh"
		"kasdfghj"
	};

	struct tile {
		std::size_t texture;
		bool transparent;
	};
	constexpr std::array<sand::tile, 6> tiles = {{
		/* 00 void */ { 0, true },
		/* 01 stone */ { 1, false },
		/* 02 cobble */ { 2, false },
		/* 03 blue? */ { 5, false },
		/* 04 */ { 6, true },
		/* 05 test */ { 7, false }
	}};

	constexpr std::uint64_t chunk_w = 16;
	constexpr std::uint64_t chunk_h = 16;
	constexpr std::int64_t view_dist = 1;

	template<std::integral Int>
	[[nodiscard]] constexpr Int parse(std::string_view strv, std::size_t& i) noexcept {
		while (" \n"sv.contains(strv[i])) {
			++i;
		}
		const bool neg = strv[i] == '-';
		Int result = 0;
		for (i += neg; i < strv.size(); ++i) {
			const std::size_t digit = "0123456789ABCDEF"sv.find(strv[i]);
			if (digit == std::string::npos) {
				break;
			}
			result = result * 16 + static_cast<Int>(digit);
		}
		++i;
		return neg ? -result : result;
	}
}

int main() {
	using namespace std::literals;

	std::uint64_t tick = 0;
	std::int64_t player_chunk_x = 0;
	std::int64_t player_chunk_y = 0;
	std::uint64_t player_subchunk_x = 0;
	std::uint64_t player_subchunk_y = 0;
	std::uint64_t holding = 0;
	std::unordered_map<std::int64_t, std::unordered_map<std::int64_t, std::array<std::array<std::size_t, sand::chunk_h>, sand::chunk_w>>> world;
	if (std::filesystem::exists("save.txt")) {
		const std::string data = xieite::read(xieite::file("save.txt", "r").get());
		std::size_t i = 0;
		tick = sand::parse<std::uint64_t>(data, i);
		player_chunk_x = sand::parse<std::int64_t>(data, i);
		player_chunk_y = sand::parse<std::int64_t>(data, i);
		player_subchunk_x = sand::parse<std::uint64_t>(data, i);
		player_subchunk_y = sand::parse<std::uint64_t>(data, i);
		holding = sand::parse<std::uint64_t>(data, i);

		while (i < data.size()) {
			std::int64_t chunk_x = sand::parse<std::int64_t>(data, i);
			std::int64_t chunk_y = sand::parse<std::int64_t>(data, i);
			for (std::uint64_t subchunk_y = sand::chunk_h; subchunk_y--;) {
				for (std::uint64_t subchunk_x = 0; subchunk_x < sand::chunk_w; ++subchunk_x) {
					world[chunk_x][chunk_y][subchunk_x][subchunk_y] = sand::parse<std::uint64_t>(data, i);
				}
			}
		}
	}

	const auto move = [&](std::int64_t x, std::int64_t y) -> void {
		const std::uint64_t subchunk_x = (player_subchunk_x + static_cast<std::uint64_t>(x) + sand::chunk_w) % sand::chunk_w;
		const std::uint64_t subchunk_y = (player_subchunk_y + static_cast<std::uint64_t>(y) + sand::chunk_h) % sand::chunk_h;
		const std::int64_t chunk_x = player_chunk_x + x * (subchunk_x == (sand::chunk_w - (x < 0)) % sand::chunk_w);
		const std::int64_t chunk_y = player_chunk_y + y * (subchunk_y == (sand::chunk_h - (y < 0)) % sand::chunk_h);
		if (!sand::tiles[holding].transparent && !sand::tiles[world[chunk_x][chunk_y][subchunk_x][subchunk_y]].transparent) {
			return;
		}
		player_chunk_x = chunk_x;
		player_chunk_y = chunk_y;
		player_subchunk_x = subchunk_x;
		player_subchunk_y = subchunk_y;
	};

	xieite::term term;
	term.echo(false);
	term.canon(false);
	term.signal(false);
	term.proc(false);
	term.cursor_alt(true);
	term.screen_alt(true);
	term.cursor_invis(true);
	term.clear_screen();

	std::int64_t prev_w = 0;
	std::int64_t prev_h = 0;
	std::vector<std::array<xieite::color3, 2>> prev_buf;

	while (true) {
		const auto [curr_h, curr_w] = term.screen_size();
		std::vector<std::array<xieite::color3, 2>> curr_buf;
		curr_buf.resize(static_cast<std::size_t>(curr_w) * static_cast<std::size_t>(curr_h));

		auto draw = [&](std::size_t texture, std::int64_t chunk_x, std::int64_t chunk_y, std::uint64_t subchunk_x, std::uint64_t subchunk_y) -> void {
			static constexpr std::int64_t texture_w = static_cast<std::int64_t>(sand::texture_w);
			static constexpr std::int64_t texture_h = static_cast<std::int64_t>(sand::texture_h);
			const std::int64_t offset_x = curr_w / 2 + (chunk_x * static_cast<std::int64_t>(sand::chunk_w) + static_cast<std::int64_t>(subchunk_x)) * texture_w - (player_chunk_x * static_cast<std::int64_t>(sand::chunk_w) + static_cast<std::int64_t>(player_subchunk_x)) * texture_w - texture_w / 2;
			const std::int64_t offset_y = curr_h - (chunk_y * static_cast<std::int64_t>(sand::chunk_h) + static_cast<std::int64_t>(subchunk_y)) * texture_h + (player_chunk_y * static_cast<std::int64_t>(sand::chunk_h) + static_cast<std::int64_t>(player_subchunk_y)) * texture_h - texture_h / 2;
			for (std::int64_t subchunk_x = 0; subchunk_x < texture_w; ++subchunk_x) {
				for (std::int64_t subchunk_y = 0; subchunk_y < texture_h; ++subchunk_y) {
					const std::int64_t idx = (offset_y + subchunk_y) / 2 * curr_w + offset_x + subchunk_x;
					if ((idx < 0) || (((offset_y + subchunk_y) / 2) >= curr_h) || ((offset_x + subchunk_x) >= curr_w) || ((offset_x + subchunk_x) < 0)) {
						continue;
					}
					if (const auto [r, g, b, a] = ([=] -> xieite::color4 {
						switch (sand::textures[texture][static_cast<std::size_t>(subchunk_y * texture_w + subchunk_x)]) {
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
							case 'B':
								return 0x00007FFF;
							case 'b':
								return 0x00003FFF;
							case '#':
								return 0xFFFFFFFF;
							case 'a':
								return 0xFF0000FF;
							case 's':
								return 0xFF7F00FF;
							case 'd':
								return 0xFFAF00FF;
							case 'f':
								return 0xFFFF00FF;
							case 'g':
								return 0x00FF00FF;
							case 'h':
								return 0x007FFFFF;
							case 'j':
								return 0x0000FFFF;
							case 'k':
								return 0x7F00FFFF;
						}
						return 0xFF00FFFF;
					})(); a) {
						curr_buf[static_cast<std::size_t>(idx)][!!(offset_y % 2) != !!(subchunk_y % 2)] = xieite::color3(r, g, b);
					}
				}
			}
		};

		for (std::int64_t chunk_x = player_chunk_x - sand::view_dist; chunk_x <= player_chunk_x + sand::view_dist; ++chunk_x) {
			for (std::int64_t chunk_y = player_chunk_y - sand::view_dist; chunk_y <= player_chunk_y + sand::view_dist; ++chunk_y) {
				if (!world[chunk_x].contains(chunk_y)) {
					for (std::size_t subchunk_x = 0; subchunk_x < sand::chunk_w; ++subchunk_x) {
						for (std::size_t subchunk_y = 0; subchunk_y < sand::chunk_h; ++subchunk_y) {
							world[chunk_x][chunk_y][subchunk_x][subchunk_y] = 0;
						}
					}
				}
				for (std::uint64_t subchunk_x = 0; subchunk_x < sand::chunk_w; ++subchunk_x) {
					for (std::uint64_t subchunk_y = 0; subchunk_y < sand::chunk_h; ++subchunk_y) {
						draw(sand::tiles[world[chunk_x][chunk_y][subchunk_x][subchunk_y]].texture, chunk_x, chunk_y, subchunk_x, subchunk_y);
					}
				}
			}
		}
		if (!sand::tiles[holding].transparent) {
			draw(sand::tiles[holding].texture, player_chunk_x, player_chunk_y, player_subchunk_x, player_subchunk_y);
			draw(4, player_chunk_x, player_chunk_y, player_subchunk_x, player_subchunk_y);
		} else {
			draw(3, player_chunk_x, player_chunk_y, player_subchunk_x, player_subchunk_y);
		}

		if (prev_buf != curr_buf) {
			const bool skippable = (prev_w == curr_w) && (prev_h == curr_h);
			prev_buf.resize(curr_buf.size());
			std::string display;
			for (std::int64_t subchunk_y = 0; subchunk_y < curr_h; ++subchunk_y) {
				for (std::int64_t subchunk_x = 0; subchunk_x < curr_w; ++subchunk_x) {
					const std::size_t idx = static_cast<std::size_t>(subchunk_y * curr_w + subchunk_x);
					if (skippable && (prev_buf[idx] == curr_buf[idx])) {
						continue;
					}
					display += term.set_cursor_code(subchunk_y, subchunk_x);
					display += term.fg_code(curr_buf[idx][0]);
					display += term.bg_code(curr_buf[idx][1]);
					display += "▀";
					display += term.reset_style_code();
				}
			}
			prev_buf = curr_buf;
			std::fputs(display.c_str(), term.out);
			std::fflush(term.out);
		}

		std::print(term.out, "\x1B[H\x1B[0m");
		std::print(term.out, "tick: {:0>8X}\r\n", tick++);
		std::print(term.out, "X:   {}{:0>8X}\r\nY:   {}{:0>8X}\r\nx:    {:0>8X}\r\ny:    {:0>8X}\r\n", " -"[player_chunk_x < 0], xieite::abs(player_chunk_x), " -"[player_chunk_y < 0], xieite::abs(player_chunk_y), player_subchunk_x, player_subchunk_y);
		std::print(term.out, "hold:       {:0>2X}\r\n", holding);
		std::print(term.out, "\r\nWASD to move\r\nSPACE to interact\r\nCTRL+Q to quit");
		std::fflush(term.out);
		std::this_thread::sleep_for(1000ms / 30);

		term.block(false);
		if (([&] -> bool {
			while (true) {
				switch (term.read_key()) {
				case xieite::keys::ctrl_q:
					{
						auto file = xieite::file("save.txt", "w");
						std::println(file.get(), "{:X} {:X} {:X} {:X} {:X} {:X}", tick, player_chunk_x, player_chunk_y, player_subchunk_x, player_subchunk_y, holding);
						for (auto&& [chunk_x, chunk_col] : world) {
							for (auto&& [chunk_y, chunk] : chunk_col) {
								if (([&] -> bool {
									for (auto&& tile_col : chunk) {
										for (auto&& tile : tile_col) {
											if (!sand::tiles[tile].transparent) {
												return false;
											}
										}
									}
									return true;
								})()) {
									continue;
								}
								std::println(file.get(), "{:X} {:X}", chunk_x, chunk_y);
								for (std::uint64_t subchunk_y = sand::chunk_h; subchunk_y--;) {
									for (std::uint64_t subchunk_x = 0; subchunk_x < (sand::chunk_w - 1); ++subchunk_x) {
										std::print(file.get(), "{:0>2X} ", world[chunk_x][chunk_y][subchunk_x][subchunk_y]);
									}
									std::println(file.get(), "{:0>2X}", world[chunk_x][chunk_y][sand::chunk_w - 1][subchunk_y]);
								}
							}
						}
					}
					return true;
				case xieite::keys::_0:
					world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 0;
					break;
				case xieite::keys::_1:
					world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 1;
					break;
				case xieite::keys::_2:
					world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 2;
					break;
				case xieite::keys::_3:
					world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 3;
					break;
				case xieite::keys::_4:
					world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 4;
					break;
				case xieite::keys::_5:
					world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 5;
					break;
				// case xieite::keys::_6:
				// 	world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 6;
				// 	break;
				// case xieite::keys::_7:
				// 	world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 7;
				// 	break;
				// case xieite::keys::_8:
				// 	world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 8;
				// 	break;
				// case xieite::keys::_9:
				// 	world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = 9;
				// 	break;
				case xieite::keys::right:
					move(1, 0);
					break;
				case xieite::keys::left:
					move(-1, 0);
					break;
				case xieite::keys::up:
					move(0, 1);
					break;
				case xieite::keys::down:
					move(0, -1);
					break;
				case xieite::keys::space:
					std::swap(holding, world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y]);
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
