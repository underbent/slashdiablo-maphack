#pragma once
#include "../../../Constants.h"
#include "../../Hook.h"
#include "../../Basic/Texthook/Texthook.h"

namespace Drawing {
	class Checkhook : public Hook {
		private:
			bool* state;//Holds if the checkbox is checked.
			TextColor color, hoverColor;//Holds text color/hover color.
			std::string text;//The text beside the checkhook.
		public:
			Checkhook(HookVisibility visibility, unsigned int x, unsigned int y, bool* checked, std::string formatString, ...);
			Checkhook(HookGroup* group, unsigned int x, unsigned int y, bool* checked, std::string formatString, ...);

			//Returns if the check is checked.
			bool IsChecked();

			//Sets if it is checked or not.
			void SetState(bool checked);

			//Returns the text color.
			TextColor GetTextColor();

			//Returns the hover color
			TextColor GetHoverColor();

			//Sets the text color
			void SetTextColor(TextColor newColor);

			//Sets the hover color
			void SetHoverColor(TextColor newColor);

			//Gets the text
			std::string GetText();

			//Sets the text
			void SetText(std::string formatString, ...);

			//Returns the total width of the check hook
			unsigned int GetXSize();

			//Returns the total hright of the check hook
			unsigned int GetYSize();

			//Draw the text.
			void OnDraw();

			//Checks if we've been clicked on and calls the handler if so.
			bool OnLeftClick(bool up, unsigned int x, unsigned int y);

			//Checks if we've been clicked on and calls the handler if so.
			bool OnRightClick(bool up, unsigned int x, unsigned int y);
	};
};