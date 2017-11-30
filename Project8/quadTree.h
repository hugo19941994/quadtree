#pragma once
#include "point.h"
#include <math.h>
#include "circle.h"
#include <vector>
#include <array>
#include <memory>

class QuadTree {
private:
	const uint8_t MAX_OBJECTS = 5;
	const uint8_t MAX_LEVELS = 5;
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
			this->nodes[calculateQuadrant(c.center, c.radius)]->insert(c);
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

		for (auto &i : this->objects) {
			this->nodes[this->calculateQuadrant(Point(i.center.x, i.center.y), i.radius)]->insert(i);
		}

		this->objects.clear();
	}
	
	std::string collision(const Point p) {
		// Leaf node
		if (this->nodes[0] == nullptr) {
			return this->collisionObjects(p);
		}

		return this->nodes[calculateQuadrant(p, 0)]->collision(p);
	}

	std::string collisionObjects(const Point p) {
		// For each object with which the point collides return the one with the lowest z-buffer (closest)
		Circle candidate = Circle(Point(0, 0), 0, 999, "");
		for (auto &i : this->objects) {
			if (sqrt(pow(p.x - i.center.x, 2) + pow(p.y - i.center.y, 2)) < i.radius) {
				// inside
				if (i.z < candidate.z) {
					candidate = i;
				}
			}
		}

		return candidate.id;
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

		// TODO: circles in parents
		if (p.x + r < horline) {
			// 0 or 2
			if (p.y + r < vertline) {
				return 0;
			} else {
				return 2;
			}
		} else {
			// 1 or 3
			if (p.y + r < vertline) {
				return 1;
			} else {
				return 3;
			}
		}
	}
};