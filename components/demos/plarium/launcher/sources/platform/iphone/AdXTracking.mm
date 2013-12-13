/*
 * @name: AdXTracking.m
 *
 * @created: 2013-04-12
 * @author: Paul Hayton, Ad-X Ltd
 * @copyright: Copyright 2011-2013 Ad-X Ltd All rights reserved
 * @description: Ad-X iOS SDK
 */

#define DEBUG 1

#if defined(DEBUG) && DEBUG
#define AdXLog(FORMAT, args...)	NSLog(FORMAT , ## args)
#else
#define AdXLog(FORMAT, args...)
#endif

#define OLD_DEVICE_ID [self Odin1];

// Imports
#import "AdXTracking.h"
#import <SystemConfiguration/SCNetworkReachability.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <sys/xattr.h>
#import <CommonCrypto/CommonDigest.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UIDevice.h>
#import <UIKit/UIPasteboard.h>
#import <AdSupport/ASIdentifierManager.h>

@implementation AdXTracking

@synthesize ClientId;
@synthesize URLScheme;
@synthesize AppleId;
@synthesize UserAgent;
@synthesize BundleID;
@synthesize IDFV;
@synthesize advertisingIdentifier;
@synthesize OptOut;

#define IFT_ETHER 0x6

#define SDK_VERSION @"4.0.3"

#define SERVER @"https://ad-x.co.uk"
#define QSERVER @"http://ad-x.co.uk"
//#define SERVER @"https://hk.ad-x.org"
//#define QSERVER @"http://hk.ad-x.org"

#define CONNECTION_TIMEOUT 4.0

- (void)sendEvent:(NSString*)event withData:(NSString*)data {
    NSMutableArray *arr1 = [[NSMutableArray alloc] initWithCapacity: 2];
    [arr1 addObject:event];
    [arr1 addObject:data];
    [self performSelectorInBackground:@selector(reportAppEventToAdX:) withObject:arr1];
    [arr1 release];
	
}

- (void)sendEvent:(NSString*)event withData:(NSString*)data andCurrency:(NSString*)currency {
    
    NSMutableArray *arr1 = [[NSMutableArray alloc] initWithCapacity: 3];
    [arr1 addObject:event];
    [arr1 addObject:data];
    [arr1 addObject:currency];
    [self performSelectorInBackground:@selector(reportAppEventToAdX:) withObject:arr1];
    [arr1 release];
}

- (void)sendEvent:(NSString*)event withData:(NSString*)data andCurrency:(NSString*)currency andCustomData:(NSString*)custom {
    
    NSMutableArray *arr1 = [[NSMutableArray alloc] initWithCapacity: 3];
    [arr1 addObject:event];
    [arr1 addObject:data];
    [arr1 addObject:currency];
    [arr1 addObject:custom];
    [self performSelectorInBackground:@selector(reportAppEventToAdX:) withObject:arr1];
    [arr1 release];
}

- (id)init {
    self = [super init];
    if (self) {
        is_upgrade = FALSE;
        Class advMan = NSClassFromString(@"ASIdentifierManager");
        currentElement = nil;
        BundleID = nil;
        connectionStartTime = nil;
        self.advertisingIdentifier = @"";
        UAwebView = nil;
        referral  = nil;
        clickID = nil;
        UserAgent = nil;
        OptOut = NO;
        seencount = -1;
        advertisingTrackingEnabled = NO;
        iOS6 = (advMan == NULL) ? NO : YES;
        self.IDFV = @"";
        
        if (advMan) {
            identifierManager = (ASIdentifierManager*)[advMan sharedManager];
            advertisingTrackingEnabled = identifierManager.advertisingTrackingEnabled;
            self.advertisingIdentifier = identifierManager.advertisingIdentifier.UUIDString; // Use setter to retain

            NSUUID *uuid = [[UIDevice currentDevice] identifierForVendor];
            if (uuid != nil)
                self.IDFV = [uuid UUIDString];
        }
	}
    return self;
}

