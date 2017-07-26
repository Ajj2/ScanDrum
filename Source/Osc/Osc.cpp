/*
  ==============================================================================

    Osc.cpp
    Created: 19 Jul 2017 1:22:35pm
    Author:  Alex Jones

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Osc.h"

//==============================================================================
Osc::Osc()
{
    const int port = 9001;
    
    if(!connect(port))
    {
        DBG("Error connecting to port " << port);
    }
    else
    {
        DBG("Connecting to port " << port);
    }
    addListener(this, "/scanDrum/test/");
    
    // Sending port --> 8327
    
    sender.connect("127.0.0.1", 8327);
}

Osc::~Osc(){}

void Osc::oscMessageReceived (const OSCMessage& message)
{
    DBG(message.getAddressPattern().toString() << " --> ");
    
    if (message.size() == 1 && message[0].getType() == OSCTypes::float32)
    {
        DBG (message[0].getFloat32());
    }
    else
    {
        DBG ("Cannot read message");
    }
}

void Osc::sendOSCMessage (String ap, float val)
{
    sender.send(ap, val);
}

void Osc::sendOSCMessage (String ap, float val, float val2)
{
    sender.send(ap, val, val2);
}

void Osc::sendOSCMessage (String ap, float val, float val2, float val3)
{
    sender.send(ap, val, val2, val3);
}