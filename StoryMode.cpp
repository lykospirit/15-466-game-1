#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "Dialog.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include <algorithm>

Sprite const *sprite_left_select = nullptr;
Sprite const *sprite_right_select = nullptr;
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
	enter_scene();
}

void StoryMode::enter_scene() {
	//just entered this scene, adjust flags and build menu as appropriate:
	std::vector< Item > items;
	glm::vec2 at(10.0f, view_min.y + 63.5f);
  float text_scale = 0.5f;

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

	menu.atlas = sprites;
	menu.left_select = sprite_left_select;
	menu.right_select = sprite_right_select;
	menu.view_min = view_min;
	menu.view_max = view_max;
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
