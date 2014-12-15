//
//  GDKGameView.m
//  Quake2
//
//  Created by Nick Gravelyn on 10/15/12.
//  Copyright (c) 2012 GDK. All rights reserved.
//

#import "GDKGameView.h"

#define kFrameInterval (1.0 / 60.0)

@interface GDKGameView ()
{
    NSTimer *timer;
}
- (void)tick;
@end

@implementation GDKGameView

- (id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        timer = [NSTimer timerWithTimeInterval:kFrameInterval
                                        target:self
                                      selector:@selector(tick)
                                      userInfo:nil
                                       repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (void)dealloc
{
    [timer invalidate];
}

- (void)tick
{
    if ([super lockFocusIfCanDraw])
    {
        
    }
}

@end
