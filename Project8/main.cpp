#include "quadTree.h"
#include <iostream>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <random>
#include <ctime>
#include <random>

void drawRectangle(const std::unique_ptr<QuadTree>& q) {
	al_draw_rectangle(q->tl.x, q->tl.y, q->tl.x + q->height, q->tl.y + q->width,
		al_map_rgb(255, 255, 255), 1.0);

	for (int i = 0; i < 4; ++i) {
		if (q->nodes[i] != nullptr) {
			drawRectangle(q->nodes[i]);
		}
	}
}

int main() {
	// Init Allegro
	al_init();
	al_init_font_addon();
	ALLEGRO_DISPLAY* display = al_create_display(1000, 1000);
	ALLEGRO_FONT* font = al_create_builtin_font();
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_init_primitives_addon();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1000);
	std::uniform_int_distribution<> col(0, 255);

	QuadTree q = QuadTree(0, Point(0, 0), 1000, 1000);
	for (int i = 0; i < 300; ++i) {
		Circle c = Circle(Point(dis(gen), dis(gen)), 50, i, std::to_string(i));
		q.insert(c);

		al_draw_circle((float)c.center.x, (float)c.center.y, 50.0,
			al_map_rgb(col(gen), col(gen), col(gen)), 1.0);
		al_draw_text(font, al_map_rgb(255, 255, 255), c.center.x, c.center.y, ALLEGRO_ALIGN_CENTRE, c.id.c_str());
	}

	drawRectangle(q.nodes[0]);
	drawRectangle(q.nodes[1]);
	drawRectangle(q.nodes[2]);
	drawRectangle(q.nodes[3]);

	std::cout << q.collision(Point(520, 520)) << std::endl;;

		al_draw_circle((float)520, (float)520, 5.0,
			al_map_rgb(255, 255, 255), 10.0);

	al_flip_display();
	while (true) {}
	return 0;
}

