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
#include <xieite/io/keys_char.hpp>
#include <xieite/io/term.hpp>
#include <xieite/math/abs.hpp>
#include <xieite/math/color3.hpp>
#include <xieite/math/color4.hpp>

using namespace std::literals;

namespace sand {
	constexpr std::uint64_t texture_w = 8;
	constexpr std::uint64_t texture_h = 8;
	constexpr std::array<std::string_view, 0x0E> textures = {
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
		"aa    aa" // 03 cursor
		"a      a"
		"        "
		"        "
		"        "
		"        "
		"a      a"
		"aa    aa"
		,
		" a    a " // 04 cursor (holding)
		"aa    aa"
		"        "
		"        "
		"        "
		"        "
		"aa    aa"
		" a    a "
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
		"asdfbk<;" // 07 rainbow
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
	};

	struct tile {
		std::size_t texture;
		bool transparent;
	};
	constexpr std::array<sand::tile, 0x0C> tiles = {{
		/* 00 void */ { 0x00, true },
		/* 01 stone */ { 0x01, false },
		/* 02 cobble */ { 0x02, false },
		/* 03 dblue */ { 0x05, true },
		/* 04 lblue */ { 0x06, true },
		/* 05 rainbow */ { 0x07, false },
		/* 06 dirt */ { 0x08, false },
		/* 07 rock */ { 0x09, false },
		/* 08 leaves */ { 0x0A, false },
		/* 09 wood */ { 0x0B, false },
		/* 0A grass */ { 0x0C, false },
		/* 0B flowers */ { 0x0D, false }
	}};

