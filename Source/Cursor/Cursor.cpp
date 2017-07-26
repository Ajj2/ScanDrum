#include "Cursor.h"

Cursor(const std::string name):m_name(name) {

}

void Cursor::initialise (float pos[], float speed[], long& duration) {
    for(int i=0;i<3;++i) {
        m_position[i]=pos[i];                                     
        m_speed[i]=speed[i];
    }
    m_time=0;
    m_duration=duration;
}

bool Cursor::update(const long& time) {
  bool done=false;
  m_time+=time;
  if(m_time>m_duration) {
    done=true;
  }
  else {
    for(int i=0; i<3; ++i) {
        // this should be 'i', no ?
        m_position[i]+=m_speed[i]; 
    }
  }
  return done;
}
