#include "ItemMover.h"
#include "../../BH.h"
#include "../../D2Ptrs.h"
#include "../../D2Stubs.h"

// This module was inspired by the RedVex plugin "Item Mover", written by kaiks.
// Thanks to kaiks for sharing his code.

bool ItemMover::LoadInventory(UnitAny *unit, int xpac, int source, int sourceX, int sourceY, int destination) {
	bool returnValue = false;
	memset(InventoryItemIds, 0, INVENTORY_WIDTH * INVENTORY_HEIGHT * sizeof(int));
	memset(StashItemIds, 0, STASH_WIDTH * LOD_STASH_HEIGHT * sizeof(int));
	memset(CubeItemIds, 0, CUBE_WIDTH * CUBE_HEIGHT * sizeof(int));

	if (!xpac) {
		for (int x = 0; x < STASH_WIDTH; x++) {
			for (int y = CLASSIC_STASH_HEIGHT; y < LOD_STASH_HEIGHT; y++) {
				StashItemIds[y*STASH_WIDTH + x] = -1;
			}
		}
	}

	unsigned int itemId = 0;
	BYTE itemXSize, itemYSize;
	for (UnitAny *pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		int *p, width;
		if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY) {
			p = InventoryItemIds;
			width = INVENTORY_WIDTH;
		} else if (pItem->pItemData->ItemLocation == STORAGE_STASH) {
			p = StashItemIds;
			width = STASH_WIDTH;
		} else if (pItem->pItemData->ItemLocation == STORAGE_CUBE) {
			p = CubeItemIds;
			width = CUBE_WIDTH;
		} else {
			continue;
		}
		int xStart = pItem->pObjectPath->dwPosX;
		int yStart = pItem->pObjectPath->dwPosY;
		BYTE xSize = D2COMMON_GetItemText(pItem->dwTxtFileNo)->xSize;
		BYTE ySize = D2COMMON_GetItemText(pItem->dwTxtFileNo)->ySize;
		for (int x = xStart; x < xStart + xSize; x++) {
			for (int y = yStart; y < yStart + ySize; y++) {
				p[y*width + x] = pItem->dwUnitId;
				if (x == sourceX && y == sourceY && pItem->pItemData->ItemLocation == source) {
					// This is the item we want to move
					itemId = pItem->dwUnitId;
					itemXSize = xSize;
					itemYSize = ySize;
				}
			}
		}
	}

	// Find a spot for the item in the destination container
	if (itemId > 0) {
		returnValue = FindDestination(xpac, destination, itemId, itemXSize, itemYSize);
	}

	FirstInit = true;
	return returnValue;
}

bool ItemMover::FindDestination(int xpac, int destination, unsigned int itemId, BYTE xSize, BYTE ySize) {
	int *p, width = 0, height = 0;
	if (destination == STORAGE_INVENTORY) {
		p = InventoryItemIds;
		width = INVENTORY_WIDTH;
		height = INVENTORY_HEIGHT;
	} else if (destination == STORAGE_STASH) {
		p = StashItemIds;
		width = STASH_WIDTH;
		height = xpac ? LOD_STASH_HEIGHT : CLASSIC_STASH_HEIGHT;
	} else if (destination == STORAGE_CUBE) {
		p = CubeItemIds;
		width = CUBE_WIDTH;
		height = CUBE_HEIGHT;
	}

	bool found = false;
	int destX = 0, destY = 0;
	if (width) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				bool abort = false;
				int vacancies = 0;
				for (int testx = x; testx < x + xSize && testx < width; testx++) {
					for (int testy = y; testy < y + ySize && testy < height; testy++) {
						if (p[testy*width + testx]) {
							abort = true;
							break;
						} else {
							vacancies++;
						}
					}
					if (abort) {
						break;
					}
				}
				if (vacancies == xSize * ySize) {
					// Found an empty spot that's big enough for the item
					found = true;
					destX = x;
					destY = y;
					break;
				}
			}
			if (found) {
				break;
			}
		}
	} else {
		found = true;
	}

	if (found) {
		Lock();
		if (ActivePacket.startTicks == 0) {
			ActivePacket.itemId = itemId;
			ActivePacket.x = destX;
			ActivePacket.y = destY;
			ActivePacket.startTicks = GetTickCount64();
			ActivePacket.destination = destination;
		}
		Unlock();
	}

	return found;
}

void ItemMover::PickUpItem() {
	BYTE PacketData[5] = {0x19,0,0,0,0};
    *reinterpret_cast<int*>(PacketData + 1) = ActivePacket.itemId;
	D2NET_SendPacket(5, 1, PacketData);
}

void ItemMover::PutItemInContainer() {
	BYTE PacketData[17] = {0x18,0,0,0,0};
    *reinterpret_cast<int*>(PacketData + 1) = ActivePacket.itemId;
    *reinterpret_cast<int*>(PacketData + 5) = ActivePacket.x;
    *reinterpret_cast<int*>(PacketData + 9) = ActivePacket.y;
    *reinterpret_cast<int*>(PacketData + 13)= ActivePacket.destination;
	D2NET_SendPacket(17, 1, PacketData);
}