- (NSString*) AdXGetBundleID {
    
    if (BundleID && [BundleID length] > 0)
        return BundleID;
    return [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleIdentifier"];
}

- (NSString*)GetReferral {
    
    NSString *referralStr = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.url"];
    if (referralStr == NULL) return NULL;
    
    return referralStr;
}
- (NSString*)GetDLReferral {
    
    NSString *referralStr = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.DLReferral"];
    if (referralStr == NULL) return NULL;
    
    return referralStr;
}
- (int)IsFirstInstall {
    if (seencount > 0) return 0;
    if (seencount < 0) return -1;
    return 1;
}
- (NSString*)GetTransactionID {
    return clickID;
}

- (NSString*)GetOldDID
{
    // If we're stored a previous device ID from old version use it.
    NSString *oldDID = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.UDID"];
    if (oldDID) return oldDID;
    oldDID = [[NSUserDefaults standardUserDefaults] objectForKey:@"UDID"];
    if (oldDID) return oldDID;

    // Otherwise default to whatever we were previously using.
    return OLD_DEVICE_ID;

}

- (NSString*) GetAdXDeviceIDForEvents {
    
    NSString *isu;

    if (iOS6)
	{
        // As this is not just conversion tracking, we need to check whether we're allowed to use the advertisingIdentifier
        if (advertisingTrackingEnabled)
            isu = advertisingIdentifier;
        else
            isu = @"ANON";
	}
    else {
        // We're not running on iOS 6. Use the Old Device ID for now.
        isu = [self GetOldDID];
    }
    
    return isu;
}


/*
 * Record if this is an upgrade. Writes to Userdefaults on first call. Then uses that value on subsequent calls.
 */
- (void)isUpgrade:(BOOL)isUpgrade {
    NSNumber *isAnUpgrade = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.isUpgrade"];
    if (isAnUpgrade == NULL)
	{
        NSNumber *upgrade = [NSNumber numberWithBool:isUpgrade];
        [[NSUserDefaults standardUserDefaults] setObject:upgrade forKey:@"Ad-X.isUpgrade"];
        [[NSUserDefaults standardUserDefaults] synchronize];
        is_upgrade = isUpgrade;
	}
    else
        is_upgrade = [isAnUpgrade boolValue];
}

- (void)dealloc
{
	[URLScheme release];
	[ClientId release];
	[AppleId release];
	if (UserAgent)              [UserAgent release];
    if (connectionStartTime)    [connectionStartTime release];
	if (currentElement)         [currentElement release];
    if (referral)               [referral release];
    if (clickID)                [clickID release];
    
    if (UAwebView) {
        // Set delegate to nil
        UAwebView.delegate = nil;
        [UAwebView release];
    }
	[super dealloc];
}

- (NSString *) macAddress
{
    char* macAddressString = (char*)malloc(18);
    
    int  success;
    struct ifaddrs * addrs;
    struct ifaddrs * cursor;
    const struct sockaddr_dl * dlAddr;
    const unsigned char* base;
    int i;
    
    success = getifaddrs(&addrs) == 0;
    if (success) {
        cursor = addrs;
        while (cursor != 0) {
            if ( (cursor->ifa_addr->sa_family == AF_LINK)
                && (((const struct sockaddr_dl *) cursor->ifa_addr)->sdl_type == IFT_ETHER) && strcmp("en0",  cursor->ifa_name)==0 ) {
                dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
                base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
                strcpy(macAddressString, "");
                for (i = 0; i < dlAddr->sdl_alen; i++) {
                    if (i != 0) {
                        strcat(macAddressString, ":");
                    }
                    char partialAddr[3];
                    sprintf(partialAddr, "%02X", base[i]);
                    strcat(macAddressString, partialAddr);
                    
                }
            }
            cursor = cursor->ifa_next;
        }
        
        freeifaddrs(addrs);
    }
    
    NSString* macAddress= [[[NSString alloc] initWithCString:macAddressString
                                                    encoding:NSMacOSRomanStringEncoding] autorelease];
    free (macAddressString);
    
    return macAddress;
}

- (NSString *) Odin1
{
    char* macAddressString = (char*)malloc(18);
    
    int  success;
    struct ifaddrs * addrs;
    struct ifaddrs * cursor;
    const struct sockaddr_dl * dlAddr;
    const unsigned char* base = NULL;
    int i;
    NSMutableString* output = [NSMutableString stringWithCapacity:CC_SHA1_DIGEST_LENGTH * 2];
    
    success = getifaddrs(&addrs) == 0;
    if (success) {
        cursor = addrs;
        while (cursor != 0) {
            if ( (cursor->ifa_addr->sa_family == AF_LINK)
                && (((const struct sockaddr_dl *) cursor->ifa_addr)->sdl_type == IFT_ETHER) && strcmp("en0",  cursor->ifa_name)==0 ) {
                dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
                base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
                strcpy(macAddressString, "");
                for (i = 0; i < dlAddr->sdl_alen; i++) {
                    if (i != 0) {
                        strcat(macAddressString, ":");
                    }
                    char partialAddr[3];
                    sprintf(partialAddr, "%02X", base[i]);
                    strcat(macAddressString, partialAddr);
                    
                }
            }
            cursor = cursor->ifa_next;
        }
        
        // Check for success
        if (base != NULL) {
            uint8_t digest[CC_SHA1_DIGEST_LENGTH];
            CC_SHA1(base, 6, digest);
		
            for(i = 0; i < CC_SHA1_DIGEST_LENGTH; i++)
                [output appendFormat:@"%02x", digest[i]];
		} else {
            [output setString:@""];
        }
        
        freeifaddrs(addrs);
    }
    
    free(macAddressString);
    return output;
}


- (void)reportAppOpenToAdXNow {
    [self reportAppOpenToAdX:true];
}


- (void)reportAppOpenToAdX:(bool)now {
    NSString *isu;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // we're in a new thread here, so we need our own autorelease pool
	// Have we already reported an app open?
	NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
																		NSUserDomainMask, YES) objectAtIndex:0];
	NSString *appOpenPath = [documentsDirectory stringByAppendingPathComponent:@"adx_app_open"];
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSError *parseError = nil;

    
    /*
     * Dont report app open until we've got the referral information.
     */
    NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyFound"];
    if(!now && ![fileManager fileExistsAtPath:deviceKeyPath])
    {
	    [pool release];
        return;
    }
    
    if(![fileManager fileExistsAtPath:appOpenPath]) {
		// Not yet reported -- report now
		NSString *appname    = [self AdXGetBundleID];
		NSString *appversion = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
		
        if (iOS6)
		{
            isu = advertisingIdentifier;
		}
        else
		{
            // We're not running on iOS 6. Use the Old Device ID for now.
            isu = [self GetOldDID];
        }
		
        NSString *old_id   = [self GetOldDID];
		
        NSString *macAddress = [self macAddress];
        
		NSString *appOpenEndpoint = [NSString stringWithFormat:@"%@/atrk/iOSapp?isu=%@&app_id=%@&app_name=%@&app_version=%@&clientid=%@&tag_version=%@&macAddress=%@&oldDID=%@&idfv=%@&ate=%d",
                                     SERVER, isu, AppleId, appname, appversion, ClientId, SDK_VERSION, macAddress, old_id, IDFV, advertisingTrackingEnabled];


        AdXLog(@"%@",appOpenEndpoint);
		
		NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:appOpenEndpoint]];
		NSURLResponse *response;
		NSError *ierror = nil;
		NSData *responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&ierror];
		NSString *respStr = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
        AdXLog(@"HTTP response %d %@",[(NSHTTPURLResponse *)response statusCode],respStr);
		if((!ierror) && ([(NSHTTPURLResponse *)response statusCode] == 200) ) {
            [self parseResponse:responseData error:&parseError];
		}
		[respStr release];
	}
	[pool release];
	
    NSNotification *n = [NSNotification notificationWithName:@"AdXDone" object:self];
    [[NSNotificationCenter defaultCenter] postNotification:n];
}

