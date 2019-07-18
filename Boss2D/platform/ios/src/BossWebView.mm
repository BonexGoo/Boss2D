#import <UIKit/UIKit.h>
#import "BossWebView.h"

@interface BossWebViewController : UIViewController<UIWebViewDelegate>
{
    NSURL* originalUrl;
    int requestCount;
    BOOL clearCache;
    BossWebView::EventCB cb;
    void* data;
    UIWebView* webView;
}
- (void)setUrl:(const char*)text;
@end

@implementation BossWebViewController

- (void)setUrl:(const char*)text cache:(BOOL)clearcache eventCB:(BossWebView::EventCB)c dataParam:(void*)d
{
    originalUrl = [NSURL URLWithString:[NSString stringWithUTF8String:text]];
    requestCount = 0;
    clearCache = (clearcache)? YES : NO;
    cb = c;
    data = d;
}

- (void)loadView
{
    CGRect bound = [UIScreen mainScreen].bounds;
    UIView* view = [[UIView alloc] initWithFrame:bound];
    view.backgroundColor = [UIColor whiteColor];
    view.opaque = YES;
    view.clipsToBounds = YES;
    self.view = view;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    webView = [[UIWebView alloc] initWithFrame:self.view.bounds];
    webView.delegate = self;
    webView.backgroundColor = [UIColor clearColor];
    webView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
    webView.scalesPageToFit = YES;
    webView.contentMode = UIViewContentModeScaleAspectFit;
    webView.opaque = NO;
    [self.view addSubview:webView];
    
    requestCount = 0;
    NSURLRequest* requestURL = nil;
    if(clearCache == YES)
    {
        requestURL = [NSURLRequest requestWithURL:originalUrl cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:10000];
        [[NSURLCache sharedURLCache] removeCachedResponseForRequest:requestURL];
        [[NSURLCache sharedURLCache] removeAllCachedResponses];
        for(NSHTTPCookie* cookie in [[NSHTTPCookieStorage sharedHTTPCookieStorage] cookies])
            [[NSHTTPCookieStorage sharedHTTPCookieStorage] deleteCookie:cookie];
    }
    else requestURL = [NSURLRequest requestWithURL:originalUrl];
    [webView loadRequest:requestURL];
}

- (BOOL)webView:(UIWebView*)webView shouldStartLoadWithRequest:(NSURLRequest*)request navigationType:(UIWebViewNavigationType)navigationType
{
    if(0 < requestCount++)
    {
        NSString* text = request.URL.absoluteString;
        if(cb) cb(data, "UrlChanged", [text UTF8String]);
    }
    return YES;
}

- (void)webViewDidStartLoad:(UIWebView*)webView
{
    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
}

- (void)webViewDidFinishLoad:(UIWebView*)webView
{
    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
}

- (void)webView:(UIWebView*)webView didFailLoadWithError:(NSError*)error
{
    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
}

@end

#include <vector>
static std::vector<BossWebViewController*> gWebViewControllers;

int BossWebView::Create(void* uiview, const char* url, bool clearcache, EventCB cb, void* data)
{
    UIView* CurView = (UIView*) uiview;
    UIViewController* CurViewController = [[CurView window] rootViewController];

    BossWebViewController* WebViewController = [[BossWebViewController alloc] init];
    [WebViewController setUrl:url cache:clearcache eventCB:cb dataParam:data];
    [CurViewController presentViewController:WebViewController animated:YES completion:nil];

    gWebViewControllers.push_back(WebViewController);
    return 0;
}

void BossWebView::Release(int id)
{
    if(BossWebViewController* WebViewController = gWebViewControllers.at(id))
    {
        [WebViewController dismissViewControllerAnimated:YES completion:nil];
        gWebViewControllers.at(id) = nullptr;
    }
}