void ItemMover::PutItemOnGround() {
	BYTE PacketData[5] = {0x17,0,0,0,0};
    *reinterpret_cast<int*>(PacketData + 1) = ActivePacket.itemId;
	D2NET_SendPacket(5, 1, PacketData);
}

void ItemMover::OnRightClick(bool up, int x, int y, bool* block) {
	BnetData* pData = (*p_D2LAUNCH_BnData);
	UnitAny *unit = D2CLIENT_GetPlayerUnit();
	bool shiftState = ((GetKeyState(VK_LSHIFT) & 0x80) || (GetKeyState(VK_RSHIFT) & 0x80));
	bool ctrlState = ((GetKeyState(VK_LCONTROL) & 0x80) || (GetKeyState(VK_RCONTROL) & 0x80));
	if (up || !pData || !unit || !(shiftState || ctrlState)) {
		return;
	}

	int xpac = pData->nCharFlags & PLAYER_TYPE_EXPANSION;

	int inventoryRight = INVENTORY_LEFT + (CELL_SIZE * INVENTORY_WIDTH);
	int inventoryBottom = INVENTORY_TOP + (CELL_SIZE * INVENTORY_HEIGHT);
	int stashRight = STASH_LEFT + (CELL_SIZE * STASH_WIDTH);
	int stashTop = xpac ? LOD_STASH_TOP : CLASSIC_STASH_TOP;
	int stashHeight = xpac ? 8 : 4;
	int stashBottom = stashTop + (CELL_SIZE * stashHeight);
	int cubeRight = CUBE_LEFT + (CELL_SIZE * CUBE_WIDTH);
	int cubeBottom = CUBE_TOP + (CELL_SIZE * CUBE_HEIGHT);

	int source, sourceX, sourceY;
	int invUI = D2CLIENT_GetUIState(UI_INVENTORY);
	int stashUI = D2CLIENT_GetUIState(UI_STASH);
	int cubeUI = D2CLIENT_GetUIState(UI_CUBE);
	if ((invUI || stashUI || cubeUI) && x >= INVENTORY_LEFT && x <= inventoryRight && y >= INVENTORY_TOP && y <= inventoryBottom) {
		source = STORAGE_INVENTORY;
		sourceX = (x - INVENTORY_LEFT) / CELL_SIZE;
		sourceY = (y - INVENTORY_TOP) / CELL_SIZE;
	} else if (stashUI && x >= STASH_LEFT && x <= stashRight && y >= stashTop && y <= stashBottom) {
		source = STORAGE_STASH;
		sourceX = (x - STASH_LEFT) / CELL_SIZE;
		sourceY = (y - stashTop) / CELL_SIZE;
	} else if (cubeUI && x >= CUBE_LEFT && x <= cubeRight && y >= CUBE_TOP && y <= cubeBottom) {
		source = STORAGE_CUBE;
		sourceX = (x - CUBE_LEFT) / CELL_SIZE;
		sourceY = (y - CUBE_TOP) / CELL_SIZE;
	} else {
		return;
	}

	int destination;
	if (ctrlState) {
		destination = STORAGE_NULL;  // this means the ground
	} else if (source == STORAGE_STASH || source == STORAGE_CUBE) {
		destination = STORAGE_INVENTORY;
	} else if (source == STORAGE_INVENTORY && D2CLIENT_GetUIState(UI_STASH)) {
		destination = STORAGE_STASH;
	} else if (source == STORAGE_INVENTORY && D2CLIENT_GetUIState(UI_CUBE)) {
		destination = STORAGE_CUBE;
	} else {
		return;
	}

	bool moveItem = LoadInventory(unit, xpac, source, sourceX, sourceY, destination);

	if (moveItem) {
		*block = true;
		PickUpItem();
	}
}

void ItemMover::OnGamePacketRecv(BYTE* packet, bool* block) {
	switch (packet[0])
	{
	case 0x9c:
		{
			// We get this packet after placing an item in a container or on the ground
			if (FirstInit) {
				BYTE action = packet[1];
				unsigned int itemId = *(unsigned int*)&packet[4];
				Lock();
				if (itemId == ActivePacket.itemId) {
					//PrintText(1, "Placed item id %d", itemId);
					ActivePacket.itemId = 0;
					ActivePacket.x = 0;
					ActivePacket.y = 0;
					ActivePacket.startTicks = 0;
					ActivePacket.destination = 0;
				}
				Unlock();
			}
			break;
		}
	case 0x9d:
		{
			// We get this packet after picking up an item
			if (FirstInit) {
				BYTE action = packet[1];
				unsigned int itemId = *(unsigned int*)&packet[4];
				Lock();
				if (itemId == ActivePacket.itemId) {
					//PrintText(2, "Picked up item id %d", itemId);
					if (ActivePacket.destination == STORAGE_NULL) {
						PutItemOnGround();
					} else {
						PutItemInContainer();
					}
				}
				Unlock();
			}
			break;
		}
	default:
		break;
	}
	return;
}