- (NSString *) urlEncode:(NSString *) input
{
    NSString * encoded = (NSString *)CFURLCreateStringByAddingPercentEscapes(NULL,
	   (CFStringRef)input,
	   NULL,
	   (CFStringRef)@"!*'();:@&=+$,/?%#[]",
	   kCFStringEncodingUTF8 );
	   
	return [encoded autorelease];
}

- (void)reportAppEventToAdX:(NSArray *)eventAndDataReference {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // we're in a new thread here, so we need our own autorelease pool
	// Have we already reported an app open?
    NSError *parseError = nil;
    referral = [[NSMutableString alloc] init];

    NSString *appname    = [self AdXGetBundleID];
    
    NSString *event = [eventAndDataReference objectAtIndex:0];
    NSString *data  = [eventAndDataReference objectAtIndex:1];
    NSString *currency = ([eventAndDataReference count] > 2) ? [eventAndDataReference objectAtIndex:2] : @"";
    NSString *extraData = ([eventAndDataReference count] > 3) ? [eventAndDataReference objectAtIndex:3] : @"";
    
    NSString *isu;
    if (iOS6)
	{
        // As this is not just conversion tracking, we need to check whether we're allowed to use the advertisingIdentifier
        if (advertisingTrackingEnabled)
            isu = advertisingIdentifier;
        else
            isu = @"ANON";
	}
    else {
        // We're not running on iOS 6. Use the Old Device ID for now.
        isu = [self GetOldDID];
    }
    NSString *old_id   = [self GetOldDID];
    NSString *macAddress = [self macAddress];
	NSString *deviceType = [self urlEncode:[UIDevice currentDevice].model];
	NSString *iOSVersion = [self urlEncode:[[UIDevice currentDevice] systemVersion]];

    // If no tracking enabled, anonymise the Device
    
    NSString *appOpenEndpoint = [NSString stringWithFormat:@"%@/API/event/%@/%@/%@/%@/%@/%@?macAddress=%@&oldDID=%@&dev=%@&os=%@&extraData=%@&idfv=%@",
                                 SERVER, ClientId, isu, appname, event, data, currency, macAddress, old_id,deviceType,iOSVersion,extraData,IDFV];
				
	
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:appOpenEndpoint]];
    NSURLResponse *response;
    NSError *ierror = nil;
    NSData *responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&ierror];

    NSString *respStr = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
    AdXLog(@"HTTP response %d %@",[(NSHTTPURLResponse *)response statusCode],respStr);
    if((!ierror) && ([(NSHTTPURLResponse *)response statusCode] == 200) ) {
        [self parseResponse:responseData error:&parseError];
    }

    [pool release];
}

