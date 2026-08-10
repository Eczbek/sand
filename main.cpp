#include "color.hpp"
#include "get_color.hpp"
#include "font_data.hpp"
#include "pos.hpp"
#include "texture.hpp"
#include "texture_data.hpp"
#include "tile.hpp"

#include <xte/data/fixed_array.hpp>
#include <xte/data/is_whitespace.hpp>
#include <xte/data/string.hpp>
#include <xte/data/string_view.hpp>
#include <xte/io/file.hpp>
#include <xte/io/file_mode.hpp>
#include <xte/math/parse_number.hpp>
#include <xte/util/error.hpp>

#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <format>
#include <print>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>

using namespace std::literals;

namespace sand {
	struct pixel_pos {
		xte::u64 x;
		xte::u64 y;

		[[nodiscard]] friend constexpr bool operator==(const sand::pixel_pos&, const sand::pixel_pos&) = default;
	};

	xte::u64 tick = 0;
	sand::pos camera_pos = { 0, 0, 0, 0 };
	xte::u64 select = 0;

	std::unordered_map<xte::u64, std::unordered_map<xte::u64, xte::fixed_array<xte::fixed_array<sand::tile, sand::chunk_h>, sand::chunk_w>>> world;

	struct display_char {
		xte::fixed_array<sand::color3, 2> pixels;

		[[nodiscard]] friend bool operator==(const sand::display_char&, const sand::display_char&) = default;
	};

	sand::pixel_pos screen_size = { 0, 0 };
	xte::array<sand::display_char> screen;

	[[nodiscard]] constexpr sand::color4 texture_at(xte::u64 index, sand::pixel_pos pos) noexcept {
		return sand::get_color(sand::texture_data[sand::textures[index].frames[sand::tick % sand::textures[index].frames_count]][pos.y * sand::texture_w + pos.x]);
	}

	[[nodiscard]] constexpr bool font_at(char index, sand::pixel_pos pos) noexcept {
		return (pos.x < sand::font_w) && (pos.y < sand::font_h) && (sand::font_data[static_cast<xte::uz>(index)][pos.y * sand::font_w + pos.x] == '#');
	}

	[[nodiscard]] sand::color3& screen_at(sand::pixel_pos pos) noexcept {
		static sand::color3 dummy;
		return ((pos.x < sand::screen_size.x) && (pos.y < (sand::screen_size.y * 2)))
			? screen[pos.y / 2 * sand::screen_size.x + pos.x].pixels[!!(pos.y % 2)]
			: dummy;
	}

	constexpr void draw_texture(xte::u64 texture_index, sand::pos pos) noexcept {
		const xte::u64 offset_x = sand::screen_size.x / 2 - sand::texture_w / 2 + ((pos.chunk_x - sand::camera_pos.chunk_x) * sand::chunk_w + pos.tile_x - sand::camera_pos.tile_x) * sand::texture_w;
		const xte::u64 offset_y = sand::screen_size.y - sand::texture_h / 2 - ((pos.chunk_y - sand::camera_pos.chunk_y) * sand::chunk_h + pos.tile_y - sand::camera_pos.tile_y) * sand::texture_h;
		for (xte::u64 texture_x = 0; texture_x < sand::texture_w; ++texture_x) {
			for (xte::u64 texture_y = 0; texture_y < sand::texture_h; ++texture_y) {
				if (const auto [r, g, b, a] = sand::texture_at(texture_index, { texture_x, texture_y }); a) {
					sand::screen_at({ offset_x + texture_x, offset_y + texture_y }) = sand::color3(r, g, b);
				}
			}
		}
	}

	constexpr void write_text(xte::string_view text, const sand::color3& fg, const sand::color3& bg, sand::pixel_pos pos) noexcept {
		xte::u64 row = 0;
		xte::u64 col = 0;
		for (char c : text) {
			if (c == '\n') {
				++row;
				col = 0;
				continue;
			}
			for (xte::u64 x = 0; x < sand::font_w; ++x) {
				for (xte::u64 y = 0; y < sand::font_h; ++y) {
					if (sand::font_at(c, { x, y })) {
						const xte::u64 pixel_x = pos.x + col * sand::font_w + x;
						const xte::u64 pixel_y = pos.y + row * sand::font_h + y;
						screen_at({ pixel_x, pixel_y }) = fg;
						screen_at({ pixel_x, pixel_y + 1 }) = bg;
					}
				}
			}
			++col;
		}
	}

	static constexpr xte::string_view save_dir = "save";

