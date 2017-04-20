/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#if HELIO_AUDIOBUS_SUPPORT

#include "AudiobusOutput.h"
#import <Foundation/Foundation.h>
#import "Audiobus.h"

@interface AudiobusInstance : NSObject
+ (id)instance;
@property (strong, nonatomic) ABAudiobusController *audiobusController;
@property (strong, nonatomic) ABSenderPort *sender;
@end

@implementation AudiobusInstance

#pragma mark Singleton Methods

+ (id)instance
{
    static AudiobusInstance *sharedMyManager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedMyManager = [[self alloc] init];
    });
    return sharedMyManager;
}

- (id)init
{
    if (self = [super init])
    {
        self.audiobusController = [[ABAudiobusController alloc] initWithApiKey:@"MCoqKkhlbGlvKioqaGVsaW8tMS42LjAuYXVkaW9idXM6Ly8qKipbYXVyaS5oZWxpLmhvc3QuMV0=:ZS3OA2MvCmy25mJVocBy8H443wwd1L+G++kyLZeXxI39ZxxQuBqBaWhBEh9ZHuGTIN9wD4IfQfW5BvoCOd0XFxSPP4BXoNg043yI0iY0ISs5FOyEuIHDkF5jM9fc2U+h"];
        self.audiobusController.connectionPanelPosition = ABConnectionPanelPositionTop;
        
        self.sender = [[ABSenderPort alloc] initWithName:@"Audio Output"
                                                   title:NSLocalizedString(@"Helio Audio Output", @"")
                               audioComponentDescription:(AudioComponentDescription)
                       {
                           .componentType = kAudioUnitType_RemoteInstrument,
                           .componentSubType = 'host',
                           .componentManufacturer = 'heli'
                       }];
        
        [self.audiobusController addSenderPort:self.sender];
    }
    
    return self;
}

- (void)shutdown
{
    if (self.sender != nil)
    {
        [self.audiobusController removeSenderPort:self.sender];
    }
}

@end

void AudiobusOutput::init()
{
    [AudiobusInstance instance];
}

// TODO
void AudiobusOutput::process(const float **inputChannelData,
                             int numInputChannels,
                             int numSamples)
{
    // Now send audio through Audiobus
    //ABSenderPortSend([[AudiobusInstance instance] sender], inputChannelData, numSamples, [AudioTimeStamp now]);
    
    // Now mute, if appropriate
    //if (ABSenderPortIsMuted(self->_sender))
    //{
    //    // If we should be muted, then mute
    //    for ( int i=0; i<ioData->mNumberBuffers; i++ ) {
    //        memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize);
    //    }
    //}
}

void AudiobusOutput::shutdown()
{
    [[AudiobusInstance instance] shutdown];
}

#endif // HELIO_AUDIOBUS_SUPPORT
