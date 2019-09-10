#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "Dialog.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
// #include "MenuMode.hpp"
#include <algorithm>

Sprite const *sprite_left_select = nullptr;
Sprite const *sprite_right_select = nullptr;

/*
Sprite const *sprite_dunes_bg = nullptr;
Sprite const *sprite_dunes_traveller = nullptr;
Sprite const *sprite_dunes_ship = nullptr;

Sprite const *sprite_oasis_bg = nullptr;
Sprite const *sprite_oasis_traveller = nullptr;
Sprite const *sprite_oasis_missing = nullptr;

Sprite const *sprite_hill_bg = nullptr;
Sprite const *sprite_hill_traveller = nullptr;
Sprite const *sprite_hill_missing = nullptr;

Sprite const *text_dunes_landing = nullptr;
Sprite const *text_dunes_return = nullptr;
Sprite const *text_dunes_wont_leave = nullptr;
Sprite const *text_dunes_do_leave = nullptr;
Sprite const *text_dunes_do_walk_east = nullptr;
Sprite const *text_dunes_do_walk_west = nullptr;

Sprite const *text_oasis_intro = nullptr;
Sprite const *text_oasis_stone = nullptr;
Sprite const *text_oasis_plain = nullptr;
Sprite const *text_oasis_stone_taken = nullptr;
Sprite const *text_oasis_do_take_stone = nullptr;
Sprite const *text_oasis_do_return = nullptr;

Sprite const *text_hill_intro = nullptr;
Sprite const *text_hill_inactive = nullptr;
Sprite const *text_hill_active = nullptr;
Sprite const *text_hill_stone_added = nullptr;
Sprite const *text_hill_do_add_stone = nullptr;
Sprite const *text_hill_do_return = nullptr;
*/

Sprite const *sprite_room_bg = nullptr;
Sprite const *sprite_bath_bg = nullptr;
Sprite const *sprite_clock_bg = nullptr;
Sprite const *sprite_calendar_bg = nullptr;
Sprite const *sprite_exit_bg = nullptr;
Sprite const *sprite_black_bg = nullptr;

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("the-planet"));

	sprite_left_select = &ret->lookup("text-select-left");
	sprite_right_select = &ret->lookup("text-select-right");

  /*
	sprite_dunes_bg = &ret->lookup("dunes-bg");
	sprite_dunes_traveller = &ret->lookup("dunes-traveller");
	sprite_dunes_ship = &ret->lookup("dunes-ship");

	sprite_oasis_bg = &ret->lookup("oasis-bg");
	sprite_oasis_traveller = &ret->lookup("oasis-traveller");
	sprite_oasis_missing = &ret->lookup("oasis-missing");

	sprite_hill_bg = &ret->lookup("hill-bg");
	sprite_hill_traveller = &ret->lookup("hill-traveller");
	sprite_hill_missing = &ret->lookup("hill-missing");

	text_dunes_landing = &ret->lookup("dunes-text-landing");
	text_dunes_return = &ret->lookup("dunes-text-return");
	text_dunes_wont_leave = &ret->lookup("dunes-text-won't-leave");
	text_dunes_do_leave = &ret->lookup("dunes-text-do-leave");
	text_dunes_do_walk_east = &ret->lookup("dunes-text-do-walk-east");
	text_dunes_do_walk_west = &ret->lookup("dunes-text-do-walk-west");

	text_oasis_intro = &ret->lookup("oasis-text-intro");
	text_oasis_stone = &ret->lookup("oasis-text-stone");
	text_oasis_plain = &ret->lookup("oasis-text-plain");
	text_oasis_stone_taken = &ret->lookup("oasis-text-stone-taken");
	text_oasis_do_take_stone = &ret->lookup("oasis-text-do-take-stone");
	text_oasis_do_return = &ret->lookup("oasis-text-do-return");

	text_hill_intro = &ret->lookup("hill-text-intro");
	text_hill_inactive = &ret->lookup("hill-text-inactive");
	text_hill_active = &ret->lookup("hill-text-active");
	text_hill_stone_added = &ret->lookup("hill-text-stone-added");
	text_hill_do_add_stone = &ret->lookup("hill-text-do-add-stone");
	text_hill_do_return = &ret->lookup("hill-text-do-return");
  */

  sprite_room_bg = &ret->lookup("room-bg");
  sprite_bath_bg = &ret->lookup("bath-bg");
  sprite_clock_bg = &ret->lookup("clock-bg");
  sprite_calendar_bg = &ret->lookup("calendar-bg");
  sprite_exit_bg = &ret->lookup("exit-bg");
  sprite_black_bg = &ret->lookup("black-bg");

	return ret;
});

