#import <UIKit/UIWebView.h>

#import <Foundation/Foundation.h>

@class ASIdentifierManager;

@interface AdXTracking : NSObject <NSXMLParserDelegate, UIWebViewDelegate> {

	ASIdentifierManager *identifierManager;
    NSString *currentElement;
    NSMutableString *referral;
    NSMutableString *clickID;
    NSMutableString *retadxudid;
    NSString *URLScheme;
    NSString *BundleID;
    NSDate *connectionStartTime;
    NSString *UserAgent;
    NSString *IDFV;
    BOOL is_upgrade;
    BOOL iOS6;
    NSString *advertisingIdentifier;
    BOOL advertisingTrackingEnabled;
    UIWebView *UAwebView;
    BOOL OptOut;
    int  seencount;
}

@property (nonatomic, retain) NSString* URLScheme;
@property (nonatomic, retain) NSString* ClientId;
@property (nonatomic, retain) NSString* AppleId;
@property (nonatomic, retain) NSString* UserAgent;
@property (nonatomic, retain) NSString* BundleID;
@property (nonatomic, retain) NSString* IDFV;
@property (nonatomic, retain) NSString* advertisingIdentifier;
@property (nonatomic) BOOL OptOut;

- (void)reportAppOpenToAdXNow;
- (void)reportAppOpenToAdX:(bool)now;

/**
 3 : Tracking Events.

 Usage :
 
 [tracking sendEvent:@"Sale" withData:@"2.50"];
 
 */

- (void)sendEvent:(NSString*)event withData:(NSString*)data;
    
- (void)sendEvent:(NSString*)event withData:(NSString*)data andCurrency:(NSString*)currency;

- (void)sendEvent:(NSString*)event withData:(NSString*)data andCurrency:(NSString*)currency andCustomData:(NSString*)custom;

- (void)reportAppEventToAdX:(NSArray *)eventAndDataReference;

- (void)reportAppOpen;

- (BOOL)handleOpenURL:(NSURL *)url;

- (BOOL)parseResponse:(NSData*)data error:(NSError **)error;

- (BOOL)recentSwish;

- (NSString *) Odin1;

- (NSString *) macAddress; 

- (void)isUpgrade:(BOOL)isUpgrade;

- (NSString*) GetReferral;

- (NSString*) GetDLReferral;

- (NSString*) GetAdXDeviceIDForEvents;

- (int)IsFirstInstall;

- (NSString*) GetTransactionID;

@end
