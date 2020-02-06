/*
**  jstest-gtk - A graphical joystick tester
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "button_widget.hpp"
#include <vector>

static void draw_label(Cairo::RefPtr<Cairo::ImageSurface>& target, int width, int height, const std::string& text)
{
  Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(target);
  int w = /* target->get_width()  */ width  - 10;
  int h = /* target->get_height() */ height - 10;

  // FIXME: There are better ways to center text
  if (text.size() == 2)
    cr->move_to(w / 2 - 6, h / 2 + 3);
  else
    cr->move_to(w / 2 - 4, h / 2 + 3);
  cr->show_text(text);
}

static void invert_alpha(Cairo::RefPtr<Cairo::ImageSurface>& target)
{
  const int stride = target->get_stride();
  const int w = target->get_width();
  const int h = target->get_height();

  for (int i = 0; i < h; ++i)
  {
    uint8_t* p = (uint8_t*)target->get_data() + i * stride;
    for (int j = 0; j < w; ++j)
    {
      (*p) = ~(*p);
      ++p;
    }
  }

  target->mark_dirty();
}

ButtonWidget::ButtonWidget(int width, int height, const std::string& name_)
  : name(name_)
{
  // draw regular label mask
  set_size_request(width, height);
  label = Cairo::ImageSurface::create(Cairo::FORMAT_A8, width, height);
  draw_label(label, width, height, name);

  // draw inverted label mask
  //labelinv = Cairo::ImageSurface::create(Cairo::FORMAT_A8, width, height);
  labelinv = Cairo::ImageSurface::create(Cairo::FORMAT_A8, width - 10, height - 10); // hack to stop it bleeding out
  draw_label(labelinv, width, height, name);
  labelinv->flush();
  invert_alpha(labelinv);
}

bool
ButtonWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  Gtk::DrawingArea::on_draw(cr);

    int w  = get_allocation().get_width()  - 10;
    int h  = get_allocation().get_height() - 10;

    auto fgcolor = get_style_context()->get_color(Gtk::STATE_FLAG_NORMAL);

    cr->set_source_rgb(fgcolor.get_red(), fgcolor.get_green(), fgcolor.get_blue());

    cr->set_line_width(1.0);
    cr->translate(5, 5);
    cr->rectangle(0, 0, w, h);
    cr->stroke();

    //FIXME: this sucks and I don't know what I'm doing
    cr->mask(down ? labelinv : label, 0.0, 0.0);
    cr->fill_preserve();
    cr->stroke();

  return true;
}

void
ButtonWidget::set_down(bool t)
{
  down = t;
  queue_draw();
}

/* EOF */
