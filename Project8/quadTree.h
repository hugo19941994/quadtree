#pragma once
#include "point.h"
#include <math.h>
#include "circle.h"
#include <vector>
#include <array>
#include <memory>

std::string collisionObjects(const Point p, std::vector<Circle> c);

class QuadTree {
private:
	const uint8_t MAX_OBJECTS = 3;
	const uint8_t MAX_LEVELS = 100;
public:
	QuadTree(int l, Point tl, int h, int w) : level(l), tl(tl), height(h), width(w) {}

	// Level of depth of the quad tree
	const uint8_t level;

	// Bounds
	Point tl;
	int height, width;

	// Circles inside the quad
	std::vector<Circle> objects;

	std::array<std::unique_ptr<QuadTree>, 4> nodes;

	void insert(Circle c) {
		// If node is leaf
		if (this->nodes[0] == nullptr) {
			this->objects.push_back(c);

			if ((this->objects.size() > this->MAX_OBJECTS) && (this->level < this->MAX_LEVELS)) {
				this->split();
			}
		}
		else {
			int quadrant = this->calculateQuadrant(c.center, c.radius);
			if (quadrant != -1) {
				this->nodes[quadrant]->insert(c);
			} else {
				this->objects.push_back(c);
			}
		}
	}

	void split() {
		int subwidth = this->width / 2;
		int subheight = this->height / 2;

		QuadTree q0 = QuadTree(this->level + 1, Point(this->tl.x, this->tl.y), subheight, subwidth);
		QuadTree q1 = QuadTree(this->level + 1, Point(this->tl.x + subheight, this->tl.y), subheight, subwidth);
		QuadTree q2 = QuadTree(this->level + 1, Point(this->tl.x, this->tl.y + subwidth), subheight, subwidth);
		QuadTree q3 = QuadTree(this->level + 1, Point(this->tl.x + subheight, this->tl.y + subwidth), subheight, subwidth);

		this->nodes[0] = std::make_unique<QuadTree>(std::move(q0));
		this->nodes[1] = std::make_unique<QuadTree>(std::move(q1));
		this->nodes[2] = std::make_unique<QuadTree>(std::move(q2));
		this->nodes[3] = std::make_unique<QuadTree>(std::move(q3));

		auto i = std::begin(this->objects);
		while (i != std::end(this->objects)) {
			int quadrant = this->calculateQuadrant(Point(i->center.x, i->center.y), i->radius);
			if (quadrant != -1) {
				this->nodes[quadrant]->insert(*i);
				i = this->objects.erase(i);
			} else {
				++i;
			}
		}
	}
	
	std::string collision(const Point p, std::vector<Circle> cands) {
		// Get all circles
		// Only add circles of current quad if the points < horline or vertline -+ radius

		std::vector<Circle> candidates = { cands };
		// Node is not leaf. Check if its circles must be added
		if (this->nodes[0] != nullptr) {
			int horline = this->tl.x + height / 2;
			int verline = this->tl.y + width  / 2;
			// CHECK RAD
			if (p.x > tl.x && p.x < tl.x + 50 || p.x < horline + 50 && p.x > horline - 50 || p.x < tl.x + height && p.x > tl.x + height - 50 ||
				p.y > tl.y && p.y < tl.y + 50 || p.y < verline + 50 && p.y > verline - 50 || p.y < tl.y + width  && p.y > tl.y + width  - 50) {
				candidates.insert(candidates.end(), this->objects.begin(), this->objects.end());
			}
		}
		int quadrant = calculateQuadrant(p, 0);

		// Continue searching!
		if (quadrant != -1 && this->nodes[0] != nullptr) {
			return this->nodes[quadrant]->collision(p, candidates);
		} else { // STOP
			return collisionObjects(p, candidates);
		}
	}

	std::string collision(const Point p) {
		// Get all circles
		std::vector<Circle> candidates;
		if (this->nodes[0] != nullptr) {
			int horline = this->tl.x + height / 2;
			int vertline = this->tl.y + width / 2;
			// CHECK RAD
			if (p.x > tl.x && p.x < tl.x + 50 || p.x < horline + 50 && p.x > horline - 50 || p.x < tl.x + height && p.x > tl.x + height - 50 ||
				p.y > tl.y && p.y < tl.y + 50 || p.y < vertline + 50 && p.y > vertline - 50 || p.y < tl.y + width && p.y > tl.y + width - 50) {
				candidates = { this->objects };
			}
		}

		int quadrant = calculateQuadrant(p, 0);

		// Continue searching!
		if (quadrant != -1 && this->nodes[0] != nullptr) {
			return this->nodes[quadrant]->collision(p, candidates);
		} else { // STOP
			candidates.insert(candidates.end(), this->objects.begin(), this->objects.end());
			return collisionObjects(p, candidates);
		}
	}

	int calculateQuadrant(Point p, int r) {
		// 0 0     0 width
		// _________
		// | 0 | 2 |
		// |---|---|
		// |_1_|_3_|
		// h 0     h w

		int horline = this->tl.x + this->height / 2;
		int vertline = this->tl.y + this->width / 2;

		//if (p.x + r < horline && p.x - r > tl.x) {
		if (p.x + r < horline && p.x - r > tl.x) {
			// 0 or 2
			if (p.y + r < vertline && p.y - r > tl.y) {
				return 0;
			} else if (p.y - r > vertline && p.y + r < tl.y + width) {
				return 2;
			} else {
				return -1;
			}
		} else if (p.x - r > horline && p.x + r < tl.x + height) {
			// 1 or 3
			if (p.y + r < vertline && p.y - r > tl.y) {
				return 1;
			} else if (p.y - r > vertline && p.y + r < tl.y + width) {
				return 3;
			} else {
				return -1;
			}
		} else {
			return -1;
		}
	}
};

