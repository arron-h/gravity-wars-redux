//
//  SimpleOGLAppDelegate.m
//  SimpleOGL
//
//  Created by Arron Hartley on 07/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"
#import "EAGLView.h"

@implementation GravWarsAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationWillResignActive:(UIApplication *)application
{
    [glView stopRender];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [glView startRender];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [glView stopRender];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Handle any foreground procedures not related to animation here.
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	//No need for nib
	window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	glView = [[EAGLView alloc] initWithFrame:window.bounds];
	[window addSubview:glView];
	[window makeKeyAndVisible];
	
	[glView startRender];
}

- (void)dealloc
{
    [window release];
	[glView release];
    
    [super dealloc];
}

@end