Load< Dialog > dialog(LoadTagDefault, []() -> Dialog const * {
  Dialog const *ret = new Dialog(data_path("dialog"));
  return ret;
});

StoryMode::StoryMode(){
  // TODO: add starting condition to pipeline
  active_sprites.push_back("room-bg");
  active_text = "You've woken up late! Time to get to work!";
	for (uint32_t i = 0; i < menu.items.size(); ++i) {
		if (!menu.items[i].on_select.empty()) {
			menu.selected = i;
			break;
		}
	}
}

StoryMode::~StoryMode(){}

bool StoryMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	// if (Mode::current.get() != this) return false;
	// return false;
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_UP) {
			//skip non-selectable items:
			for (uint32_t i = menu.selected - 1; i < menu.items.size(); --i) {
				if (!menu.items[i].on_select.empty()) {
					menu.selected = i;
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			//note: skips non-selectable items:
			for (uint32_t i = menu.selected + 1; i < menu.items.size(); ++i) {
				if (!menu.items[i].on_select.empty()) {
					menu.selected = i;
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN) {
			if (menu.selected < menu.items.size() && !menu.items[menu.selected].on_select.empty()) {
				// menu.items[menu.selected].on_select(menu.items[menu.selected]);
        std::string event = menu.items[menu.selected].on_select;
        std::vector<Instruction> is = dialog->lookup(event);
        std::vector<Instruction>::iterator it;
        for (it = is.begin(); it != is.end(); it++) {
          // Search for first rule that satisfies predicate
          std::vector<std::string>::iterator c;
          std::vector<std::string> &pred = it->pred;
          bool sat = true;
          for (c = pred.begin(); c != pred.end(); c++) {
            if (std::find(active_variables.begin(), active_variables.end(), *c) == active_variables.end()) {
              sat = false;
            }
          }
          if (!sat) continue;
          active_variables.insert(active_variables.end(), it->toTrue.begin(), it->toTrue.end());
          active_sprites.insert(active_sprites.end(), it->spriteOn.begin(), it->spriteOn.end());
          for (c = it->toFalse.begin(); c != it->toFalse.end(); c++) {
            auto isVar = [&](const std::string &s){ return (s.compare(*c) == 0); };
            std::vector<std::string>::iterator end = std::remove_if(active_variables.begin(), active_variables.end(), isVar);
            active_variables.erase(end, active_variables.end());
          }
          for (c = it->spriteOff.begin(); c != it->spriteOff.end(); c++) {
            auto isSprite = [&](const std::string &s){ return (s.compare(*c) == 0); };
            std::vector<std::string>::iterator end = std::remove_if(active_sprites.begin(), active_sprites.end(), isSprite);
            active_sprites.erase(end, active_sprites.end());
          }

          auto print_string_vector = [](std::vector<std::string> &v){
            std::vector<std::string>::iterator s;
            std::cout << v.size();
            for (s = v.begin(); s != v.end(); s++){
              std::cout << (*s);
            }
            std::cout << "|";
          };
          print_string_vector(it->pred);
          print_string_vector(it->toTrue);
          print_string_vector(it->toFalse);
          print_string_vector(it->spriteOn);
          print_string_vector(it->spriteOff);
          std::cout << std::endl;
          print_string_vector(active_variables);
          print_string_vector(active_sprites);
          std::cout << std::endl;

          active_text = it->text;
          menu.selected = 1;
          break;
        }
        return true;
			}
		}
	}
  return false;
}

void StoryMode::update(float elapsed) {
	// if (Mode::current.get() == this) { //there is no menu displayed! Make one:
	enter_scene();
	// }
}

void StoryMode::enter_scene() {
	//just entered this scene, adjust flags and build menu as appropriate:
	std::vector< Item > items;
	glm::vec2 at(10.0f, view_min.y + 63.5f);
  float text_scale = 0.5f;

  /*
	auto add_text = [&items,&at](Sprite const *text) {
		assert(text);
		items.emplace_back("TEST TEXT", nullptr, 1.0f, nullptr, at);
		at.y -= text->max_px.y - text->min_px.y;
		at.y -= 4.0f;
	};
	auto add_choice = [&items,&at](Sprite const *text, std::function< void(MenuMode::Item const &) > const &fn) {
		assert(text);
		items.emplace_back("TEST CHOICE", nullptr, 1.0f, fn, at + glm::vec2(8.0f, 0.0f));
		at.y -= text->max_px.y - text->min_px.y;
		at.y -= 4.0f;
	};
	if (location == Dunes) {
		if (dunes.wont_leave) {
			dunes.wont_leave = false;
			add_text(text_dunes_wont_leave);
		}
		if (dunes.first_visit) {
			dunes.first_visit = false;
			add_text(text_dunes_landing);
		} else {
			add_text(text_dunes_return);
		}
		at.y -= 8.0f; //gap before choices
		add_choice(text_dunes_do_walk_west, [this](MenuMode::Item const &){
			location = Hill;
			Mode::current = shared_from_this();
		});
		add_choice(text_dunes_do_walk_east, [this](MenuMode::Item const &){
			location = Oasis;
			Mode::current = shared_from_this();
		});
		if (!dunes.first_visit) {
			add_choice(text_dunes_do_leave, [this](MenuMode::Item const &){
				if (added_stone) {
					//TODO: some sort of victory animation?
					Mode::current = nullptr;
				} else {
					dunes.wont_leave = true;
					Mode::current = shared_from_this();
				}
			});
		}
	} else if (location == Oasis) {
		if (oasis.took_stone) {
			oasis.took_stone = false;
			add_text(text_oasis_stone_taken);
		}
		if (oasis.first_visit) {
			oasis.first_visit = false;
			add_text(text_oasis_intro);
		} else {
			add_text(text_oasis_plain);
		}
		if (!have_stone) {
			add_text(text_oasis_stone);
		}
		at.y -= 8.0f; //gap before choices
		if (!have_stone) {
			add_choice(text_oasis_do_take_stone, [this](MenuMode::Item const &){
				have_stone = true;
				oasis.took_stone = true;
				Mode::current = shared_from_this();
			});
		}
		add_choice(text_oasis_do_return, [this](MenuMode::Item const &){
			location = Dunes;
			Mode::current = shared_from_this();
		});
	} else if (location == Hill) {
		if (hill.added_stone) {
			hill.added_stone = false;
			add_text(text_hill_stone_added);
		}
		if (hill.first_visit) {
			hill.first_visit = false;
			add_text(text_hill_intro);
		} else {
			if (added_stone) {
				add_text(text_hill_active);
			} else {
				add_text(text_hill_inactive);
			}
		}
		at.y -= 8.0f; //gap before choices
		if (have_stone && !added_stone) {
			add_choice(text_hill_do_add_stone, [this](MenuMode::Item const &){
				added_stone = true;
				hill.added_stone = true;
				Mode::current = shared_from_this();
			});
		}
		add_choice(text_hill_do_return, [this](MenuMode::Item const &){
			location = Dunes;
			Mode::current = shared_from_this();
		});
	}
  */
	auto add_choice = [&](std::string text, std::string event) {
    menu.items.emplace_back(text, nullptr, text_scale, event, at);
		at.y -= 8.0f;
	};
  //
  menu.items.clear();
  menu.items.emplace_back(active_text, nullptr, text_scale, "", at);
  at.y -= 14.0f;
  if (active_sprites[0] == "room-bg") {
    add_choice("Leave", "room/gotoExit");
    add_choice("Go to bathroom", "room/gotoBath");
    add_choice("Get dressed", "room/interactCloset");
    add_choice("Go to bed", "room/interactBed");
    add_choice("Look at clock", "room/gotoClock");
    add_choice("Look at calendar", "room/gotoCalendar");
  } else if (active_sprites[0] == "bath-bg") {
    add_choice("Go to room", "bath/gotoRoom");
    add_choice("Wash up", "bath/interactSink");
  } else if (active_sprites[0] == "clock-bg") {
    add_choice("Go to room", "clock/gotoRoom");
    add_choice("Look at clock", "clock/interactTime");
  } else if (active_sprites[0] == "exit-bg") {
    add_choice("Head home", "exit/interactSign");
  } else if (active_sprites[0] == "calendar-bg") {
    add_choice("Go to room", "calendar/gotoRoom");
    add_choice("Look at today's date", "calendar/interactDate");
  }

	// std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >(items);
	menu.atlas = sprites;
	menu.left_select = sprite_left_select;
	menu.right_select = sprite_right_select;
	menu.view_min = view_min;
	menu.view_max = view_max;
	// menu->background = shared_from_this();
	// Mode::current = menu;
}

void StoryMode::draw(glm::uvec2 const &drawable_size) {
	//clear the color buffer:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);
		glm::vec2 ul = glm::vec2(view_min.x, view_max.y);
		/*
    if (location == Dunes) {
			draw.draw(*sprite_dunes_bg, ul);
			draw.draw(*sprite_dunes_ship, ul);
			draw.draw(*sprite_dunes_traveller, ul);
		} else if (location == Oasis) {
			draw.draw(*sprite_oasis_bg, ul);
			if (!have_stone) {
				draw.draw(*sprite_oasis_missing, ul);
			}
			draw.draw(*sprite_oasis_traveller, ul);

		} else if (location == Hill) {
			draw.draw(*sprite_hill_bg, ul);
			if (added_stone) {
				draw.draw(*sprite_hill_missing, ul);
			}
			draw.draw(*sprite_hill_traveller, ul);
		}
    */
    for (std::vector<std::string>::iterator it = active_sprites.begin(); it != active_sprites.end(); it++) {
      draw.draw(sprites->lookup(*it), ul);
    }

	}

  { //draw the menu using DrawSprites:
		assert(menu.atlas && "it is an error to try to draw a menu without an atlas");
		DrawSprites draw_sprites(*(menu.atlas), menu.view_min, menu.view_max, drawable_size, DrawSprites::AlignPixelPerfect);
    // draw_sprites.draw_text(active_text, active_text_pos, 1.0f, glm::u8vec4(0xff, 0x00, 0xff, 0xff));
    for (auto const &item : menu.items) {
			bool is_selected = (&item == &(menu.items)[0] + menu.selected);
			glm::u8vec4 color = (is_selected ? glm::u8vec4(0xff, 0xff, 0x00, 0xff) : glm::u8vec4(0xff, 0xff, 0xff, 0xff));
			float left, right, height = 0.0f;
			if (!item.sprite) {
				//draw item.name as text:
				draw_sprites.draw_text(item.name, item.at, item.scale, color);
				glm::vec2 min, max;
				draw_sprites.get_text_extents(&min, &max, item.name, item.at, item.scale);
				left = min.x;
				right = max.x;
        height = max.y - min.y;
			} else {
				draw_sprites.draw(*item.sprite, item.at, item.scale, color);
				left = item.at.x + item.scale * (item.sprite->min_px.x - item.sprite->anchor_px.x);
				right = item.at.x + item.scale * (item.sprite->max_px.x - item.sprite->anchor_px.x);
			}
			if (is_selected) {
				if (menu.left_select) {
					draw_sprites.draw(*(menu.left_select), glm::vec2(left, item.at.y + height), item.scale);
				}
				if (menu.right_select) {
					draw_sprites.draw(*(menu.right_select), glm::vec2(right, item.at.y + height), item.scale);
				}
			}

		}
	} //<-- gets drawn here!
	GL_ERRORS(); //did the DrawSprites do something wrong?
}
