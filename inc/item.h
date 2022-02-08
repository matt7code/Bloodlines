#ifndef ITEM_H
#define ITEM_H

#include <string>

class Item {
public:
    enum {Slot_None, Slot_Head, Slot_Neck, Slot_Armor, Slot_Hand_R, Slot_Hand_L, Slot_Ring_R, Slot_Ring_L, Slot_Gloves, Slot_Belt, Slot_Boots};
    enum {ItemType_None = 0, ItemType_Wand = 1, ItemType_Potion = 2, ItemType_Armor = 3,
          ItemType_Weapon = 4, ItemType_Scroll = 5, ItemType_Junk = 6, ItemType_Key = 7,
          ItemType_Tool = 8, ItemType_Valuable = 9
         };
    enum {
        ItemSubType_None = 0,
        ItemSubType_Wand = 100,   ItemSubType_Wand_Lightning, ItemSubType_Wand_Missiles, ItemSubType_Wand_Fireball, ItemSubType_Wand_Cold,
        ItemSubType_Wand_End,
        ItemSubType_Potion = 200, ItemSubType_Potion_Healing, ItemSubType_Potion_Poison, ItemSubType_Potion_Mana,
        ItemSubType_Potion_End,
        ItemSubType_Armor = 300,  ItemSubType_Armor_Head, ItemSubType_Armor_Body, ItemSubType_Armor_Neck, ItemSubType_Armor_Shield, ItemSubType_Armor_Ring, ItemSubType_Armor_Gloves, ItemSubType_Armor_Belt, ItemSubType_Armor_Boots,
        ItemSubType_Armor_End,
        ItemSubType_Weapon = 400, ItemSubType_Weapon_Sword, ItemSubType_Weapon_Dagger, ItemSubType_Weapon_Bow, ItemSubType_Weapon_Hammer, ItemSubType_Weapon_2H_Sword, ItemSubType_Weapon_Staff,
        ItemSubType_Weapon_End,
        ItemSubType_Scroll = 500, ItemSubType_Scroll_Healing, ItemSubType_Scroll_Teleport, ItemSubType_Scroll_Exit, ItemSubType_Scroll_Light, ItemSubType_Scroll_RemoveCurse,
        ItemSubType_Scroll_End,
        ItemSubType_Junk = 600,   ItemSubType_Junk_Large, ItemSubType_Junk_Medium, ItemSubType_Junk_Small,
        ItemSubType_Junk_End,
        ItemSubType_Key = 700,    ItemSubType_Key_Magic, ItemSubType_Key_Normal,
        ItemSubType_Key_End,
        ItemSubType_Tool = 800,   ItemSubType_Tool_Tent, ItemSubType_Tool_Shovel,
        ItemSubType_Tool_End,
        ItemSubType_Valuable = 900,  ItemSubType_Valuable_Gold, ItemSubType_Valuable_Gem, ItemSubType_Valuable_Jewelry,
        ItemSubType_Valuable_End
    };

    Item(int id = -1) {
        itemID = id;
    };
    virtual ~Item() {};

    static std::string getTypeName(int type);
    static std::string getSubtypeName(int type);
    static std::string getSlotName(int type);

public:
    unsigned    itemID      = 0;
    unsigned    stackable   = 0;
    unsigned    quest       = 0;
    unsigned    equipSlot   = Slot_None;
    unsigned    type        = ItemType_None;
    unsigned    subtype     = ItemSubType_None;
    unsigned    cost        = 0;
    float       value1      = 0;
    float       value2      = 0;
    float       value3      = 0;
    float       value4      = 0;
    std::string name        = "New Item";
    std::string desc        = "Edit this description.";
};

#endif // ITEM_H
