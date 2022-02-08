#include "party.h"

void Party::modPartyGold(int val) {
    std::cout << "Previous Gold: " << m_gold << " After Modification: " << m_gold + val << std::endl;
    m_gold += val;
}


void Party::Update(float delta) {
    for(auto& member : party) member.AnimateWalk(delta);

    /// Detect if step has been taken...
    /// Handle statues and step numbers....
    if (Tool::dist(getPos(0), getLastStepLocation()) > STEP_DISTANCE) {
        for(auto it = party.begin(); it != party.end(); ++it) {
            if(it->isActive() && it->isAlive()) it->UpdateStatus();
        }
        m_steps++;
        setLastStepLocation(getPos(0));
    }
}

void Party::Draw(sf::RenderWindow& window) {
    for(auto it = party.rbegin(); it != party.rend(); ++it) {
        window.draw(it->getSprite());
    }
}

void Party::recordMove(sf::Vector2f mov) {
    m_moveRecord.push_front(mov);

    for (unsigned int i = 0; i < party.size(); ++i) {
        if(m_moveRecord.size() > i * PARTY_LAG_STEPS) {
            party[i].SetFacingByVector(m_moveRecord[i * PARTY_LAG_STEPS]);
            party[i].getSprite().move(m_moveRecord[i * PARTY_LAG_STEPS]);
        }
    }

    // Cull record size.
    while(m_moveRecord.size() > ((party.size() - 1) * PARTY_LAG_STEPS)) m_moveRecord.pop_back();
}


void Party::RemoveMember() {
    if(party.size() == 1) return;
    /// @TODO
    // Default to last member for now
    if (!validPartySlot(party.size() - 1)) {
        std::cerr << "Party::RemoveMember - Error: Invalid Party Slot." << std::endl;
        return;
    }
    party.resize(party.size() - 1);

    while(m_moveRecord.size() > (party.size() - 1) * PARTY_LAG_STEPS && !m_moveRecord.empty())
        m_moveRecord.pop_back();
}

sf::Vector2f Party::getPos(int mem_num) {
    if (validPartySlot(mem_num)) return (party.begin() + mem_num)->getSprite().getPosition();
    else std::cerr << "Party::getPos - Error: Invalid Party Slot." << std::endl;
    return NEW_GAME_START_POSITION;
}

bool Party::validPartySlot(int mem_num) {
    if (mem_num < 0 || (unsigned)(mem_num + 1) > party.size()) {
        std::cerr << "Invalid Party Slot" << std::endl;
        return false;
    }
    return true;
}

void Party::AddMember(int avatar, int level, int my_class, std::string name) {
    avatar %= 40;
    if (avatar < 0) avatar = 0;

    Hero p(level, my_class);
    p.setName(name);
    if(!texture.loadFromFile("graphics/characters.png"))
        std::cerr << "Problem loading graphics/characters.png" << std::endl;

    p.getSprite().setTexture(texture);
    p.setAvatar(avatar);

    p.getTextureSheetBase().x = ((avatar % NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_X * CHARACTER_TILE_X);
    p.getTextureSheetBase().y = ((avatar / NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_Y * CHARACTER_TILE_Y);

    p.getSprite().setTextureRect(sf::IntRect(p.getTextureSheetBase().x, p.getTextureSheetBase().y, TILE_SIZE_X, TILE_SIZE_Y));
    p.getSprite().setOrigin(16, 30);
    //p.getSprite().setOrigin(16, 16);

    if(party.empty()) {
        p.getSprite().setPosition(NEW_GAME_START_POSITION);
    } else {
        p.getSprite().setPosition((party.end() - 1)->getSprite().getPosition());
    }
    p.setCurHP(p.getMaxHP());
    p.setCurMP(p.getMaxMP());

    party.push_back(p);
}

void Party::MovePartyTo(sf::Vector2f pos) {
    for(auto& p : party) {
        p.getSprite().setPosition(pos);
    }
    m_moveRecord.clear();
}

std::string Party::getSavedValue(const std::string& key) {
    auto it = m_saved_values.find(key);
    if (it == m_saved_values.end()) return "NOT_FOUND";
    return it->second;
}

void Party::debugSavedValues() {
    std::cout << "Key:          Value:" << std::endl;
    for(const auto& it : m_saved_values) {
        std::cout << it.first << "\t\t" << it.second << std::endl;
    }
}

void Party::setSavedValue(const std::string& key, const std::string& val) {
    m_saved_values[key] = val;
}

void Party::deleteQuestValue(const std::string& key) {
    auto it = m_saved_values.find(key);
    if (it == m_saved_values.end()) return;
    else m_saved_values.erase(it);
}
