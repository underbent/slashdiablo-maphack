#pragma once

#include <string>
#include "../../Hook.h"
#include "../../Basic/Texthook/Texthook.h"

namespace Drawing {
	class Inputhook : public Hook {
		private:
			std::string text; //Text that is actually in the input box
			bool active, showCursor; //Booleans set if the hook is active / currently showing cursor.
			unsigned int xSize; //Length of the input box
			unsigned int cursorPos, cursorTick; //Cursor Position / Timer to control cursor blink
			unsigned int textPos;//Used to determine which part of the current text I should show
			unsigned int selectPos, selectLength; // Selection position and length
			unsigned int font; //What type of font to use in the input hook.
		public:
			Inputhook(HookVisibility visibility, unsigned int x, unsigned int y, unsigned int xSize, std::string formatString, ...);
			Inputhook(HookGroup* group, unsigned int x, unsigned int y, unsigned int xSize, std::string formatString, ...);

			//Getters and Setters

			//Text in the input box
			std::string GetText() { return text; };
			void SetText(std::string newText, ...);

			//If the inputhook box is active(Can be typed in)
			bool IsActive() { return active; };
			void SetActive(bool isActive) { Lock(); active = isActive; Unlock(); };

			//Font Size
			unsigned int GetFont() { return font; };
			void SetFont(unsigned int newFont);

			//X Size
			unsigned int GetXSize() { return xSize; };
			void SetXSize(unsigned int newXSize);

			//Y Size
			unsigned int GetYSize() { unsigned int height[] = {10,11,18,24,10,13,7,13,10,12,8,8,7,12}; return height[GetFont()]; };

			//If we are current showing the cursor, for blinking purposes!
			bool ShowCursor() { return showCursor; };
			void SetCursorState(bool state) { Lock(); showCursor = state; Unlock(); };
			void ToggleCursor() { SetCursorState(!ShowCursor()); };

			void CursorTick();
			void ResetCursorTick() { cursorTick = 0; };

			unsigned int GetCursorPosition() { return cursorPos; };
			void SetCursorPosition(unsigned int newPosition);
			void IncreaseCursorPosition(unsigned int len);
			void DecreaseCursorPosition(unsigned int len);

			unsigned int GetSelectionPosition() { return selectPos; };
			void SetSelectionPosition(unsigned int pos);

			unsigned int GetSelectionLength() { return selectLength; };
			void SetSelectionLength(unsigned int length);

			bool IsSelected() { return selectLength > 0; };
			void ResetSelection() { Lock(); selectPos = 0; selectLength = 0; Unlock(); };
		
			bool OnKey(bool up, BYTE key, LPARAM lParam);
			bool OnLeftClick(bool up, unsigned int x, unsigned int y);
			bool OnRightClick(bool up, unsigned int x, unsigned int y);

			unsigned int GetCharacterLimit();

			unsigned int GetTextPos() { return textPos; };
			void SetTextPos(unsigned int pos) { Lock(); textPos = pos; Unlock(); };

			void OnDraw();

			void InputText(std::string newText);
			void Backspace();
			void Replace(unsigned int pos, unsigned int len, std::string str);
			void Erase(unsigned int pos, unsigned int len);
	};
};