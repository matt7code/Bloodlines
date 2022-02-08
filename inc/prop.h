#ifndef PROP_H_INCLUDED
#define PROP_H_INCLUDED

#include <SFML/Graphics.hpp>

class Prop {
public:
    enum {
        ANIM_END,
        ANIM_LOOP,
        ANIM_OSCILLATE_FORWARD,
        ANIM_OSCILLATE_BACKWARD,
        ANIM_MAX
    };

    Prop() {};
    virtual ~Prop() {};

    sf::Sprite&                     getSprite() { return m_Sprite; }
    bool                            getActive() const { return m_Active; }
    bool                            getAnimationRunning() const { return m_AnimationRunning; }
    std::string                     getAnimationName() const { return m_AnimationName; }
    unsigned                        getAnimationIndex() const { return m_AnimIndex; }
    unsigned                        getAnimationStyle() const { return m_AnimStyle; }
    bool                            getAnimated() const { return m_Animated; }
    unsigned                        getAnimCurrentFrame() const { return m_currentFrame; }
    unsigned                        getAnimTotalFrames() const { return m_Sprites.size(); }
    unsigned                        getAnimMode() const { return m_animMode; }
    float                           getAnimFrameTime() const { return m_frameTime; }
    std::vector<sf::Sprite>&        getAnimSprites() { return m_Sprites; }
    sf::Time                        getPrevAnimTime() const { return m_prevAnimatedTime; }

    void                            setSprite(sf::Sprite val) { m_Sprite = val; }
    void                            setActive(bool val) { m_Active = val; }
    void                            setAnimationRunning(bool val) { m_AnimationRunning = val; }
    void                            setAnimationName(std::string val) { m_AnimationName = val; }
    void                            setAnimationIndex(unsigned val) { m_AnimIndex = val; }
    void                            setAnimationStyle(unsigned val) { m_AnimStyle = val; }

    void                            setAnimated(bool val) { m_Animated = val; }
    void                            setAnimCurrentFrame(unsigned val) { m_currentFrame = val; }
    void                            setAnimMode(unsigned val) { m_animMode = val; }
    void                            setAnimFrameTime(float val) { m_frameTime = val; }
    void                            setPrevAnimTime(sf::Time val) { m_prevAnimatedTime = val; }

private:
    sf::Sprite                      m_Sprite;
    std::vector<sf::Sprite>         m_Sprites;
    bool                            m_Active            = false;
    bool                            m_AnimationRunning  = false;
    std::string                     m_AnimationName     = "None";
    unsigned                        m_AnimIndex         = 0;
    unsigned                        m_AnimStyle         = ANIM_END;
    bool                            m_Animated          = false;
    unsigned                        m_currentFrame      = 0;
    unsigned                        m_animMode          = ANIM_END;
    float                           m_frameTime         = 0.033333f;
    sf::Time                        m_prevAnimatedTime  = sf::Time::Zero;
};

#endif // PROP_H_INCLUDED
