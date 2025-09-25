#include <array>
#include <chrono> // IWYU pragma: keep
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <print>
#include <random>
#include <stdio.h>
#include <string>
#include <string_view>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <xieite/io/file.hpp>
#include <xieite/io/read.hpp>
#include <xieite/math/color3.hpp>
#include <xieite/math/color4.hpp>
#include <xieite/math/parse_number.hpp>
#include "get_color.hpp"
#include "font_data.hpp"
#include "texture.hpp"
#include "texture_data.hpp"
#include "tile.hpp"

namespace sand {
	struct pixel_pos {
		std::uint64_t x;
		std::uint64_t y;

		[[nodiscard]] friend constexpr bool operator==(const sand::pixel_pos&, const sand::pixel_pos&) = default;
	};

	struct world_subpos {
		std::uint64_t x;
		std::uint64_t y;

		[[nodiscard]] friend constexpr bool operator==(const sand::world_subpos&, const sand::world_subpos&) = default;
	};

	struct world_pos {
		sand::world_subpos chunk;
		sand::world_subpos tile;

		[[nodiscard]] friend constexpr bool operator==(const sand::world_pos&, const sand::world_pos&) = default;
	};

	constexpr sand::world_subpos chunk_size = { 32, 32 };

	std::uint64_t tick = 0;
	sand::world_pos camera_pos = { { 0, 0 }, { 0, 0 } };
	std::uint64_t select = 0;

	std::unordered_map<std::uint64_t, std::unordered_map<std::uint64_t, std::array<std::array<sand::tile, sand::chunk_size.y>, sand::chunk_size.x>>> world;

	struct display_char {
		std::array<xieite::color3, 2> pixels;

		[[nodiscard]] friend bool operator==(const sand::display_char&, const sand::display_char&) = default;
	};

	sand::pixel_pos screen_size = { 0, 0 };
	std::vector<sand::display_char> screen;

	[[nodiscard]] constexpr xieite::color4 texture_at(std::uint64_t index, sand::pixel_pos pos) noexcept {
		return sand::get_color(sand::texture_data[sand::textures[index].frame_indices[sand::tick % sand::textures[index].frames_count]][pos.y * sand::texture_width + pos.x]);
	}

	[[nodiscard]] constexpr bool font_at(char index, sand::pixel_pos pos) noexcept {
		return (pos.x < sand::font_width) && (pos.y < sand::font_height) && (sand::font_data[static_cast<std::size_t>(index)][pos.y * sand::font_width + pos.x] == '#');
	}

	[[nodiscard]] xieite::color3& screen_at(sand::pixel_pos pos) noexcept {
		static xieite::color3 dummy;
		return ((pos.x < sand::screen_size.x) && (pos.y < (sand::screen_size.y * 2)))
			? screen[pos.y / 2 * sand::screen_size.x + pos.x].pixels[!!(pos.y % 2)]
			: dummy;
	}

	constexpr void draw_texture(std::uint64_t texture_index, sand::world_pos pos) noexcept {
		const std::uint64_t offset_x = sand::screen_size.x / 2 - sand::texture_width / 2 + ((pos.chunk.x - sand::camera_pos.chunk.x) * sand::chunk_size.x + pos.tile.x - sand::camera_pos.tile.x) * sand::texture_width;
		const std::uint64_t offset_y = sand::screen_size.y - sand::texture_height / 2 - ((pos.chunk.y - sand::camera_pos.chunk.y) * sand::chunk_size.y + pos.tile.y - sand::camera_pos.tile.y) * sand::texture_height;
		for (std::uint64_t texture_x = 0; texture_x < sand::texture_width; ++texture_x) {
			for (std::uint64_t texture_y = 0; texture_y < sand::texture_height; ++texture_y) {
				if (const auto [r, g, b, a] = sand::texture_at(texture_index, { texture_x, texture_y }); a) {
					sand::screen_at({ offset_x + texture_x, offset_y + texture_y }) = xieite::color3(r, g, b);
				}
			}
		}
	}

