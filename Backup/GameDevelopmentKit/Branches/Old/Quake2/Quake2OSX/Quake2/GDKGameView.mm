//
//  GDKGameView.m
//  Quake2
//
//  Created by Nick Gravelyn on 10/15/12.
//  Copyright (c) 2012 GDK. All rights reserved.
//

#import "GDKGameView.h"

#import <Precomp.h>
#import <Platform.h>
#import <Game.h>
#import <Quake2Game.h>
#import <ContentArchive.h>

#define kFrameInterval (1.0 / 60.0)

namespace GDK
{
    namespace Graphics
    {
        extern GDK::Graphics::IGraphicsDevice * CreateGraphicsDeviceForOSX(const GDK::Graphics::GraphicsDeviceCreationParameters &createParams);
    }
}

class GameHost : public GDK::RuntimeObject<GDK::IGameHost>
{
public:
};

@interface GDKGameView ()
{
    NSTimer *timer;

    GameHost *host;
    GDK::Graphics::IGraphicsDevice *graphicsDevice;
    GDK::IContent *content;
    GDK::IGame *game;
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

        try
        {        
            host = GDK::Make<GameHost>().Detach();

            GDK::Graphics::GraphicsDeviceCreationParameters deviceParams;
            deviceParams.backBufferWidth = (int)frameRect.size.width;
            deviceParams.backBufferHeight = (int)frameRect.size.height;
            deviceParams.vsyncEnabled = true;
            deviceParams.windowIdentity = (void *)self;
            graphicsDevice = CreateGraphicsDeviceForOSX(deviceParams);

            content = GDK::Make<ContentArchive>();

            GDK::GameCreationParameters gameParams;
            gameParams.host = host;
            gameParams.graphicsDevice = graphicsDevice;
            gameParams.content = content;
            game = GDK::Make<Quake2::Quake2Game>(gameParams, false).Detach();
        }
        catch (const GDK::Exception &ex)
        {
            DebugOut(L"Error initializing game: %ls", ex.GetMessage());
            exit(-1);
        }
    }
    return self;
}

- (void)dealloc
{
    host->Release();
    graphicsDevice->Release();
    game->Release();

    [timer invalidate];
    [super dealloc];
}

- (void)tick
{
    game->Update(kFrameInterval);

    if ([super lockFocusIfCanDraw])
    {
        game->Draw(GDK::Matrix::Identity(), GDK::Matrix::Identity());
    }
}

@end