- (BOOL) connectedToNetwork
{
    // Create zero addy
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;
	
    // Recover reachability flags
    SCNetworkReachabilityRef defaultRouteReachability = SCNetworkReachabilityCreateWithAddress(NULL, (struct sockaddr *)&zeroAddress);
    SCNetworkReachabilityFlags flags;
	
    BOOL didRetrieveFlags = SCNetworkReachabilityGetFlags(defaultRouteReachability, &flags);
    CFRelease(defaultRouteReachability);
	
    if (!didRetrieveFlags)
    {
        //printf("Error. Could not recover network reachability flags\n");
        return 0;
    }
	
    BOOL isReachable = flags & kSCNetworkFlagsReachable;
    BOOL needsConnection = flags & kSCNetworkFlagsConnectionRequired;
    BOOL nonWiFi = flags & kSCNetworkReachabilityFlagsTransientConnection;
    return ((isReachable && !needsConnection) || nonWiFi) ?
    (([[[NSURLConnection alloc] initWithRequest:[NSURLRequest
												 requestWithURL: [NSURL URLWithString:SERVER]
												 cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:20.0]
									   delegate:self] autorelease]) ? YES : NO) : NO;
}

- (BOOL)recentSwish
{
    NSNumber *deviceKeyInProgress = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.DeviceKeyInProgress"];
    // If we started a swich already within the last 5 minutes, don't repeat...
    if (deviceKeyInProgress != NULL && [deviceKeyInProgress longValue]+300 > [[NSDate date] timeIntervalSince1970])
        return TRUE;
    return FALSE;
}

- (int)swishAttempts
{
    NSNumber *attempts = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.SwishAttempts"];
    if (attempts != NULL) return [attempts intValue];
    return 0;
}

-(BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    // Get the user agent from the request
    UserAgent = [request valueForHTTPHeaderField:@"User-Agent"];
    [UserAgent retain];

    // Now we've got the User agent continue with the connection to Ad-X.
    [self performSelectorInBackground:@selector(doReportAppOpen) withObject:nil];

	// Return no, we don't care about executing an actual request so cancel it.
    return NO;
}