	constexpr void write_text(std::string_view text, const xieite::color3& fg, const xieite::color3& bg, sand::pixel_pos pos) noexcept {
		std::uint64_t row = 0;
		std::uint64_t col = 0;
		for (char c : text) {
			if (c == '\n') {
				++row;
				col = 0;
				continue;
			}
			for (std::uint64_t x = 0; x < sand::font_width; ++x) {
				for (std::uint64_t y = 0; y < sand::font_height; ++y) {
					if (sand::font_at(c, { x, y })) {
						const std::uint64_t pixel_x = pos.x + col * sand::font_width + x;
						const std::uint64_t pixel_y = pos.y + row * sand::font_height + y;
						screen_at({ pixel_x, pixel_y }) = fg;
						screen_at({ pixel_x + 1, pixel_y + 1 }) = bg;
					}
				}
			}
			++col;
		}
	}
}

int main() {
	using namespace std::literals;

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
	std::print("\x1B[?47h\x1B[s\x1B[?25l\x1B[2J\x1B[0m");

	if (std::filesystem::exists("save/index.txt")) {
		const std::string data = xieite::read(xieite::file("save/index.txt", "r").get());
		std::size_t i = 0;

		sand::tick = xieite::parse_number<std::uint64_t>(data, i, 16);
		sand::camera_pos = {
			{ xieite::parse_number<std::uint64_t>(data, i, 16), xieite::parse_number<std::uint64_t>(data, i, 16) },
			{ xieite::parse_number<std::uint64_t>(data, i, 16), xieite::parse_number<std::uint64_t>(data, i, 16) }
		};
		sand::select = xieite::parse_number<std::uint64_t>(data, i, 16);

		if (std::filesystem::exists("save/chunks")) {
			for (const auto& chunk_file : std::filesystem::directory_iterator("save/chunks")) {
				i = 0;
				const sand::world_subpos chunk = {
					xieite::parse_number<std::uint64_t>(chunk_file.path().c_str(), i, 16),
					xieite::parse_number<std::uint64_t>(chunk_file.path().c_str(), i, 16)
				};
				std::println("CHUNK {:X} {:X}", chunk.x, chunk.y);
				i = 0;
				const std::string data = xieite::read(xieite::file(chunk_file.path(), "r").get());
				for (std::uint64_t tile_y = sand::chunk_size.y; tile_y--;) {
					for (std::uint64_t tile_x = 0; tile_x < sand::chunk_size.x; ++tile_x) {
						sand::world[chunk.x][chunk.y][tile_x][tile_y] = sand::tiles[xieite::parse_number<std::uint64_t>(data, i, 16)];
					}
				}
			}
		}
	}

	auto rng = std::mt19937(std::random_device()());

	sand::pixel_pos previous_screen_size = { 0, 0 };
	std::vector<sand::display_char> previous_screen;
	for (;; ++sand::tick) {
		::winsize screen_size;
		::ioctl(::fileno(stdin), TIOCGWINSZ, &screen_size);
		sand::screen_size = { screen_size.ws_col, screen_size.ws_row };

		sand::screen.clear();
		sand::screen.resize(sand::screen_size.x * sand::screen_size.y);

		for (std::uint64_t view_chunk_x = 0; view_chunk_x < 3; ++view_chunk_x) {
			for (std::uint64_t view_chunk_y = 0; view_chunk_y < 3; ++view_chunk_y) {
				const sand::world_subpos chunk = {
					sand::camera_pos.chunk.x + view_chunk_x - 1,
					sand::camera_pos.chunk.y + view_chunk_y - 1
				};
				if (!sand::world.contains(chunk.x) || !sand::world[chunk.x].contains(chunk.y)) {
					for (std::uint64_t tile_x = 0; tile_x < sand::chunk_size.x; ++tile_x) {
						for (std::uint64_t tile_y = 0; tile_y < sand::chunk_size.y; ++tile_y) {
							sand::world[chunk.x][chunk.y][tile_x][tile_y] = sand::tiles[std::uniform_int_distribution<std::uint64_t>(0x03, 0x04)(rng)];
						}
					}
					for (std::uint64_t tile_x = 0; tile_x < sand::chunk_size.x; ++tile_x) {
						for (std::uint64_t tile_y = 0; tile_y < sand::chunk_size.y; ++tile_y) {
							if (!std::uniform_int_distribution<std::uint64_t>(0, 99)(rng)
								|| (!tile_x
									&& sand::world.contains(chunk.x - 1)
									&& sand::world[chunk.x - 1].contains(chunk.y)
									&& ((sand::world[chunk.x - 1][chunk.y][sand::chunk_size.x - 1][tile_y] == sand::tiles[0x0C])
										|| (sand::world[chunk.x - 1][chunk.y][sand::chunk_size.x - 1][tile_y] == sand::tiles[0x0D])))
								|| (!tile_y
									&& sand::world.contains(chunk.x)
									&& sand::world[chunk.x].contains(chunk.y - 1)
									&& ((sand::world[chunk.x][chunk.y - 1][tile_x][sand::chunk_size.y - 1] == sand::tiles[0x0C])
										|| (sand::world[chunk.x][chunk.y - 1][tile_x][sand::chunk_size.y - 1] == sand::tiles[0x0D])))
								|| ((tile_x == (sand::chunk_size.x - 1))
									&& sand::world.contains(chunk.x + 1)
									&& sand::world[chunk.x + 1].contains(chunk.y)
									&& ((sand::world[chunk.x + 1][chunk.y][0][tile_y] == sand::tiles[0x0C])
										|| (sand::world[chunk.x + 1][chunk.y][0][tile_y] == sand::tiles[0x0D])))
								|| ((tile_y == (sand::chunk_size.y - 1))
									&& sand::world.contains(chunk.x)
									&& sand::world[chunk.x].contains(chunk.y + 1)
									&& ((sand::world[chunk.x][chunk.y + 1][tile_x][0] == sand::tiles[0x0C])
										|| (sand::world[chunk.x][chunk.y + 1][tile_x][0] == sand::tiles[0x0D])))) {
								sand::world[chunk.x][chunk.y][tile_x][tile_y] = sand::tiles[std::uniform_int_distribution<std::uint64_t>(0x0C, 0x0D)(rng)];
								([&](this auto self, sand::world_pos pos) -> void {
									sand::world_subpos offset = {
										std::uniform_int_distribution<std::uint64_t>(0, 2)(rng),
										std::uniform_int_distribution<std::uint64_t>(0, 2)(rng)
									};
									sand::world_pos next = {
										{ pos.chunk.x, pos.chunk.y },
										{
											!offset.x ? (pos.tile.x - !!pos.tile.x) : (pos.tile.x + ((offset.x == 2) && (pos.tile.x < (sand::chunk_size.x - 1)))),
											!offset.y ? (pos.tile.y - !!pos.tile.y) : (pos.tile.y + ((offset.y == 2) && (pos.tile.y < (sand::chunk_size.y - 1))))
										}
									};
									const bool recurse = (sand::world[next.chunk.x][next.chunk.y][next.tile.x][next.tile.y] == sand::tiles[0x0C]) || (sand::world[next.chunk.x][next.chunk.y][next.tile.x][next.tile.y] == sand::tiles[0x0D]);
									sand::world[next.chunk.x][next.chunk.y][next.tile.x][next.tile.y] = sand::tiles[std::uniform_int_distribution<std::uint64_t>(0x0C, 0x0D)(rng)];
									if (recurse || std::uniform_int_distribution<std::uint64_t>(0, 9)(rng)) {
										self(next);
									}
								})({ chunk, { tile_x, tile_y } });
							}
						}
					}
				}
				for (std::uint64_t tile_x = 0; tile_x < sand::chunk_size.x; ++tile_x) {
					for (std::uint64_t tile_y = 0; tile_y < sand::chunk_size.y; ++tile_y) {
						sand::draw_texture(sand::world[chunk.x][chunk.y][tile_x][tile_y].texture_index, { chunk, { tile_x, tile_y } });
					}
				}
			}
		}

		sand::draw_texture(sand::tiles[sand::select].texture_index, sand::camera_pos);
		sand::draw_texture(0x0E, { // top left corner
			{
				sand::camera_pos.chunk.x - !sand::camera_pos.tile.x,
				sand::camera_pos.chunk.y + (sand::camera_pos.tile.y == (sand::chunk_size.y - 1))
			},
			{
				(sand::camera_pos.tile.x - 1 + sand::chunk_size.x) % sand::chunk_size.x,
				(sand::camera_pos.tile.y + 1) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x0F, { // top left horizontal
			{
				sand::camera_pos.chunk.x,
				sand::camera_pos.chunk.y + (sand::camera_pos.tile.y == (sand::chunk_size.y - 1))
			},
			{
				sand::camera_pos.tile.x,
				(sand::camera_pos.tile.y + 1) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x10, { // top left vertical
			{
				sand::camera_pos.chunk.x - !sand::camera_pos.tile.x,
				sand::camera_pos.chunk.y
			},
			{
				(sand::camera_pos.tile.x - 1 + sand::chunk_size.x) % sand::chunk_size.x,
				sand::camera_pos.tile.y
			}
		});
		sand::draw_texture(0x11, { // top right corner
			{
				sand::camera_pos.chunk.x + (sand::camera_pos.tile.x == (sand::chunk_size.x - 1)),
				sand::camera_pos.chunk.y + (sand::camera_pos.tile.y == (sand::chunk_size.y - 1))
			},
			{
				(sand::camera_pos.tile.x + 1) % sand::chunk_size.x,
				(sand::camera_pos.tile.y + 1) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x12, { // top right horizontal
			{
				sand::camera_pos.chunk.x,
				sand::camera_pos.chunk.y + (sand::camera_pos.tile.y == (sand::chunk_size.y - 1))
			},
			{
				sand::camera_pos.tile.x,
				(sand::camera_pos.tile.y + 1) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x13, { // top right vertical
			{
				sand::camera_pos.chunk.x + (sand::camera_pos.tile.x == (sand::chunk_size.x - 1)),
				sand::camera_pos.chunk.y
			},
			{
				(sand::camera_pos.tile.x + 1) % sand::chunk_size.x,
				sand::camera_pos.tile.y
			}
		});
		sand::draw_texture(0x14, { // bottom left corner
			{
				sand::camera_pos.chunk.x - !sand::camera_pos.tile.x,
				sand::camera_pos.chunk.y - !sand::camera_pos.tile.y
			},
			{
				(sand::camera_pos.tile.x - 1 + sand::chunk_size.x) % sand::chunk_size.x,
				(sand::camera_pos.tile.y - 1 + sand::chunk_size.y) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x15, { // bottom left horizontal
			{
				sand::camera_pos.chunk.x,
				sand::camera_pos.chunk.y - !sand::camera_pos.tile.y
			},
			{
				sand::camera_pos.tile.x,
				(sand::camera_pos.tile.y - 1 + sand::chunk_size.y) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x16, { // bottom left vertical
			{
				sand::camera_pos.chunk.x - !sand::camera_pos.tile.x,
				sand::camera_pos.chunk.y
			},
			{
				(sand::camera_pos.tile.x - 1 + sand::chunk_size.x) % sand::chunk_size.x,
				sand::camera_pos.tile.y
			}
		});
		sand::draw_texture(0x17, { // bottom right corner
			{
				sand::camera_pos.chunk.x + (sand::camera_pos.tile.x == (sand::chunk_size.x - 1)),
				sand::camera_pos.chunk.y - !sand::camera_pos.tile.y
			},
			{
				(sand::camera_pos.tile.x + 1) % sand::chunk_size.x,
				(sand::camera_pos.tile.y - 1 + sand::chunk_size.y) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x18, { // bottom right horizontal
			{
				sand::camera_pos.chunk.x,
				sand::camera_pos.chunk.y - !sand::camera_pos.tile.y
			},
			{
				sand::camera_pos.tile.x,
				(sand::camera_pos.tile.y - 1 + sand::chunk_size.y) % sand::chunk_size.y
			}
		});
		sand::draw_texture(0x19, { // bottom right vertical
			{
				sand::camera_pos.chunk.x + (sand::camera_pos.tile.x == (sand::chunk_size.x - 1)),
				sand::camera_pos.chunk.y
			},
			{
				(sand::camera_pos.tile.x + 1) % sand::chunk_size.x,
				sand::camera_pos.tile.y
			}
		});

		sand::write_text(std::format(
			"tick: {:X}\n"
			"X:    {:X}\n"
			"Y:    {:X}\n"
			"x:    {:X}\n"
			"y:    {:X}\n"
			"hold: {:X}",
			sand::tick,
			static_cast<std::int64_t>(sand::camera_pos.chunk.x),
			static_cast<std::int64_t>(sand::camera_pos.chunk.y),
			sand::camera_pos.tile.x,
			sand::camera_pos.tile.y,
			sand::select
		), 0xFFFFFF, 0x000000, { 0, 0 });

		std::string display;
		if (sand::screen != previous_screen) {
			const bool skippable = sand::screen_size == previous_screen_size;
			previous_screen.resize(sand::screen.size());
			for (std::uint64_t pixel_y = 0; pixel_y < sand::screen_size.y; ++pixel_y) {
				for (std::uint64_t pixel_x = 0; pixel_x < sand::screen_size.x; ++pixel_x) {
					const std::uint64_t pixel_index = pixel_y * sand::screen_size.x + pixel_x;
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
				case ' ':
					sand::world[sand::camera_pos.chunk.x][sand::camera_pos.chunk.y][sand::camera_pos.tile.x][sand::camera_pos.tile.y] = sand::tiles[sand::select];
					break;
				case 'D':
				case 'd':
					sand::camera_pos.chunk.x += !(++sand::camera_pos.tile.x %= sand::chunk_size.x);
					break;
				case 'A':
				case 'a':
					sand::camera_pos.chunk.x -= (sand::camera_pos.tile.x = (sand::camera_pos.tile.x - 1 + sand::chunk_size.x) % sand::chunk_size.x) == (sand::chunk_size.x - 1);
					break;
				case 'W':
				case 'w':
					sand::camera_pos.chunk.y += !(++sand::camera_pos.tile.y %= sand::chunk_size.y);
					break;
				case 'S':
				case 's':
					sand::camera_pos.chunk.y -= (sand::camera_pos.tile.y = (sand::camera_pos.tile.y - 1 + sand::chunk_size.y) % sand::chunk_size.y) == (sand::chunk_size.y - 1);
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
		std::filesystem::create_directory("save");
		auto index_file = xieite::file("save/index.txt", "w");
		std::println(
			index_file.get(),
			"{:X} {:X} {:X} {:X} {:X} {:X}",
			sand::tick,
			sand::camera_pos.chunk.x,
			sand::camera_pos.chunk.y,
			sand::camera_pos.tile.x,
			sand::camera_pos.tile.y,
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
				std::filesystem::create_directory("save/chunks");
				auto chunk_file = xieite::file(std::format("save/chunks/{:0>16X} {:0>16X}.txt", chunk_x, chunk_y), "w");
				// std::println(chunk_file.get(), "{:X} {:X}", chunk_x, chunk_y);
				for (std::uint64_t tile_y = sand::chunk_size.y; tile_y--;) {
					for (std::uint64_t tile_x = 0; tile_x < (sand::chunk_size.x - 1); ++tile_x) {
						std::print(chunk_file.get(), "{:0>2X} ", sand::world[chunk_x][chunk_y][tile_x][tile_y].texture_index);
					}
					std::println(chunk_file.get(), "{:0>2X}", sand::world[chunk_x][chunk_y][sand::chunk_size.x - 1][tile_y].texture_index);
				}
			}
		}
	}
}
