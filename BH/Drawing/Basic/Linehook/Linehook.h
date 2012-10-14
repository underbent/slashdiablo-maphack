#pragma once

#include "../../Hook.h"

namespace Drawing {
	class Linehook : public Hook {
		private:
			unsigned int color;//Color of the line hook 0-255.
			unsigned int x2, y2;//2 of the line hook.

		public:
			//Linehook Initaliztors, one for basic hooks and one for groups.
			Linehook(HookVisibility visiblity, unsigned int x, unsigned int y, unsigned int x2, unsigned int y2);
			Linehook(HookGroup* group, unsigned int x, unsigned int y, unsigned int x2, unsigned int y2);

			//Returns the color of the line hook.
			unsigned int GetColor();

			//Sets the color of the line hook.
			void SetColor(unsigned int newColor);


			//Get the size of the line hook.
			unsigned int GetX2();

			//Set the size of the x hook.
			void SetX2(unsigned int newX);

			//Filler to return nothing to base class.
			unsigned int GetXSize();
			unsigned int GetYSize();

			//Get the height of the line hook.
			unsigned int GetY2();

			//Set the height of the line hook.
			void SetY2(unsigned int newY);

			//Draw the text.
			void OnDraw();

			//Static line draw
			static bool Draw(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2, unsigned int color);
	};
};