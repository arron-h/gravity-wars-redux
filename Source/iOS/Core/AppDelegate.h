//
//  SimpleOGLAppDelegate.h
//  SimpleOGL
//
//  Created by Arron Hartley on 07/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;
@interface GravWarsAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	EAGLView *glView;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) EAGLView* glView;

@end

