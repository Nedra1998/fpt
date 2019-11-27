#include <Carbon/Carbon.h>
const float kBlackColor[] = {0.0, 0.0, 0.0, 1.0};
const float kRedColor[] = {1.0, 0.0, 0.0, 1.0};
const float kWhiteColor[] = {
    1.0, 1.0, 1.0,
    1.0};  // Handy routine for centering some text on the origin.
void CenterTextInContextAtOrigin(CGContextRef cgContext, const char* fontName,
                                 float textSize, const char* text) {
  CGContextSaveGState(cgContext);
  CGContextSelectFont(
      cgContext, fontName, textSize,
      kCGEncodingMacRoman);  // Save the GState again for the text drawing mode
  CGContextSaveGState(cgContext);
  CGContextSetTextDrawingMode(cgContext, kCGTextInvisible);
  CGContextShowTextAtPoint(cgContext, 0, 0, text, strlen(text));
  CGPoint destination = CGContextGetTextPosition(cgContext);
  CGContextRestoreGState(cgContext);  // Draw the text
  CGContextSetFillColor(cgContext, kWhiteColor);
  CGContextShowTextAtPoint(cgContext, -destination.x / 2.0, -textSize / 2, text,
                           strlen(text));
  CGContextRestoreGState(cgContext);
}
int main(int argc, char* argv[]) {
  const int kImageSize = 512;
  const int kHalfSize = kImageSize / 2;
  const CFIndex kBytesPerRow = (CFIndex)kImageSize * 4;
  const CFIndex kDataLength = kBytesPerRow * kImageSize;
  CGColorSpaceRef kGenericRGBColorSpace =
      CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
  UInt8* bitmapData = (UInt8*)malloc(kDataLength);
  CGContextRef cgContext = CGBitmapContextCreate(
      bitmapData, kImageSize, kImageSize, 8, kBytesPerRow,
      kGenericRGBColorSpace,
      kCGImageAlphaNoneSkipFirst);  // Set up the color environment
  CGContextSetFillColorSpace(cgContext, kGenericRGBColorSpace);
  CGContextSetStrokeColorSpace(
      cgContext, kGenericRGBColorSpace);  // Draw a white background
  CGContextSetFillColor(cgContext, kWhiteColor);
  CGContextAddRect(cgContext, CGRectMake(0, 0, kImageSize, kImageSize));
  CGContextFillPath(cgContext);  // put the origin at the center of the context
                                 // CGContextTranslateCTM(cgContext, kHalfSize,
                                 // kHalfSize); // draw the checkered background
  CGContextSetFillColor(cgContext, kBlackColor);
  CGContextBeginPath(cgContext);
  CGContextAddRect(cgContext,
                   CGRectMake(-kHalfSize, -kHalfSize, kHalfSize, kHalfSize));
  CGContextAddRect(cgContext, CGRectMake(0, 0, kHalfSize, kHalfSize));
  CGContextFillPath(cgContext);  // use a stroke to frame the entire image
  CGContextSetStrokeColor(cgContext, kBlackColor);
  CGContextSetLineWidth(cgContext, 4);
  CGContextAddRect(cgContext, CGRectMake(-kHalfSize + 2, -kHalfSize + 2,
                                         kImageSize - 4, kImageSize - 4));
  CGContextStrokePath(cgContext);  // construct and draw a heart shaped path
  CGContextSaveGState(cgContext);
  CGContextTranslateCTM(cgContext, kHalfSize / 2, kHalfSize / 2);
  CGContextMoveToPoint(cgContext, 0, 21);
  CGContextAddCurveToPoint(cgContext, -16, 49.8, -53.2, 41.0, -49.6, 5.8);
  CGContextAddCurveToPoint(cgContext, -46, -29.4, -9.4, -53.4, 0, -69.8);
  CGContextAddCurveToPoint(cgContext, 9.4, -53.4, 46, -29.4, 49.6, 5.8);
  CGContextAddCurveToPoint(cgContext, 53.2, 41, 16, 49.8, 0, 21);
  CGContextClosePath(cgContext);  // ask the computer to fill the path
  CGContextSetFillColor(cgContext, kRedColor);
  CGContextFillPath(cgContext);  // draw some text on top of the path.
  CenterTextInContextAtOrigin(cgContext, "Zapfino", 24, "Ace of Hearts");
  CGContextRestoreGState(cgContext);
  CGContextSaveGState(cgContext);
  CGContextTranslateCTM(cgContext, -kHalfSize / 2.0, -kHalfSize / 2.0);
  CenterTextInContextAtOrigin(cgContext, "Zapfino", 18, "Fine baked goods");
  CGContextRestoreGState(cgContext);
  CGContextRelease(cgContext);  // Create an image from the bitmap data
  CGDataProviderRef dataProvider =
      CGDataProviderCreateWithData(NULL, bitmapData, kDataLength, NULL);
  CGImageRef image =
      CGImageCreate(kImageSize, kImageSize, 8, 32, kBytesPerRow,
                    kGenericRGBColorSpace, kCGImageAlphaNoneSkipFirst,
                    dataProvider, NULL, true, kCGRenderingIntentDefault);
  CGDataProviderRelease(
      dataProvider);  // Find the user's desktop folder and create a     // URL
                      // to a file named AceOfHeartsBakery.png on the desktop
  FSRef usersDesktop;
  FSFindFolder(kUserDomain, kDesktopFolderType, false, &usersDesktop);
  CFURLRef desktopURL = CFURLCreateFromFSRef(NULL, &usersDesktop);
  CFURLRef destinationURL = CFURLCreateWithFileSystemPathRelativeToBase(
      NULL, CFSTR("AceOfHeartsBakery.png"), kCFURLPOSIXPathStyle, false,
      desktopURL);
  CFRelease(desktopURL);  // Export our image into that png file.
  CGImageDestinationRef exportDestination =
      CGImageDestinationCreateWithURL(destinationURL, kUTTypePNG, 1, NULL);
  CGImageDestinationAddImage(exportDestination, image, NULL);
  CGImageDestinationFinalize(exportDestination);
  CFRelease(exportDestination);
  CFRelease(destinationURL);
  CGImageRelease(image);
  free(bitmapData);
  CFRelease(kGenericRGBColorSpace);
  return 0;
}