	constexpr void log(xte::string_view message) noexcept {
		std::println("{}\r", message);
		std::fflush(stdout);
	}
}

int main() {
	const int terminal_blocking = ::fcntl(STDIN_FILENO, F_GETFL);
	const ::termios terminal_cooked = ([] -> ::termios {
		::termios terminal_cooked;
		::tcgetattr(STDIN_FILENO, &terminal_cooked);
		return terminal_cooked;
	})();
	{
		::termios terminal_raw = terminal_cooked;
		terminal_raw.c_iflag &= ~static_cast<::tcflag_t>(ICRNL | IXON);
		terminal_raw.c_lflag &= ~static_cast<::tcflag_t>(ECHO | ICANON | IEXTEN | ISIG);
		terminal_raw.c_oflag &= ~static_cast<::tcflag_t>(OPOST);
		::tcsetattr(STDIN_FILENO, TCSANOW, &terminal_raw);
	}
	std::print("\x1B[?47h\x1B[s\x1B[?25l\x1B[2J\x1B[3J\x1B[0m");

	if (std::filesystem::exists(std::format("{}/index.txt", sand::save_dir))) {
		const xte::string data = xte::file(std::format("{}/index.txt", sand::save_dir), xte::file_mode::read).read();
		xte::uz i = 0;

		auto parse = [&](xte::string_view data) -> xte::u64 {
			while (xte::is_whitespace(data[i])) {
				++i;
			}
			auto [number, length] = xte::parse_number<xte::u64>.with_index(data.subview(i), 16);
			if (!length) {
				sand::log("failed to parse save data");
				throw;
			}
			i += length;
			return number;
		};

		sand::tick = parse(data);
		sand::camera_pos = { parse(data), parse(data), parse(data), parse(data) };
		sand::select = parse(data);

		if (std::filesystem::exists(std::format("{}/chunks", sand::save_dir))) {
			for (const auto& chunk_file : std::filesystem::directory_iterator(std::format("{}/chunks", sand::save_dir))) {
				i = 0;
				const xte::u64 chunk_x = parse(xte::string_view(chunk_file.path().filename().c_str()));
				const xte::u64 chunk_y = parse(xte::string_view(chunk_file.path().filename().c_str()));
				i = 0;
				const xte::string data = xte::file(xte::string_view(chunk_file.path().c_str()), xte::file_mode::read).read();
				for (xte::u64 tile_y = sand::chunk_h; tile_y--;) {
					for (xte::u64 tile_x = 0; tile_x < sand::chunk_w; ++tile_x) {
						sand::world[chunk_x][chunk_y][tile_x][tile_y] = sand::tiles[parse(data)];
					}
				}
			}
		}
	}

	auto rng = std::mt19937(std::random_device()());

	sand::pixel_pos previous_screen_size = { 0, 0 };
	xte::array<sand::display_char> previous_screen;
	bool placed = false;
	for (;; ++sand::tick) {
		::winsize screen_size;
		::ioctl(::fileno(stdin), TIOCGWINSZ, &screen_size);
		sand::screen_size = { screen_size.ws_col, screen_size.ws_row };

		sand::screen.reset();
		sand::screen.resize(sand::screen_size.x * sand::screen_size.y);

		for (xte::u64 view_chunk_x = 0; view_chunk_x < 3; ++view_chunk_x) {
			for (xte::u64 view_chunk_y = 0; view_chunk_y < 3; ++view_chunk_y) {
				const xte::u64 chunk_x = sand::camera_pos.chunk_x + view_chunk_x - 1;
				const xte::u64 chunk_y = sand::camera_pos.chunk_y + view_chunk_y - 1;
				if (!sand::world.contains(chunk_x) || !sand::world[chunk_x].contains(chunk_y)) {
					for (xte::u64 tile_x = 0; tile_x < sand::chunk_w; ++tile_x) {
						for (xte::u64 tile_y = 0; tile_y < sand::chunk_h; ++tile_y) {
							sand::world[chunk_x][chunk_y][tile_x][tile_y] = sand::tiles[std::uniform_int_distribution<xte::u64>(0, sand::tiles.size() - 1)(rng)];
						}
					}
				}
				for (xte::u64 tile_x = 0; tile_x < sand::chunk_w; ++tile_x) {
					for (xte::u64 tile_y = 0; tile_y < sand::chunk_h; ++tile_y) {
						sand::draw_texture(sand::world[chunk_x][chunk_y][tile_x][tile_y].texture_index, { chunk_x, chunk_y, tile_x, tile_y });
					}
				}
			}
		}

		if (sand::select && !placed) {
			sand::draw_texture(sand::tiles[sand::select].texture_index, sand::camera_pos);
			sand::draw_texture(0x0E, sand::camera_pos - sand::pos(0, 0, 1, 0) + sand::pos(0, 0, 0, 1)); // top left corner
			sand::draw_texture(0x0F, sand::camera_pos + sand::pos(0, 0, 0, 1)); // top left horizontal
			sand::draw_texture(0x10, sand::camera_pos - sand::pos(0, 0, 1, 0)); // top left vertical
			sand::draw_texture(0x11, sand::camera_pos + sand::pos(0, 0, 1, 1)); // top right corner
			sand::draw_texture(0x12, sand::camera_pos + sand::pos(0, 0, 0, 1)); // top right horizontal
			sand::draw_texture(0x13, sand::camera_pos + sand::pos(0, 0, 1, 0)); // top right vertical
			sand::draw_texture(0x14, sand::camera_pos - sand::pos(0, 0, 1, 1)); // bottom left corner
			sand::draw_texture(0x15, sand::camera_pos - sand::pos(0, 0, 0, 1)); // bottom left horizontal
			sand::draw_texture(0x16, sand::camera_pos - sand::pos(0, 0, 1, 0)); // bottom left vertical
			sand::draw_texture(0x17, sand::camera_pos + sand::pos(0, 0, 1, 0) - sand::pos(0, 0, 0, 1)); // bottom right corner
			sand::draw_texture(0x18, sand::camera_pos - sand::pos(0, 0, 0, 1)); // bottom right horizontal
			sand::draw_texture(0x19, sand::camera_pos + sand::pos(0, 0, 1, 0)); // bottom right vertical
		} else {
			sand::draw_texture(0x0E, sand::camera_pos); // top left corner
			sand::draw_texture(0x0F, sand::camera_pos + sand::pos(0, 0, 0, 1)); // top left horizontal
			sand::draw_texture(0x10, sand::camera_pos - sand::pos(0, 0, 1, 0)); // top left vertical
			sand::draw_texture(0x11, sand::camera_pos); // top right corner
			sand::draw_texture(0x12, sand::camera_pos + sand::pos(0, 0, 0, 1)); // top right horizontal
			sand::draw_texture(0x13, sand::camera_pos + sand::pos(0, 0, 1, 0)); // top right vertical
			sand::draw_texture(0x14, sand::camera_pos); //bottom left corner
			sand::draw_texture(0x15, sand::camera_pos - sand::pos(0, 0, 0, 1)); // bottom left horizontal
			sand::draw_texture(0x16, sand::camera_pos - sand::pos(0, 0, 1, 0)); // bottom left vertical
			sand::draw_texture(0x17, sand::camera_pos); //bottom right corner
			sand::draw_texture(0x18, sand::camera_pos - sand::pos(0, 0, 0, 1)); // bottom right horizontal
			sand::draw_texture(0x19, sand::camera_pos + sand::pos(0, 0, 1, 0)); // bottom right vertical
		}

		sand::write_text(std::format(
			"tick: {:X}\n"
			"X:    {:X}\n"
			"Y:    {:X}\n"
			"x:    {:X}\n"
			"y:    {:X}\n"
			"hold: {:X}",
			sand::tick,
			static_cast<xte::i64>(sand::camera_pos.chunk_x),
			static_cast<xte::i64>(sand::camera_pos.chunk_y),
			sand::camera_pos.tile_x,
			sand::camera_pos.tile_y,
			sand::select
		), 0xFFFFFF, 0x000000, { 0, 0 });

		std::string display;
		if (sand::screen != previous_screen) {
			const bool skippable = sand::screen_size == previous_screen_size;
			previous_screen.resize(sand::screen.size());
			for (xte::u64 pixel_y = 0; pixel_y < sand::screen_size.y; ++pixel_y) {
				for (xte::u64 pixel_x = 0; pixel_x < sand::screen_size.x; ++pixel_x) {
					const xte::u64 pixel_index = pixel_y * sand::screen_size.x + pixel_x;
					if (skippable && (sand::screen[pixel_index] == previous_screen[pixel_index])) {
						continue;
					}
					std::format_to(
						std::back_inserter(display),
						"\x1B[{};{}H\x1B[38;2;{};{};{}m\x1B[48;2;{};{};{}m▀\x1B[0m",
						pixel_y + 1,
						pixel_x + 1,
						sand::screen[pixel_index].pixels[0].r,
						sand::screen[pixel_index].pixels[0].g,
						sand::screen[pixel_index].pixels[0].b,
						sand::screen[pixel_index].pixels[1].r,
						sand::screen[pixel_index].pixels[1].g,
						sand::screen[pixel_index].pixels[1].b
					);
				}
			}
			previous_screen = sand::screen;
		}

		std::print("{}", display);
		std::fflush(stdout);

		std::this_thread::sleep_for(1000ms / 20);

		placed = false;
		::fcntl(STDIN_FILENO, F_SETFL, terminal_blocking | O_NONBLOCK);
		if (([&] -> bool {
			while (true) {
				switch (std::fgetc(stdin)) {
				case '%':
					return true;
				case '[':
					sand::select = (sand::select - 1 + sand::tiles.size()) % sand::tiles.size();
					break;
				case ']':
					++sand::select %= sand::tiles.size();
					break;
				case '\r':
					sand::world[sand::camera_pos.chunk_x][sand::camera_pos.chunk_y][sand::camera_pos.tile_x][sand::camera_pos.tile_y] = sand::tiles[sand::select];
					placed = true;
					break;
				case 'D':
				case 'd':
					sand::camera_pos.chunk_x += !(++sand::camera_pos.tile_x %= sand::chunk_w);
					break;
				case 'A':
				case 'a':
					sand::camera_pos.chunk_x -= (sand::camera_pos.tile_x = (sand::camera_pos.tile_x - 1 + sand::chunk_w) % sand::chunk_w) == (sand::chunk_w - 1);
					break;
				case 'W':
				case 'w':
					sand::camera_pos.chunk_y += !(++sand::camera_pos.tile_y %= sand::chunk_h);
					break;
				case 'S':
				case 's':
					sand::camera_pos.chunk_y -= (sand::camera_pos.tile_y = (sand::camera_pos.tile_y - 1 + sand::chunk_h) % sand::chunk_h) == (sand::chunk_h - 1);
					break;
				default:
					while (std::fgetc(stdin) > 0);
					return false;
				}
			}
		})()) {
			break;
		}
		::fcntl(STDIN_FILENO, F_SETFL, terminal_blocking);
	}

	std::print("\x1B[0m\x1B[?25h\x1B[u\x1B[?47l");
	::tcsetattr(STDIN_FILENO, TCSANOW, &terminal_cooked);
	::fcntl(STDIN_FILENO, F_SETFL, terminal_blocking);

	{
		std::filesystem::create_directory(std::format("{}", sand::save_dir));
		auto index_file = xte::file(std::format("{}/index.txt", sand::save_dir), xte::file_mode::overwrite);
		if (!index_file) {
			sand::log("failed to write index");
			throw;
		}
		std::println(
			index_file,
			"{:X} {:X} {:X} {:X} {:X} {:X}",
			sand::tick,
			sand::camera_pos.chunk_x,
			sand::camera_pos.chunk_y,
			sand::camera_pos.tile_x,
			sand::camera_pos.tile_y,
			sand::select
		);
		for (auto&& [chunk_x, chunks_column] : sand::world) {
			for (auto&& [chunk_y, chunk] : chunks_column) {
				if (([&] -> bool {
					for (auto&& tiles_column : chunk) {
						for (auto&& tile : tiles_column) {
							if (tile != sand::tiles[0]) {
								return false;
							}
						}
					}
					return true;
				})()) {
					continue;
				}
				std::filesystem::create_directory(std::format("{}/chunks", sand::save_dir));
				auto chunk_file = xte::file(std::format("{}/chunks/{:0>16X} {:0>16X}.txt", sand::save_dir, chunk_x, chunk_y), xte::file_mode::overwrite);
				if (!chunk_file) {
					sand::log(std::format("failed to write chunk {} {}", static_cast<xte::i64>(chunk_x), static_cast<xte::i64>(chunk_y)));
					throw;
				}
				for (xte::u64 tile_y = sand::chunk_h; tile_y--;) {
					for (xte::u64 tile_x = 0; tile_x < (sand::chunk_w - 1); ++tile_x) {
						std::print(chunk_file, "{:0>2X} ", sand::world[chunk_x][chunk_y][tile_x][tile_y].texture_index);
					}
					std::println(chunk_file, "{:0>2X}", sand::world[chunk_x][chunk_y][sand::chunk_w - 1][tile_y].texture_index);
				}
			}
		}
	}
}
