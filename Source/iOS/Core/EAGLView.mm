//
//  EAGLView.m
//  SimpleOGL
//
//  Created by Arron Hartley on 07/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "EAGLView.h"
#include "stdafx.h"
#include "App.h"

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
- (void)setFramebuffer;
- (BOOL)presentFramebuffer;
@end

@implementation EAGLView
@synthesize context;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

/*!***********************************************************************
 @Function		initWithFrame
 @Access		private 
 @Param			
 @Returns		void
 @Description	
 *************************************************************************/
- (id)initWithFrame:(CGRect)frame 
{
	if ((self = [super initWithFrame:frame])) 
    {
		EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		self.context = aContext;
		[aContext release];
		
		[self setFramebuffer];
		
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
		CGSize windowSize;
		windowSize = [eaglLayer bounds].size;
		
		fScaleFactor = 1.0f;
		float ver = [[[UIDevice currentDevice] systemVersion] floatValue];
		if(ver >= 3.2)                            // Check for Retina
			{
			UIScreen* mainscr = [UIScreen mainScreen];
			fScaleFactor = [mainscr scale];
			windowSize.width  = mainscr.bounds.size.width  * fScaleFactor;   // iPhone 4 reports bounds as 320x480, so we need to apply the scale factor that it reports to get 640x960
			windowSize.height = mainscr.bounds.size.height * fScaleFactor;
			}
		
		// Round out
		windowSize.width  = roundf(windowSize.width);
		windowSize.height = roundf(windowSize.height);
		
		if((int)fScaleFactor == 2)
			{
			// Set contentScale Factor to 2
			self.contentScaleFactor = 2.0;
			
			// Also set our glLayer contentScale Factor to 2
			eaglLayer.contentsScale = 2;
			}
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
										
		animating = FALSE;
		displayLinkSupported = FALSE;
		animationFrameInterval = 1;			// Max Render Speed
		displayLink = nil;
		animationTimer = nil;
		
		// Use of CADisplayLink requires iOS version 3.1 or greater.
		// The NSTimer object is used as fallback when it isn't available.
		NSString *reqSysVer = @"3.1";
		NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
		if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
			displayLinkSupported = TRUE;
			
		
		GetApp()->Initialise(windowSize.height, windowSize.width, "Hello");									
    }
    
    return self;
}

/*!***********************************************************************
 @Function		dealloc
 @Access		private 
 @Param			
 @Returns		void
 @Description	
 *************************************************************************/
- (void)dealloc
{
    [self deleteFramebuffer];    
	
	// Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	
	GetApp()->OnDestroy();
	
    [context release];
		    
    [super dealloc];
}

/*!***********************************************************************
 @Function		context
 @Access		private 
 @Param			
 @Returns		EAGLContext*
 @Description	
 *************************************************************************/
- (EAGLContext *)context
{
    return context;
}

/*!***********************************************************************
 @Function		setContext
 @Access		private 
 @Param			EAGLContext*
 @Returns		void
 @Description	
 *************************************************************************/
- (void)setContext:(EAGLContext *)newContext
{
    if (context != newContext)
    {
        [self deleteFramebuffer];
        
        [context release];
        context = [newContext retain];
        
        [EAGLContext setCurrentContext:nil];
    }
}

/*!***********************************************************************
 @Function		createFramebuffer
 @Access		private 
 @Param			
 @Returns		void
 @Description	
 *************************************************************************/
