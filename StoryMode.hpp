
/*
 * StoryMode implements a story about The Planet of Choices.
 *
 */

#include "Mode.hpp"
#include "Sprite.hpp"
#include "Dialog.hpp"
#include <vector>
#include <string>
#include <functional>

struct StoryMode : Mode {
	StoryMode();
	virtual ~StoryMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//called to create menu for current scene:
	void enter_scene();

	//------ story state -------

  std::vector<std::string> active_sprites;
  std::vector<std::string> active_variables;
  std::string active_text;
  glm::uvec2 active_text_pos = glm::vec2(-0.9f, -0.6f);

	glm::vec2 view_min = glm::vec2(0,0);
	glm::vec2 view_max = glm::vec2(256, 224);

  struct Item;
	struct Item {
		Item(
			std::string const &name_,
			Sprite const *sprite_ = nullptr,
			float scale_ = 1.0f,
			std::string const &on_select_ = "",
			glm::vec2 const &at_ = glm::vec2(0.0f)
			) : name(name_), sprite(sprite_), scale(scale_), on_select(on_select_), at(at_) {
		}
		std::string name;
		Sprite const *sprite; //sprite drawn for item
		float scale; //scale for sprite
		std::string on_select; //if set, item is selectable
		glm::vec2 at; //location to draw item
	};

  struct Menu {
    std::vector< Item > items;
  	//if set, used to highlight the current selection:
  	Sprite const *left_select = nullptr;
  	Sprite const *right_select = nullptr;
  	//must be set to the atlas from which all the sprites used herein are taken:
  	SpriteAtlas const *atlas = nullptr;
  	//currently selected item:
  	uint32_t selected = 1;
  	//area to display; by default, menu lays items out in the [-1,1]^2 box:
  	glm::uvec2 view_min = glm::vec2(-1.0f, -1.0f);
  	glm::uvec2 view_max = glm::vec2( 1.0f,  1.0f);
  } menu;
};
