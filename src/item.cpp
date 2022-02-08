#include "item.h"

std::string Item::getTypeName(int type) {
    if(type == Item::ItemType_None)         return "None";
    else if(type == Item::ItemType_Wand)    return "Wand";
    else if(type == Item::ItemType_Potion)  return "Potion";
    else if(type == Item::ItemType_Armor)   return "Armor";
    else if(type == Item::ItemType_Weapon)  return "Weapon";
    else if(type == Item::ItemType_Scroll)  return "Scroll";
    else if(type == Item::ItemType_Junk)    return "Junk";
    else if(type == Item::ItemType_Key)     return "Key";
    else if(type == Item::ItemType_Tool)    return "Tool";
    else if(type == Item::ItemType_Valuable)return "Valuable";
    else return "Error";
}

std::string Item::getSubtypeName(int type) {
    if(type == Item::ItemSubType_None)                  return "None";
    else if(type == Item::ItemSubType_Wand)             return "Wand";
    else if(type == Item::ItemSubType_Wand_Lightning)   return "Lightning";
    else if(type == Item::ItemSubType_Wand_Missiles)    return "Missiles";
    else if(type == Item::ItemSubType_Wand_Fireball)    return "Fireball";
    else if(type == Item::ItemSubType_Wand_Cold)        return "Cold";
    else if(type == Item::ItemSubType_Potion)           return "Potion";
    else if(type == Item::ItemSubType_Potion_Healing)   return "Healing";
    else if(type == Item::ItemSubType_Potion_Poison)    return "Poison";
    else if(type == Item::ItemSubType_Potion_Mana)      return "Mana";
    else if(type == Item::ItemSubType_Armor)            return "Armor";
    else if(type == Item::ItemSubType_Armor_Head)       return "Head";
    else if(type == Item::ItemSubType_Armor_Body)       return "Body";
    else if(type == Item::ItemSubType_Armor_Neck)       return "Necklace";
    else if(type == Item::ItemSubType_Armor_Shield)     return "Shield";
    else if(type == Item::ItemSubType_Armor_Ring)       return "Ring";
    else if(type == Item::ItemSubType_Armor_Gloves)     return "Gloves";
    else if(type == Item::ItemSubType_Armor_Belt)       return "Belt";
    else if(type == Item::ItemSubType_Armor_Boots)      return "Boots";
    else if(type == Item::ItemSubType_Weapon)           return "Weapon";
    else if(type == Item::ItemSubType_Weapon_Sword)     return "Sword";
    else if(type == Item::ItemSubType_Weapon_Dagger)    return "Dagger";
    else if(type == Item::ItemSubType_Weapon_Bow)       return "Bow";
    else if(type == Item::ItemSubType_Weapon_Hammer)    return "Hammer";
    else if(type == Item::ItemSubType_Weapon_Staff)                         return "Staff";
    else if(type == Item::ItemSubType_Weapon_2H_Sword)  return "2H Sword";
    else if(type == Item::ItemSubType_Scroll)           return "Scroll";
    else if(type == Item::ItemSubType_Scroll_Healing)   return "Healing";
    else if(type == Item::ItemSubType_Scroll_Teleport)  return "Teleport";
    else if(type == Item::ItemSubType_Scroll_Exit)      return "Exit";
    else if(type == Item::ItemSubType_Scroll_Light)     return "Light";
    else if(type == Item::ItemSubType_Scroll_RemoveCurse) return "Remove Curse";
    else if(type == Item::ItemSubType_Junk)             return "Junk";
    else if(type == Item::ItemSubType_Junk_Large)       return "Large";
    else if(type == Item::ItemSubType_Junk_Medium)      return "Medium";
    else if(type == Item::ItemSubType_Junk_Small)       return "Small";
    else if(type == Item::ItemSubType_Key)              return "Key";
    else if(type == Item::ItemSubType_Key_Magic)        return "Magic";
    else if(type == Item::ItemSubType_Key_Normal)       return "Normal";
    else if(type == Item::ItemSubType_Tool)             return "Tool";
    else if(type == Item::ItemSubType_Tool_Tent)        return "Tent";
    else if(type == Item::ItemSubType_Tool_Shovel)      return "Shovel";
    else if(type == Item::ItemSubType_Valuable)         return "Valuable";
    else if(type == Item::ItemSubType_Valuable_Gold)    return "Gold";
    else if(type == Item::ItemSubType_Valuable_Gem)     return "Gem";
    else if(type == Item::ItemSubType_Valuable_Jewelry) return "Jewelry";
    else return "Error";
}

std::string Item::getSlotName(int type) {
    if(type == Item::Slot_None)        return "None";
    else if(type == Item::Slot_Head)   return "Head";
    else if(type == Item::Slot_Neck)   return "Neck";
    else if(type == Item::Slot_Armor)  return "Body";
    else if(type == Item::Slot_Hand_R) return "Held";
    else if(type == Item::Slot_Hand_L) return "Held";
    else if(type == Item::Slot_Ring_R) return "Ring";
    else if(type == Item::Slot_Ring_L) return "Ring";
    else if(type == Item::Slot_Gloves) return "Hands";
    else if(type == Item::Slot_Belt)   return "Waist";
    else if(type == Item::Slot_Boots)  return "Feet";
    return "Error";
}