- (void)reportAppOpen{
    
    // See if we have the user-agent already stored.
    NSString *s = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.UserAgent"];
    
    if (s != NULL) {

        UserAgent = s;
        [UserAgent retain];

        // We have the user agent already so send the data to Ad-X. Otherwise this will happen when the user agent is available in shouldStartLoadWithRequest.
        [self performSelectorInBackground:@selector(doReportAppOpen) withObject:nil];
        
    } else {
        
        // Create a webview; we'll read the user-agent in shouldStartLoadWithRequest and continue from there.
        UAwebView = [[UIWebView alloc] init]; // release in dealloc
        [UAwebView setDelegate:self];
        [UAwebView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://www.google.com"]]];
    }
}

- (void)doReportAppOpen
{
	NSError *parseError = nil;
    NSString *isu;
    referral = [[NSMutableString alloc] init];
    clickID  = [[NSMutableString alloc] init];
    
    // We've done the swish, just report the App open to Ad-X.
    NSNumber *n = [[NSUserDefaults standardUserDefaults] objectForKey:@"Ad-X.SwishDone"];
    if ((n != NULL) && [n boolValue] == true)
	{
        [self reportAppOpenToAdX:TRUE];
        return;
	}
    
    if ([self recentSwish]) return;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // we're in a new thread here, so we need our own autorelease pool
    
	// Record the time we're starting so we can decide is a swish is ok.
    if (connectionStartTime) [connectionStartTime release];
    connectionStartTime = [[NSDate alloc] init];
    
    NSString *appname    = [self AdXGetBundleID];
    NSString *macAddress = [self macAddress];
	
    //NSString *odin1 = [self Odin1];
    NSString *old_id   = [self GetOldDID];
    NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
    UIPasteboard *pb = [UIPasteboard
                        pasteboardWithName:@"fb_app_attribution"
                        create:NO];
    NSString *attributionID = @"";
    if (pb) attributionID = pb.string;
    
    if (iOS6)
	{
        isu      = advertisingIdentifier;
	}
    else
    {
        // We're not running on iOS 6. Use the Old Device ID and save it so we can migrate later.
        isu = [self GetOldDID];
        [[NSUserDefaults standardUserDefaults] setObject:isu forKey:@"Ad-X.UDID"];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
	
    
	NSString * encodedUAgent = [self urlEncode:UserAgent];
	NSString *deviceType = [self urlEncode:[UIDevice currentDevice].model];
	NSString *iOSVersion = [self urlEncode:[[UIDevice currentDevice] systemVersion]];
    
    NSString *appOpenEndpoint = [NSString stringWithFormat:@"%@/atrk/iOSAppOpen?isu=%@&clientID=%@&app_name=%@&app_id=%@&tag_version=%@&macAddress=%@&uagent=%@&version=%@&upgrade=%d&fb=%@&oldDID=%@&dev=%@&os=%@&idfv=%@&ate=%d&optout=%d",
                                 QSERVER, isu, ClientId, appname, AppleId, SDK_VERSION, macAddress, encodedUAgent,version,is_upgrade,attributionID,old_id,deviceType,iOSVersion,IDFV,advertisingTrackingEnabled,OptOut];
	
	
    AdXLog(@"%@",appOpenEndpoint);
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:appOpenEndpoint]];
    NSURLResponse *response;
    NSError *ierror = nil;
    NSData *responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&ierror];
    
    NSString *respStr = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
    AdXLog(@"HTTP response %d %@",[(NSHTTPURLResponse *)response statusCode],respStr);
    if((!ierror) && ([(NSHTTPURLResponse *)response statusCode] == 200) ) {
        [self parseResponse:responseData error:&parseError];
    }
    else {
        NSNotification *notification = [NSNotification notificationWithName:@"AdXDone" object:self];
        [[NSNotificationCenter defaultCenter] postNotification:notification];
    }
    [respStr release];
    [pool release];
    
}

