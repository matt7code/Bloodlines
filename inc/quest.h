#ifndef QUEST_H_INCLUDED
#define QUEST_H_INCLUDED

struct Quest {
    std::string m_tag;
    int         m_part;
    std::string m_name;
    std::string m_desc;
    bool        m_display;
    bool        m_completed;
};

#endif // QUEST_H_INCLUDED
