

#import <Foundation/Foundation.h>


@interface Tracking : NSObject <NSXMLParserDelegate> {
    NSString *currentElement;
    NSMutableString *referral;
    NSMutableString *retadxudid;
    NSString *URLScheme;
    NSDate *connectionStartTime;
    NSString *UserAgent;
    BOOL is_upgrade;
    BOOL iOS6;
}

@property (nonatomic, retain) NSString* URLScheme;
@property (nonatomic, retain) NSString* ClientId;
@property (nonatomic, retain) NSString* AppleId;
@property (nonatomic, retain) NSString* UserAgent;


/**
 Step 1 : Add the SDK to your project.
 
 1. Add Tracking.m and Tracking.h into your project
 
 2. Add the Tracking object into your application delegate AppDelegate.h 
 
 Tracking *tracker;
 
 
 3. Add the following 2 functions into your application Delegate
 // Replace URLSCHEME with your applications unique URL Scheme ID from AD-X
 // Replace CLIENTID with your clientID from AD-X
 // Replace APPLEID with the Apple ID of your application from iTunes Connect
 
 - (void)reportAppOpen
 {
 tracker = [[Tracking alloc] init];
 [tracker setURLScheme:@"ADXnnnn"];
 [tracker setClientId:@"CLIENTID"];
 [tracker setAppleId:@"APPLEID"];
 [tracker reportAppOpenToAdX];
 }
 
 - (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
 {
 return [tracker application:application handleOpenURL:url];
 }
 
 
 4. Add the following line into your applicationWillEnterForeground so that the tracking code is executed.
 
 - (void)applicationWillEnterForeground:(UIApplication *)application {
 
 [self performSelectorInBackground:@selector(reportAppOpen) withObject:nil];
 
 }
 
 
 
 5. It should also be called from within applicationDidFinishLaunching or 
 didFinishLaunchingWithOptions (depending on how you have chosen to start their app):
 // Replace URLSCHEME with your applications unique URL Scheme ID from AD-X
 // Replace CLIENTID with your clientID from AD-X
 // Replace APPLEID with the APPLE ID of your application from iTunes Connect
 
 - (void)applicationDidFinishLaunching:(UIApplication *)application
 {
 tracker = [Tracking alloc];
 [tracker setURLScheme:@"ADXnnnn"];
 [tracker setClientId:@"CLIENTID"];
 [tracker setAppleId:@"APPLEID"];
 if (![tracker getReferral]) 
 [self performSelectorInBackground:@selector(reportAppOpen) withObject:nil];
 

*/

- (void)reportAppOpenToAdXNow;
- (void)reportAppOpenToAdX:(bool)now;

/**
 3 : Tracking Events.

 Usage :
 
 [tracking sendEvent:@"Sale" withData:@"2.50"];
 
 */

- (void)sendEvent:(NSString*)event withData:(NSString*)data;
    
- (void)sendEvent:(NSString*)event withData:(NSString*)data andCurrency:(NSString*)currency;

- (void)reportAppEventToAdX:(NSArray *)eventAndDataReference;

- (void)reportAppOpen;

- (BOOL)handleOpenURL:(NSURL *)url;

- (BOOL)parseResponse:(NSData*)data error:(NSError **)error;

- (BOOL)recentSwish;

- (NSString *) Odin1;

- (NSString *) macAddress; 

- (void)isUpgrade:(BOOL)isUpgrade;

@end
