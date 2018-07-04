//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "editor/scroller.hpp"

#include <math.h>

#include "editor/editor.hpp"
#include "gui/menu_manager.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

namespace {

#ifdef __ANDROID__
const float TOPLEFT = 16 * 2;
const float MIDDLE = 48 * 2;
const float BOTTOMRIGHT = 80 * 2;
const float SIZE = 96 * 2;
const float SPEED = 0.8;
#else
const float TOPLEFT = 16;
const float MIDDLE = 48;
const float BOTTOMRIGHT = 80;
const float SIZE = 96;
const float SPEED = 2;
#endif

}

#ifdef __ANDROID__
int EditorScroller::rendered = EditorScroller::SCROLLER_BOTTOM;
#else
int EditorScroller::rendered = EditorScroller::SCROLLER_TOP;
#endif

EditorScroller::EditorScroller() :
  scrolling(),
  scrolling_vec(0, 0),
  mouse_pos(0, 0)
{
}

bool
EditorScroller::can_scroll() const {
  int ypos = (rendered == SCROLLER_TOP) ? 0 : SCREEN_HEIGHT - SIZE;
  return scrolling && mouse_pos.x < SIZE && mouse_pos.y < SIZE + ypos && mouse_pos.y >= ypos;
}

void
EditorScroller::draw(DrawingContext& context) {
  if (rendered == SCROLLER_NONE) return;
  if (MenuManager::instance().is_active()) return;

  int ypos = rendered == SCROLLER_TOP ? 0 : SCREEN_HEIGHT - SIZE;
  context.draw_filled_rect(Rectf(Vector(0, ypos), Vector(SIZE, SIZE + ypos)),
                           Color(0.9f, 0.9f, 1.0f, 0.6f),
                           MIDDLE, LAYER_GUI-10);
  context.draw_filled_rect(Rectf(Vector(MIDDLE - 8, MIDDLE - 8 + ypos), Vector(MIDDLE + 8, MIDDLE + 8 + ypos)),
                           Color(0.9f, 0.9f, 1.0f, 0.6f),
                           8, LAYER_GUI-20);
  if (can_scroll()) {
    draw_arrow(context, mouse_pos);
  }

  draw_arrow(context, Vector(TOPLEFT, MIDDLE));
  draw_arrow(context, Vector(BOTTOMRIGHT, MIDDLE));
  draw_arrow(context, Vector(MIDDLE, TOPLEFT));
  draw_arrow(context, Vector(MIDDLE, BOTTOMRIGHT));
}

void
EditorScroller::draw_arrow(DrawingContext& context, const Vector& pos) {
  Vector ypos = Vector(0, rendered == SCROLLER_TOP ? 0 : SCREEN_HEIGHT - SIZE);
  Vector dir = pos - Vector(MIDDLE, MIDDLE);
  if (dir.x != 0 || dir.y != 0) {
    // draw a triangle
    dir = dir.unit() * 8;
    Vector dir2 = Vector(-dir.y, dir.x);
    context.draw_triangle(pos + dir + ypos, pos - dir + dir2 + ypos, pos - dir - dir2 + ypos,
                          Color(1, 1, 1, 0.5), LAYER_GUI-20);
  }
}

void
EditorScroller::update(float elapsed_time) {
  if (rendered == SCROLLER_NONE) return;
  if (!can_scroll()) return;

  float horiz_scroll = scrolling_vec.x * elapsed_time;
  float vert_scroll = scrolling_vec.y * elapsed_time;
  auto editor = Editor::current();

  if (horiz_scroll < 0)
    editor->scroll_left(-horiz_scroll);
  else if (horiz_scroll > 0)
    editor->scroll_right(horiz_scroll);

  if (vert_scroll < 0)
    editor->scroll_up(-vert_scroll);
  else if (vert_scroll > 0)
    editor->scroll_down(vert_scroll);
}

bool
EditorScroller::event(SDL_Event& ev) {
  switch (ev.type) {
    case SDL_MOUSEBUTTONDOWN:
    {
      if(ev.button.button == SDL_BUTTON_LEFT) {
        if (rendered == SCROLLER_NONE) return false;

        int ypos = (rendered == SCROLLER_TOP) ? 0 : SCREEN_HEIGHT - SIZE;
        if (mouse_pos.x < SIZE && mouse_pos.y < SIZE + ypos && mouse_pos.y >= ypos) {
          scrolling = true;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } break;

    case SDL_MOUSEBUTTONUP:
      scrolling = false;
      return false;
      break;

    case SDL_MOUSEMOTION:
    {
      if (rendered == SCROLLER_NONE) return false;

      int ypos = (rendered == SCROLLER_TOP) ? 0 : SCREEN_HEIGHT - SIZE;
      mouse_pos = VideoSystem::current()->get_renderer().to_logical(ev.motion.x, ev.motion.y);
      if (mouse_pos.x < SIZE && mouse_pos.y < SIZE + ypos && mouse_pos.y >= ypos) {
        scrolling_vec = mouse_pos - Vector(MIDDLE, MIDDLE + ypos);
        if (scrolling_vec.x != 0 || scrolling_vec.y != 0) {
          float norm = scrolling_vec.norm();
          scrolling_vec /= norm;
          norm = std::min(MIDDLE / 2, norm) * SPEED;
          scrolling_vec *= norm;
        }
      }
      return false;
    } break;

    case SDL_KEYDOWN:
      if (ev.key.keysym.sym == SDLK_F9) {
        rendered = !rendered;
      }
      return false;
      break;

    default:
      return false;
      break;
  }
  return true;
}

/* EOF */
