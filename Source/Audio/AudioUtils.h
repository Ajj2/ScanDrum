/*
  ==============================================================================

    AudioUtils.h
    Created: 21 Jul 2017 6:03:52pm
    Author:  Alex Jones

  ==============================================================================
*/

#ifndef AUDIOUTILS_H_INCLUDED
#define AUDIOUTILS_H_INCLUDED

#include <JuceHeader.h>

class Envelope : public stk::Envelope
{
public:
    enum STAGE
    {
        ENV_SUSTAIN,
        ENV_RELEASE,
        ENV_OFF
    };
    
    struct Point
    {
        float x;
        float y;
    };
    
    struct Points : public Point
    {
        Points(float x, float y)
        {
            Point::x = x;
            Point::y = y;
            next = NULL;
        }
        Points()
        {
            delete next;
        }
        
        Points& operator() (float x, float y)
        {
            last().next = new Points(x, y);
            return *this;
        }
        
        Points& last()
        {
            return next ? next->last() : *this;
        }
        
        int count() const
        {
            return next ? 1 + next->count() : 1;
        }
        
        Points* next;
    };
    
    struct Loop
    {
        Loop(int from = -1, int to = -1) : start(from), end(to) {}
        
        void set(int from, int to) { start = from; end = to; }
        void reset() { start = end = -1; }
        
        bool isActive() const { return start != -1 && end != -1; }
        
        int start;
        int end;
    };
    
    Envelope() : stk::Envelope()
    {
        setLoop(0,0);
        sampleRate = 44100;
        set(Points(0.0,1.0), sampleRate);
    }
    
    Envelope(const Points& points) : stk::Envelope()
    {
        set(points, sampleRate);
        setLoop(0,0);
    }
    
    void set(const Points& point, float Sr)
    {
        points.clear();
        
        const Points* pPoint = &point;
        while(pPoint)
        {
            points.push_back(*pPoint);
            pPoint = pPoint->next;
        }
        
        initialise(Sr);
    }
    
    void setLoop(int startPoint, int endPoint)
    {
        if(startPoint >= 0 && endPoint < points.size())
            loop.set(startPoint, endPoint);
    }
    
    void resetLoop()
    {
        loop.reset();
        if(stage == ENV_SUSTAIN && (point+1) < points.size())
            setTarget(points[point+1], points[point].x);
    }
    
    void setStage(STAGE stage){ this->stage = stage; }
    
    const STAGE getStage() const { return stage; }
    
    float getLength() const { return points.size() ? points[points.size() - 1].x : 0.0; }
    
    void release(float time)
    {
        stage = ENV_RELEASE;
        setTime(time);
        stk::Envelope::setTarget(0.0);
    }
    
    void initialise(float Sr)
    {
        point = 0;
        timeInc = 1.0f / Sr;
        loop.reset();
        stage = ENV_SUSTAIN;
        
        if(points.size())
        {
            setValue(points[0].y);
            
            if(points.size() > 1)
                setTarget(points[1], points[0].x);
        }
        else
        {
            stk::Envelope::setValue(1.0);
        }
    }
    
    void resize(int samples)
    {
        float length = getLength();
        if(length == 0.0)
            return;
        
        float multiplier = samples/(sampleRate * length);
        std::vector<Point>::iterator point = points.begin();
        while(point != points.end())
        {
            point->x *= multiplier;
            point++;
        }
        
        initialise(sampleRate);
    }
    
    void setTarget(Point& point, float time = 0.0)
    {
        this->time = time;
        stk::Envelope::setTarget(point.y);
        stk::Envelope::setRate(fabs(point.y - value_) / ((point.x - time) * Stk::sampleRate()));
    }
    
    float tick()
    {
        float amplitude = stk::Envelope::tick();
        
        if (stage == ENV_SUSTAIN)
        {
            time += timeInc;
            if (stk::Envelope::getState() == 0)
            { // envelop segment end reached
                if (loop.isActive() && (point+1) >= loop.end)
                {
                    point = loop.start;
                    stk::Envelope::setValue(points[point].y);
                    if (loop.start != loop.end)
                    {
                        setTarget(points[point+1], points[point].x);
                    }
                }
                else if ((point+1) < points.size())
                {
                    if (time >= points[point+1].x)
                    { // reached target point
                        point++;
                        setValue(points[point].y); // make sure exact value is set
                        
                        if ((point+1) < points.size()) // new target point?
                            setTarget(points[point+1], points[point].x);
                    }
                }
                else
                {
                    stage = ENV_OFF;
                }
            }
        }
        else if (stage == ENV_RELEASE)
        {
            if (amplitude == 0.0)
                stage = ENV_OFF;
        }
        
        return amplitude;
    }
    
    const Point& operator[](int point) const
    {
        return points[point];
    }
    
private:
    std::vector<Point> points;
    Loop loop;
    
    int sampleRate;
    int point;
    float time, timeInc;
    STAGE stage;
};

#endif  // AUDIOUTILS_H_INCLUDED
