#pragma once

#include "../../Hook.h"
#include <string>

namespace Drawing {
	enum TextColor {Disabled=-1,White=0,Red,Green,Blue,Gold,Grey,Black,Tan,Orange,Yellow,DarkGreen,Purple,Silver=15};

	class Texthook : public Hook {
		private:
			std::string text;//Text to be drawn.
			unsigned int font;//Determines which built-in diablo font to use.
			TextColor color, hoverColor;//Determines which color to use.
		public:
			//Two Hook Initializations; one for basic hooks, one for grouped hooks.
			Texthook(HookVisibility visibility, unsigned int x, unsigned int y, std::string formatString, ...);
			Texthook(HookGroup* group, unsigned int x, unsigned int y, std::string formatString, ...);

			//Returns what D2 Font we are drawing with.
			unsigned int GetFont();

			//Sets what D2 Font to draw with
			void SetFont(unsigned int newFont);

		
			//Returns what D2 Color we are drawing with.
			TextColor GetColor();

			//Sets what D2 Color we are drawing with.
			void SetColor(TextColor newColor);


			//Returns what D2 Color we want when mouse is hovering over text, default is disabled.
			TextColor GetHoverColor();

			//Set what D2 Color we should draw with when hovered.
			void SetHoverColor(TextColor newHoverColor);


			//Returns what text we are drawing
			std::string GetText();

			//Set what text you want drawn
			void SetText(std::string formatString, ...);

			//Determine the pixel length of the text.
			unsigned int GetXSize();

			//Determine the pixel height of the text.
			unsigned int GetYSize();

			//Draw the text.
			void OnDraw();

			//Checks if we've been clicked on and calls the handler if so.
			bool OnLeftClick(bool up, unsigned int x, unsigned int y);

			//Checks if we've been clicked on and calls the handler if so.
			bool OnRightClick(bool up, unsigned int x, unsigned int y);

			//Handy function to have!
			static POINT GetTextSize(std::string text, unsigned int font);

			//Static draw text function
			static bool Draw(unsigned int x, unsigned int y, int align, unsigned int font, TextColor color, std::string text, ...);
	};
};