#include "../../JuceLibraryCode/JuceHeader.h"
#include "Cursor.h"
#include "Osc.h"

using namespace juce;
using namespace std;

Cursor::Cursor(int index, const juce::String& name, const int& updateRate):m_name(name) , m_updateRate(updateRate), m_index (index)
{

}

void Cursor::initialise (float pos[], float speed[], 
                         const long& duration, std::vector<cv::Point>& points, float avBrightness)
{
    for(int i=0;i<3;++i) 
    {
        m_position[i]=pos[i];                                     
        m_speed[i]=speed[i];
    }
    m_time=0;
    m_duration=duration;
    m_avBrightness = avBrightness;
    
    //cout<<"init "<<m_name<<" with "<<points.size()<<" points"<<endl;

    Osc* osc = Osc::getInstance();
    //clear vertices
    osc->sendOSCMessage("/revil/spaces/space/"+m_name+"/clear_vertices",0);
    //add all vertices
    for(unsigned int i = 2; i < points.size(); i++)
    {
        osc->sendOSCMessage("/revil/spaces/space/"+m_name+"/add_vertex", 
                   points[0].x, points[0].y, 0);
        osc->sendOSCMessage("/revil/spaces/space/"+m_name+"/add_vertex", 
                   points[i-1].x, points[i-1].y, 0);
        osc->sendOSCMessage("/revil/spaces/space/"+m_name+"/add_vertex", 
                   points[i].x, points[i].y, 0);
        /*
        cout<<"adding points "<<points[0].x<<" "<<points[0].y<<endl;
        cout<<"adding points "<<points[i-1].x<<" "<<points[i-1].y<<endl;
        cout<<"adding points "<<points[i].x<<" "<<points[i].y<<"\n"<<endl; */
    }
    //initialize position
    osc->sendOSCMessage("/revil/spaces/space/"+m_name+"/position",
                        pos[0], pos[1], pos[2]);
}

bool Cursor::update(const long& time) 
{
  bool done=false;
  m_time+=time;
  if(m_time>m_duration) 
  {
    done=true;
  }
  else 
  {
    for(int i=0; i<3; ++i) 
    {
        m_position[i]+=m_speed[i]; 
    }
    //send new position
    Osc* osc = Osc::getInstance();
    osc->sendOSCMessage("/revil/spaces/space/"+m_name+"/position",
                        m_position[0], m_position[1], m_position[2]);

  }
  return done;
}
