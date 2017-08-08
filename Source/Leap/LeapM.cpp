/*
  ==============================================================================

    LeapM.cpp
    Created: 19 Jul 2017 1:48:07pm
    Author:  Alex Jones

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "LeapM.h"

using namespace juce;


//==============================================================================
LeapM::LeapM(Audio& audioRef_) : audioRef(audioRef_), gotOnset (0)
{
    addAndMakeVisible(textDisplay);
    textDisplay.setText("leap motion input data will appear here .. ");
    textDisplay.setColour(TextEditor::backgroundColourId, Colours::azure.interpolatedWith(Colours::lightgrey, 0.8));
    textDisplay.setColour(TextEditor::textColourId, Colours::black);
    textDisplay.setFont(Font(40));
    textDisplay.setMultiLine(true);

    const int updateRate = 60;
    for (int i  = 0; i < maxNumCursors; i++)
    {
        String name = "cursor" + (String)i;
        inactiveCursors.push_front(new Cursor(i, name, updateRate));
    }

    m_prevTime = 0;
    m_currentTime = 0;
     
    controller.setPolicy(Leap::Controller::POLICY_IMAGES);
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

    startTimerHz(updateRate);
}


LeapM::~LeapM()
{
    
}

void LeapM::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);
}

void LeapM::resized()
{
    textDisplay.setBounds(getLocalBounds().reduced(5, 5));
}

void LeapM::timerCallback()
{
    // Not sure if it accurate enough or not to do it like this. 

    m_prevTime = m_currentTime;
    m_currentTime = Time::getMillisecondCounterHiRes();
    double timeElapsed = m_currentTime - m_prevTime;
  
    textDisplay.clear();
    if (!controller.isConnected())
    {
        textDisplay.setText("No Leap Conntected");
    }
    else
    {
        textDisplay.setText("Getting Data from Leap");
    }
    
    //extract video
   
    cv::namedWindow("Leap", CV_WINDOW_NORMAL);
    Leap::Image img = controller.images()[0];
    
    if(img.isValid())
    {
        cv::Mat leapImg = cv::Mat::zeros(cv::Size(img.width(),
                                                  img.height()),
                                         CV_8UC1);
        const unsigned char* image_buffer = img.data();
        int coord=0;
        for(int j=0; j<leapImg.rows; ++j)
        {
            for(int i=0; i<leapImg.cols; ++i)
            {
                char& pix = leapImg.at<char>(j,i);
                pix=image_buffer[coord];
                coord++;
            }
        }
        
        //avg brightness
        cv::Scalar meanValue = cv::mean(leapImg);
        
        //String mess="/exprsui/input/video/brightness";
        std::vector<float> values(1,meanValue[0]);
        
        //threshold
        cv::Mat threshImg;
        cv::threshold(leapImg, threshImg, 230, 255,
                      cv::THRESH_BINARY);
        
        imshow("Leap", threshImg);
        cvWaitKey(10);
        
        //avg brightness
        cv::Scalar avBri = cv::mean(threshImg);
        
        //contours points
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(threshImg, contours, CV_RETR_EXTERNAL,
                         CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
        for(unsigned int c=0; c<contours.size() && c<1; c++)
        {
            
//            if(contours[c].size()>100)
//            {
                std::vector<std::vector<cv::Point> > hull(contours.size());
                convexHull(cv::Mat(contours[c]), hull[c], false);
                cv::drawContours(leapImg, hull, c,
                                 cv::Scalar(255));
                
                //center points
                for(int p=0; p<hull[c].size(); ++p) {
                    hull[c][p].x = hull[c][p].x-img.width()/2;
                    hull[c][p].y = hull[c][p].y-img.height()/2;
                }
                        
                if(gotOnset.get() == 1)
                {
                    // If the inactive cursor array still has something in it
                    if(inactiveCursors.size() != 0)
                    {   // Copy one from it's front and add it to the back of the active cursor array
                        activeCursors.push_back(inactiveCursors.front());
                        // Remove the one that was copied from the inactive array
                        inactiveCursors.pop_front();
                    }
                    else // Else, if the inactive array is empty
                    {   // Copy the oldest one from the active array (the front) and move it to the back
                        activeCursors.push_back(activeCursors.front());
                        // Remove the one that was copied
                        activeCursors.pop_front();
                    }
                    // initialise the new or moved one
                    float pos[3]={0,0,0};
                    float speed[3]={0,0,10};
                    activeCursors.back()->initialise(pos, speed, 2000, hull[c], (float)meanValue[0]);
                    
                    audioRef.newCursorCreated(activeCursors.back()->getIndex(), activeCursors.back()->getName(), avBri[0]);
                    
                    gotOnset = 0;
                }
//            }
        }
    }
    for (int i = 0; i < activeCursors.size(); i++)
    {
        if (activeCursors[i]->update(timeElapsed))
        {
            inactiveCursors.push_back(activeCursors[i]);
            activeCursors.erase(activeCursors.begin()+i);
        }
    }
}

void LeapM::actionListenerCallback (const String &message)
{
    if (message == "onsetDetected")
    {
        gotOnset = true;
    }
}