	constexpr std::uint64_t chunk_w = 16;
	constexpr std::uint64_t chunk_h = 16;
	constexpr std::int64_t view_dist = 20;

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
	xieite::term term;
	term.echo(false);
	term.canon(false);
	term.signal(false);
	term.proc(false);
	term.cursor_alt(true);
	term.screen_alt(true);
	term.cursor_invis(true);
	term.clear_screen();

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
		// if (!sand::tiles[holding].transparent && !sand::tiles[world[chunk_x][chunk_y][subchunk_x][subchunk_y]].transparent) {
		// 	return;
		// }
		player_chunk_x = chunk_x;
		player_chunk_y = chunk_y;
		player_subchunk_x = subchunk_x;
		player_subchunk_y = subchunk_y;
	};

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
						case ' ': return 0x00000000;
						case '0': return 0x000000FF;
						case '1': return 0x3F3B37FF;//0x373737FF;
						case '2': return 0x5B5753FF;//0x4C4C4CFF;
						case '3': return 0x776F67FF;//0x707070FF;
						case '4': return 0xA3978FFF;//0xAFA79FFF;//0x8D8D8DFF;
						case '5': return 0xCFC7BFFF;//0xC0C0C0FF;
						case 'q': return 0xCFB777FF;
						case 'w': return 0xA78F57FF;
						case 'e': return 0x7F6737FF;
						case 'r': return 0x574727FF;
						case 'Q': return 0x775737FF;
						case 'W': return 0x573F2FFF;
						case 'E': return 0x37271FFF;
						case 'R': return 0x271F17FF;
						case 'T': return 0x1B130FFF;
						case 'a': return 0xFF0000FF;
						case 'z': return 0xBF0000FF;
						case 'A': return 0x7F0000FF;
						case 'Z': return 0x3F0000FF;
						case 's': return 0xFF7F00FF;
						case 'x': return 0xCF5F00FF;
						case 'd': return 0xFFBF00FF;
						case 'c': return 0xCF7F00FF;
						case 'f': return 0xFFFF00FF;
						case 'v': return 0xCF9F1FFF;
						case 'g': return 0x7FFF00FF;
						case 'b': return 0x00F700FF;
						case 'G': return 0x00BF00FF;
						case 'B': return 0x007F00FF;
						case 'h': return 0x00BF7FFF;
						case 'n': return 0x007F5FFF;
						case 'H': return 0x076F35FF;
						case 'N': return 0x075727FF;
						case 'j': return 0x00FFFFFF;
						case 'm': return 0x00DFFFFF;
						case 'J': return 0x00BFFFFF;
						case 'M': return 0x009FFFFF;
						case 'k': return 0x007FFFFF;
						case ',': return 0x005FFFFF;
						case 'K': return 0x003FFFFF;
						case '<': return 0x0000FFFF;
						case 'l': return 0x00007FFF;
						case '.': return 0x00003FFF;
						case ';': return 0x7F00FFFF;
						case ':': return 0xFF00FFFF;
						case '?': return 0xFF007FFF;
						}
						return 0xFF00FFFF;
					})(); a) {
						curr_buf[static_cast<std::size_t>(idx)][!!(offset_y % 2) != !!(subchunk_y % 2)] = xieite::color3(r, g, b);
					}
				}
			}
		};

		for (std::int64_t chunk_x = player_chunk_x - sand::view_dist; chunk_x <= (player_chunk_x + sand::view_dist); ++chunk_x) {
			for (std::int64_t chunk_y = player_chunk_y - sand::view_dist; chunk_y <= (player_chunk_y + sand::view_dist); ++chunk_y) {
				// if (!world[chunk_x].contains(chunk_y)) {
				// 	for (std::size_t subchunk_x = 0; subchunk_x < sand::chunk_w; ++subchunk_x) {
				// 		for (std::size_t subchunk_y = 0; subchunk_y < sand::chunk_h; ++subchunk_y) {
				// 			if (chunk_y > 0xFFF) {
				// 				world[chunk_x][chunk_y][subchunk_x][subchunk_y] = 0x00;
				// 			} else if (chunk_y > -1) {
				// 				world[chunk_x][chunk_y][subchunk_x][subchunk_y] = 0x04;
				// 			} else if (chunk_y == -1) {
				// 				if (subchunk_y == (sand::chunk_h - 1)) {
				// 					world[chunk_x][chunk_y][subchunk_x][subchunk_y] = 0x0A;
				// 				} else {
				// 					world[chunk_x][chunk_y][subchunk_x][subchunk_y] = 0x06;
				// 				}
				// 			} else if (chunk_y > -0xFFF) {
				// 				world[chunk_x][chunk_y][subchunk_x][subchunk_y] = 0x07;
				// 			} else {
				// 				world[chunk_x][chunk_y][subchunk_x][subchunk_y] = 0x05;
				// 			}
				// 		}
				// 	}
				// }
				for (std::uint64_t subchunk_x = 0; subchunk_x < sand::chunk_w; ++subchunk_x) {
					for (std::uint64_t subchunk_y = 0; subchunk_y < sand::chunk_h; ++subchunk_y) {
						draw(sand::tiles[world[chunk_x][chunk_y][subchunk_x][subchunk_y]].texture, chunk_x, chunk_y, subchunk_x, subchunk_y);
					}
				}
			}
		}
		// if (!sand::tiles[holding].transparent) {
			draw(sand::tiles[holding].texture, player_chunk_x, player_chunk_y, player_subchunk_x, player_subchunk_y);
			draw(0x03 /*0x04*/, player_chunk_x, player_chunk_y, player_subchunk_x, player_subchunk_y);
		// } else {
		// 	draw(0x03, player_chunk_x, player_chunk_y, player_subchunk_x, player_subchunk_y);
		// }

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
		std::print(term.out, "\r\nWASD to move\r\n<> to cycle\r\nSPACE to place\r\nCTRL+Q to quit");
		std::fflush(term.out);
		std::this_thread::sleep_for(1000ms / 30);

		term.block(false);
		if (([&] -> bool {
			while (true) {
				switch (/*const xieite::keys key =*/ term.read_key()) {
				case xieite::keys::ctrl_q:
					return true;
				case xieite::keys::comma:
					holding = (holding - 1 + sand::tiles.size()) % sand::tiles.size();
					break;
				case xieite::keys::dot:
					holding = (holding + 1) % sand::tiles.size();
					break;
				case xieite::keys::d:
				case xieite::keys::D:
				case xieite::keys::right:
					move(1, 0);
					break;
				case xieite::keys::a:
				case xieite::keys::A:
				case xieite::keys::left:
					move(-1, 0);
					break;
				case xieite::keys::w:
				case xieite::keys::W:
				case xieite::keys::up:
					move(0, 1);
					break;
				case xieite::keys::s:
				case xieite::keys::S:
				case xieite::keys::down:
					move(0, -1);
					break;
				case xieite::keys::space:
					// std::swap(holding, world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y]);
					world[player_chunk_x][player_chunk_y][player_subchunk_x][player_subchunk_y] = holding;
					break;
				default:
					// if (const char digit = xieite::keys_char(key); (digit >= '0') && (digit <= '9')) {
						
					// }
					while (std::fgetc(term.in) > 0);
					return false;
				}
			}
		})()) {
			break;
		}
		term.block(true);
	}

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
