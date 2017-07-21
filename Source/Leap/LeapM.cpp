/*
  ==============================================================================

    LeapM.cpp
    Created: 19 Jul 2017 1:48:07pm
    Author:  Alex Jones

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "LeapM.h"



//==============================================================================
LeapM::LeapM(Osc& oscRef_) : oscRef(oscRef_), sendPosition (0)
{
    addAndMakeVisible(textDisplay);
    textDisplay.setText("leap motion input data will appear here .. ");
    textDisplay.setColour(TextEditor::backgroundColourId, Colours::azure.interpolatedWith(Colours::lightgrey, 0.8));
    textDisplay.setColour(TextEditor::textColourId, Colours::black);
    textDisplay.setFont(Font(40));
    textDisplay.setMultiLine(true);

    startTimerHz(60);
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
    textDisplay.clear();
    if (!controller.isConnected())
    {
        textDisplay.setText("No Leap Conntected");
    }
    
    //extract video
    
    controller.setPolicy(Leap::Controller::POLICY_IMAGES);
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
    
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
        
        String mess="/exprsui/input/video/brightness";
        std::vector<float> values(1,meanValue[0]);
        
        //threshold
        cv::Mat threshImg;
        cv::threshold(leapImg, threshImg, 230, 255,
                      cv::THRESH_BINARY);
        
        imshow("Leap", threshImg);
        cvWaitKey(10);
        
        //contours points
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(threshImg, contours, CV_RETR_EXTERNAL,
                         CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
        for(unsigned int c=0; c<contours.size(); c++)
        {
            if(contours[c].size()>100)
            {
                std::vector<std::vector<cv::Point> > hull(contours.size());
                convexHull(cv::Mat(contours[c]), hull[c], false);
                cv::drawContours(leapImg, hull, c,
                                 cv::Scalar(255));
                std::vector<float> vals;
                for(int v=0; v<hull[c].size(); ++v)
                {
                    vals.push_back(hull[c][v].x);
                    vals.push_back(hull[c][v].y);
                }
                
                //oscRef.sendOSCMessage("/revil/spaces/space/cursor1/clear_vertices", 1);
                
                if (sendPosition.get() == 1)
                {
                    for (int i = 2; i < vals.size(); i++)
                    {
                        oscRef.sendOSCMessage("/revil/spaces/space/cursor1/add_vertex", vals[0], vals[1], 0);
                    }
                    sendPosition = 0;
                }
            }
        }
    }
}

void LeapM::actionListenerCallback (const String &message)
{
    if (message == "onsetDetected")
    {
        sendPosition = true;
    }
}
