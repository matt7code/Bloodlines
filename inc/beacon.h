#ifndef BEACON_H
#define BEACON_H

/*
    T       Type    m_Type  Position                v1      v2      v3      v4                  t1      t2
    -1 - None
    00 - Inactive
    01 - Combat     Type    PosX    PosY    PosZ    Radius  Freq    Diff    GpMult  XpMult
    02 - Search     Type    PosX    PosY    PosZ    Radius  Item    Count   Quality HowMany     First   Exhausted
    03 - Location   Type    PosX    PosY    PosZ    Radius  Backgro Weather HPChangeMPChange                                  Area    Country?
*/

class Beacon {
public:
    enum            {NONE, INACTIVE, COMBAT, SEARCH, LOCATION};
    Beacon()        {}
    virtual         ~Beacon() {}

    int             getType()       {
        return m_Type;
    }
    sf::Vector3i    getPosition()   {
        return m_Pos;
    }
    float           getValue1()     {
        return m_Value1;
    }
    float           getValue2()     {
        return m_Value2;
    }
    float           getValue3()     {
        return m_Value3;
    }
    float           getValue4()     {
        return m_Value4;
    }
    float           getValue5()     {
        return m_Value5;
    }
    std::string     getText1()      {
        return m_Text1;
    }
    std::string     getText2()      {
        return m_Text2;
    }

    void            setType(int val)    {
        m_Type = val;
    }
    void            setPosition( sf::Vector3i val ) {
        m_Pos = val;
    }
    void            setValue1(float val)  {
        m_Value1 = val;
    }
    void            setValue2(float val)  {
        m_Value2 = val;
    }
    void            setValue3(float val)  {
        m_Value3 = val;
    }
    void            setValue4(float val)  {
        m_Value4 = val;
    }
    void            setValue5(float val)  {
        m_Value5 = val;
    }
    void            setText1( std::string val )     {
        m_Text1 = val;
    }
    void            setText2( std::string val )     {
        m_Text2 = val;
    }

private:
    int             m_Type   = 0;
    sf::Vector3i    m_Pos    = sf::Vector3i(102400, 64000, 0);

    float           m_Value1 = 0;  // lileyhood of combat per 1000 steps
    float           m_Value2 = 0;  // Levels brought against you
    float           m_Value3 = 0;
    float           m_Value4 = 0;
    float           m_Value5 = 0;
    std::string     m_Text1  = "";
    std::string     m_Text2  = "";
};

#endif // BEACON_H
