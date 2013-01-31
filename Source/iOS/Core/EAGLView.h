//
//  EAGLView.h
//  SimpleOGL
//
//  Created by Arron Hartley on 07/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{
@private
    EAGLContext *context;
    
    // The pixel dimensions of the CAEAGLLayer.
    GLint framebufferWidth;
    GLint framebufferHeight;
    
    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
    GLuint defaultFramebuffer, colorRenderbuffer;
	
	BOOL					animating;
    BOOL					displayLinkSupported;
    NSInteger				animationFrameInterval;
	
	CGFloat					fScaleFactor;
	
	// Run loop control
    id						displayLink;
    NSTimer					*animationTimer;
}

- (void)startRender;
- (void)stopRender;

@property (nonatomic, retain) EAGLContext *context;

@end