- (BOOL)doSwish
{
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
																		NSUserDomainMask, YES) objectAtIndex:0];
    NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyFound"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    // If we've got the referral don't do it again.
    if([fileManager fileExistsAtPath:deviceKeyPath])
        return TRUE;
	
    if ([self recentSwish]) return TRUE;
    int attempts = [self swishAttempts];
    if (attempts > 3) return TRUE;
	
    NSNumber *newattempts;
    if (attempts == 0) newattempts = [NSNumber numberWithInt:1];
    else               newattempts = [NSNumber numberWithInt:attempts+1];
	
	
    NSString *appname    = [self AdXGetBundleID];
    NSString *odin1      = [self Odin1];
	
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/atrk/Rconv.php?tag=%@&appid=%@&adxid=%@&idfa=%@",SERVER,URLScheme, appname, odin1, advertisingIdentifier]];
    
    if ([self connectedToNetwork])
    {
        // Save the time and number of attempts.
        NSNumber *ts = [NSNumber numberWithLong:[[NSDate date] timeIntervalSince1970]];
        [[NSUserDefaults standardUserDefaults] setObject:ts forKey:@"Ad-X.DeviceKeyInProgress"];
        [[NSUserDefaults standardUserDefaults] setObject:newattempts forKey:@"Ad-X.SwishAttempts"];
        [[NSUserDefaults standardUserDefaults] synchronize];
		
        if (![[UIApplication sharedApplication] openURL:url]) {
            AdXLog(@"%@%@",@"Failed to open url:",[url description]);
        }
        return TRUE;
    }
    return TRUE;
}

-(BOOL)addSkipBackupAttributeToItemAtURL:(NSURL *)URL
{
    
    const char* filePath = [[URL path] fileSystemRepresentation];
    const char* attrName = "com.apple.MobileBackup";
    if (&NSURLIsExcludedFromBackupKey == nil) {
        // iOS 5.0.1 and lower
        u_int8_t attrValue = 1;
        int result = setxattr(filePath, attrName, &attrValue, sizeof(attrValue), 0, 0);
        return result == 0;
        
    }
    else {
        // First try and remove the extended attribute if it is present
        int result = getxattr(filePath, attrName, NULL, sizeof(u_int8_t), 0, 0);
        if (result != -1) {
            // The attribute exists, we need to remove it
            int removeResult = removexattr(filePath, attrName, 0);
            if (removeResult == 0) {
                NSLog(@"Removed extended attribute on file %@", URL);
            }
        }
        
        // Set the new key
        NSError *error = nil;
        [URL setResourceValue:[NSNumber numberWithBool:YES] forKey:NSURLIsExcludedFromBackupKey error:&error];
        return error == nil;
    }
}

/**
 Ad-X third party is going out to Safari and Safari is starting our application through a URL.
 This mechanism is handled here.
 */
- (BOOL)handleOpenURL:(NSURL *)url
{
	NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
																		NSUserDomainMask, YES) objectAtIndex:0];
    NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyFound"];
    
    //if (!url) {  return NO; }
    
    /* Retrieve the ADX UDID */
	/*    NSString *urlquery = [url absoluteString];
	 NSMutableDictionary *params = [[NSMutableDictionary alloc] init];
	 for (NSString *param in [urlquery componentsSeparatedByString:@"&"]) {
	 NSArray *elts = [param componentsSeparatedByString:@"="];
	 if([elts count] < 2) continue;
	 [params setObject:[elts objectAtIndex:1] forKey:[elts objectAtIndex:0]];
	 }
	 [params release];    */
    NSString *URLString = [url absoluteString];
    // Save the information to UserDefaults.
    [[NSUserDefaults standardUserDefaults] setObject:URLString forKey:@"Ad-X.adxurl"];
    [[NSUserDefaults standardUserDefaults] synchronize];
    
    
    
    // Create a marker file to prevent us doing this each time.
	NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager createFileAtPath:deviceKeyPath contents:nil attributes:nil]; // successful report, mark it as such
    [self addSkipBackupAttributeToItemAtURL:[NSURL fileURLWithPath:deviceKeyPath]];
    
    AdXLog(@"Got the URL as %@",URLString);
    [self performSelectorInBackground:@selector(reportAppOpenToAdXNow) withObject:nil];
    return YES;
}

/*
 * Take the XML Puzzle String and parse it.
 */
- (BOOL)parseResponse:(NSData*)data error:(NSError **)error
{
    NSXMLParser *parser = [[NSXMLParser alloc] initWithData:data];
    // Set self as the delegate of the parser so that it will receive the parser delegate methods callbacks.
    [parser setDelegate:self];
    // Depending on the XML document you're parsing, you may want to enable these features of NSXMLParser.
    [parser setShouldProcessNamespaces:NO];
    [parser setShouldReportNamespacePrefixes:NO];
    [parser setShouldResolveExternalEntities:NO];
    
    [parser parse];
    
    NSError *parseError = [parser parserError];
    if (parseError && error) {
        *error = parseError;
        [parser release];
        return FALSE;
    }
    
    [parser release];
    return TRUE;
}


- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict
{
    if (qName) {
        elementName = qName;
    }
	
    if (currentElement)
    {
        [currentElement release];
        currentElement = nil;
    }
	currentElement = elementName;
    [currentElement retain];
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{
    
    if ([currentElement isEqualToString:@"Referral"])
	{
        AdXLog(@"Got Referral from server %@",referral);
        // Save the information to UserDefareportults.
        [[NSUserDefaults standardUserDefaults] setObject:referral forKey:@"Ad-X.url"];
        [[NSUserDefaults standardUserDefaults] synchronize];
	}
    else if ([currentElement isEqualToString:@"DLReferral"])
	{
        AdXLog(@"Got Referral from server %@",referral);
        // Save the information to UserDefareportults.
        [[NSUserDefaults standardUserDefaults] setObject:referral forKey:@"Ad-X.DLReferral"];
        [[NSUserDefaults standardUserDefaults] synchronize];
	}
}

- (void)parser:(NSXMLParser *)parser foundCDATA:(NSData *)CDATABlock
{
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
																		NSUserDomainMask, YES) objectAtIndex:0];
    NSString *appOpenPath = [documentsDirectory stringByAppendingPathComponent:@"adx_app_open"];
    
    if ([currentElement isEqualToString:@"Referral"]) {
        [referral appendString:string];
	}
    if ([currentElement isEqualToString:@"DLReferral"]) {
        [referral appendString:string];
	}
    if ([currentElement isEqualToString:@"Seencount"]) {
        seencount = [string intValue];
	}
    if ([currentElement isEqualToString:@"ClickID"]) {
        [clickID appendString:string];
	}
    if ([currentElement isEqualToString:@"Success"])
	{
        if ([string compare:@"true"] == NSOrderedSame || [string compare:@"stop"] == NSOrderedSame) {
            [fileManager createFileAtPath:appOpenPath contents:nil attributes:nil]; // successful report, mark it as such
		    [self addSkipBackupAttributeToItemAtURL:[NSURL fileURLWithPath:appOpenPath]];
        }
        // We've had a response back from an App Open - set swish to no if not already done.
        NSNumber *n = [NSNumber numberWithBool:true];
        [[NSUserDefaults standardUserDefaults] setObject:n forKey:@"Ad-X.SwishDone"];
        [[NSUserDefaults standardUserDefaults] synchronize];
		
        AdXLog(@"Success is %@",string);
	}
    if ([currentElement isEqualToString:@"Swish"])
	{
        NSDate *secondDate = [NSDate date];
        NSTimeInterval interval = [secondDate timeIntervalSinceDate:connectionStartTime];
        if ([string compare:@"true"] == NSOrderedSame)
		{
            if (interval < CONNECTION_TIMEOUT)
			{
                AdXLog(@"Swish is a go - %f seconds",interval);
                [self doSwish];
			}
            else
			{
                AdXLog(@"Connection took %f seconds - defer",interval);
                NSNotification *n = [NSNotification notificationWithName:@"AdXDone" object:self];
                [[NSNotificationCenter defaultCenter] postNotification:n];
			}
		}
        else
		{
            // Swish is 'no' so record it.
            NSNumber *n = [NSNumber numberWithBool:true];
            [[NSUserDefaults standardUserDefaults] setObject:n forKey:@"Ad-X.SwishDone"];
            [[NSUserDefaults standardUserDefaults] synchronize];
			
            AdXLog(@"Ok - ready to send to Ad-X");
            // Mark the swish information as found so we do send Open notifications.
            NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"Ad-X.DeviceKeyFound"];
            [fileManager createFileAtPath:deviceKeyPath contents:nil attributes:nil]; // successful report, mark it as such
			[self addSkipBackupAttributeToItemAtURL:[NSURL fileURLWithPath:deviceKeyPath]];
            
            NSNotification *n2 = [NSNotification notificationWithName:@"AdXDone" object:self];
            [[NSNotificationCenter defaultCenter] postNotification:n2];
		}
	}
	
}


@end