- (void)createFramebuffer
{
    if (context && !defaultFramebuffer)
    {
        [EAGLContext setCurrentContext:context];
        
        // Create default framebuffer object.
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        // Create color render buffer and allocate backing store.
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

/*!***********************************************************************
 @Function		deleteFramebuffer
 @Access		private 
 @Param			
 @Returns		void
 @Description	
 *************************************************************************/
- (void)deleteFramebuffer
{
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        if (defaultFramebuffer)
        {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        if (colorRenderbuffer)
        {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
    }
}

/*!***********************************************************************
 @Function		setFramebuffer
 @Access		private 
 @Param			
 @Returns		void
 @Description	
 *************************************************************************/
- (void)setFramebuffer
{
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        if (!defaultFramebuffer)
            [self createFramebuffer];
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    }
}

/*!***********************************************************************
 @Function		presentFramebuffer
 @Access		private 
 @Param			
 @Returns		void
 @Description	Swaps buffers
 *************************************************************************/
- (BOOL)presentFramebuffer
{
    BOOL success = FALSE;
    
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        
        success = [context presentRenderbuffer:GL_RENDERBUFFER];
    }
    
    return success;
}

/*!***********************************************************************
 @Function		layoutSubviews
 @Access		private 
 @Param			
 @Returns		void
 @Description	
 *************************************************************************/
- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    [self deleteFramebuffer];
}

/*!***********************************************************************
 @Function		startRender
 @Access		private 
 @Param			
 @Returns		void
 @Description	Starts Rendering
 *************************************************************************/
- (void)startRender
{
    if (!animating)
    {
		// Try and use new Display Link
        if (displayLinkSupported)
        {
            displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawFrame)];
            [displayLink setFrameInterval:animationFrameInterval];
            
            // The run loop will retain the display link on add.
            [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        }
		// Fall back to NSTimer
        else
            animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawFrame) userInfo:nil repeats:TRUE];
        
        animating = TRUE;
    }
}

/*!***********************************************************************
 @Function		stopRender
 @Access		private 
 @Param			
 @Returns		void
 @Description	Halts rendering
 *************************************************************************/
- (void)stopRender
{
    if (animating)
    {
        if (displayLinkSupported)
        {
            [displayLink invalidate];
            displayLink = nil;
        }
        else
        {
            [animationTimer invalidate];
            animationTimer = nil;
        }
        
        animating = FALSE;
    }
}

/*!***********************************************************************
 @Function		drawFrame
 @Access		private 
 @Param			
 @Returns		void
 @Description	Called from NSTimer or Display Link (Main Render Loop)
 *************************************************************************/
- (void)drawFrame
{
    [(EAGLView *)self setFramebuffer];
	
	GetApp()->OnUpdate();
	GetApp()->OnRender();
    
    [(EAGLView *)self presentFramebuffer];
}


/*!***********************************************************************
 @Function		touchesEnded
 @Access		private 
 @Param			NSSet*
 @Returns		void
 @Description	
 *************************************************************************/
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch* pTouch = [touches anyObject];
	CGPoint touchLoc = [pTouch locationInView:self];
	
	// iPhone is rotated on the side. Rotate the touch coords to reflect this.
	float fY = touchLoc.x * fScaleFactor;
	float fX = ([self bounds].size.height - touchLoc.y) * fScaleFactor;
	
	Touch newTouch = { fX, fY };
	GetApp()->OnTouchDown(&newTouch, 1);
}

/*!***********************************************************************
 @Function		touchesEnded
 @Access		private 
 @Param			NSSet*
 @Returns		void
 @Description	
 *************************************************************************/
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch* pTouch = [touches anyObject];
	CGPoint touchLoc = [pTouch locationInView:self];
	
	float fY = touchLoc.x * fScaleFactor;
	float fX = ([self bounds].size.height - touchLoc.y) * fScaleFactor;
	
	Touch newTouch = { fX, fY };	
	GetApp()->OnTouchMoved(&newTouch, 1);
}

/*!***********************************************************************
 @Function		touchesEnded
 @Access		private 
 @Param			NSSet*
 @Returns		void
 @Description	
 *************************************************************************/
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch* pTouch = [touches anyObject];
	CGPoint touchLoc = [pTouch locationInView:self];
	
	float fY = touchLoc.x * fScaleFactor;
	float fX = ([self bounds].size.height - touchLoc.y) * fScaleFactor;
	
	Touch newTouch = { fX, fY };	
	GetApp()->OnTouchUp(&newTouch, 1);
}


@end
