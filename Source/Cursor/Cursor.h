#ifndef CURSOR_H
#define CURSOR_H

#include <string>
#include <iostream> 

class Cursor {
    private:
        std::string m_name;
        long m_time;
        long m_duration;
        float m_position[3];
        float m_speed[3];
        int numFartingHorses; //just one i think 

    public:
        Cursor(const std::string name);
        void initialise (float pos[], float speed[], long& duration);
        bool update(const long& time);
    
};

#endif
