#define OLC_PGE_APPLICATION
#include <iostream>
#include <stdlib.h> 
#include "olcPixelGameEngine.h"
#include <cmath>
#include <memory>
#include <string>


class LineOfSight : public olc::PixelGameEngine {
private:
	std::unique_ptr<olc::Sprite> background;
	bool levelReset;
	struct shipObject {
		float x;
		float y;
		float dy;
		float dy2;
		float grav;
		float damage;
		int alive;
		bool broken;
	};
	struct lazerObject {
		float xstat, ystat;
		float xmov, ymov;
		float tSign;
		float theta, dx, dy;
		float rotSpeed;
	};
	void WrapCoordinates(float ix, float iy, float &ox, float &oy) {
		ox = ix; oy = iy;
		if (ix >= (float)ScreenWidth()) { ox = ix - (float)ScreenWidth(); }
		else if (ix < 0.0f) { ox = ix + (float)ScreenWidth(); }

		if (iy >= (float)ScreenHeight()) { oy = iy - (float)ScreenHeight(); }
		else if (iy < 0.0f) { oy = iy + (float)ScreenHeight(); }
	}
	void WrapAngle(float it, float& ot) {
		ot = it;
		float pi = 3.1415;

		if (it > (pi / 2.0f)) { ot = it - pi; }
		else if (it < -(pi / 2.0f)) { ot = it + pi; }
	}
	bool Draw(int x, int y, short c = 0x2588, short col = 0x000F) {
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olc::PixelGameEngine::Draw(fx, fy, olc::BLACK);
		return false;
	}
	bool changeSign() {
		int sampleVal = rand() % 100;
		if (sampleVal > 1) { return false; }
		else { return true; }
	}
	//Check collinearity (line of sight) with lazer by computing the area of the triange formed by the ship and the lazer.
	//Uses the matrix determinant formula for the area of a triangle.
	bool isCollinear(float x1, float y1, float x2, float y2, float x3, float y3) {
		float triangleArea = (x2 * y3) - (x3 * y2) - (x1 * y3) + (x3 * y1) + (x1 * y2) - (x2 * y1);
		if (std::abs(triangleArea) < 9.0f) { return true; }
		else { return false; }
	}
	shipObject player;
	lazerObject lazer;
public:
	LineOfSight() {
		sAppName = "LineOfSight";
	}
	//Called upon starting the game.
	bool OnUserCreate() override {
		levelReset = true;
		background = std::make_unique<olc::Sprite>("lineofSightBackground1.png");
		DrawSprite(olc::vi2d(0, 0), background.get());
		return true;
	}
	//Called each frame of the game. This is the "game loop".
	bool OnUserUpdate(float fElapsedTime) override{
		//Reset level with starting state
		if (levelReset) {
			player.x = ScreenWidth() / 4.0f;
			player.y = 3.0f * ScreenHeight() / 4.0f;
			player.damage = 0.0f;
			player.dy = 0; player.dy2 = 0; player.grav = 100.0f; player.alive = 0; player.broken = false;
			lazer.xstat = (3.0f * ScreenWidth() / 4.0f) + 10.0f;
			lazer.ystat = ScreenHeight() / 2.0f;
			lazer.xmov = -20.0f;
			lazer.ymov = 0.0f;
			lazer.theta = 0.0f; lazer.dx = 0.0f; lazer.ymov = 0.0f; lazer.tSign = 1.0f; lazer.rotSpeed = 1.0f;
			levelReset = false;
		}
	    DrawSprite(0, 0, background.get());
		//Used to compute the score
		player.alive++;
		//Update lazer rotation speed
		if (player.alive > 7500.0f) { lazer.rotSpeed = 2.5f; }
		else if (player.alive > 1000.0f) { lazer.rotSpeed = 1.5f; }

		//Up/down
		//If ship builds up enough damage, it become broken which changes the thrust type
		if (!player.broken) {
			if (GetKey(olc::Key::DOWN).bHeld) {
				player.y += 25.0f * fElapsedTime;
			}
			if (GetKey(olc::Key::UP).bHeld) {

				player.y -= 25.0f * fElapsedTime;
			}
		}
		else {
			if (GetKey(olc::Key::UP).bPressed) {
				player.dy2 = 0.0f;
				player.dy = -player.grav / 4.0f -10.0f;
			}
			else {
				player.dy2 += player.grav * fElapsedTime * 1.1f;
			}
			if (player.dy2 >= player.grav) { player.dy2 = player.grav; }

			player.dy += player.dy2 * fElapsedTime;
			if (player.dy >= 10.0f) { player.dy = 45.0f; }
			player.y += player.dy * fElapsedTime;
		}
		
		//Ship coordinates wrap around if limits of console exceeded
		WrapCoordinates(player.x, player.y, player.x, player.y);

		//Player line (ship is two lines in an arrow shape)
		float sx[2] = { player.x - 3.0f, player.x + 3.0f };
		float sy[2] = { player.y, player.y+3.0f };

		//Draw line for space ship, takes damage into account
		DrawLine((sx[0]+player.damage), (sy[0]+player.damage), sx[1], sy[1], olc::BLACK);
		DrawLine((sx[1]+player.damage), sy[1], sx[0], sy[0] + 6.0f - player.damage, olc::BLACK);
		

		//Update lazer theta. The lazer has a small probability of changing direction each frame.
		if (changeSign()) { lazer.tSign = -1.0f * lazer.tSign; }
		lazer.theta += lazer.tSign *fElapsedTime * lazer.rotSpeed;

		WrapAngle(lazer.theta, lazer.theta);
		//rotate around origin
		float lazerxt = lazer.xmov * cos(lazer.theta) - lazer.ymov * sin(lazer.theta);
		float lazeryt = lazer.xmov * sin(lazer.theta) + lazer.ymov * cos(lazer.theta);
		//translate back
		lazerxt += lazer.xstat;
		lazeryt += lazer.ystat;

		//draw lazer line
		DrawLine(lazerxt, lazeryt, lazer.xstat, lazer.ystat, olc::GREY);

		//check line of sight
		if (isCollinear(player.x, player.y, lazerxt, lazeryt, lazer.xstat, lazer.ystat)) {
			player.damage+=0.2;
			DrawLine(player.x, player.y, lazer.xstat, lazer.ystat, olc::DARK_RED); }
		if (player.damage >= 2.0f) { player.broken=true; }
		std::string score = "   Score " + std::to_string((player.alive - player.alive%100)/100);
		DrawString(1, 1, score, olc::BLACK) ;
		if (player.damage >= 3.4f) { levelReset = true; }
		return true;
	}
};

int main() {

	LineOfSight demo;
	demo.Construct(160, 140, 4, 4);
	demo.Start();
	return 0;
} 