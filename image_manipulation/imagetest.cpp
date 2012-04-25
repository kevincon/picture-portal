#include <Magick++.h> 
#include <iostream> 
#include <stdint.h>

using namespace std; 
using namespace Magick; 
int main(int argc,char **argv) 
{ 
  // Construct the image object. Seperating image construction from the 
  // the read operation ensures that a failure to read the image file 
  // doesn't render the image object useless. 
  try { 
    // Read a file into image object 
    Image image( "cyan.jpg" );
    
    image.display();
   
    image.modifyImage();

    // Crop the image to specified size (width, height, xOffset, yOffset)
    //image.crop( Geometry(100,100, 100, 100) );
    image.resize("240x240");

    image.display();
    // Write the image to a file 
    //image.write( "x.gif" ); 

    Pixels my_pixel_cache(image); // allocate an image pixel cache associated with my_image
    PixelPacket* pixels;             // 'pixels' is a pointer to a PixelPacket array

    // define the view area that will be accessed via the image pixel cache

    int start_x = 0, start_y = 0, size_x = 240, size_y = 1; 

    // return a pointer to the pixels of the defined pixel cache

    pixels = my_pixel_cache.get(start_x, start_y, size_x, size_y);

    int i;
    uint16_t newpixel;
    for (i = 0; i < 240; i++) { 
        printf("pixel %d: red: %u green: %u blue: %u\n", i, 
        	(unsigned char) pixels[i].red,
		(unsigned char) pixels[i].green,
		(unsigned char) pixels[i].blue);

	printf("maxrgb: %d\n", MaxRGB);

	newpixel = 0;
	//red
        uint16_t newred = ((uint8_t) pixels[i].red) & 0xF8;
	printf("new red: %x\n", newred >> 4);
	newpixel |= newred << 8;
	//green
	newpixel |= (((uint16_t) (pixels[i].green)) & 0xF800) >> 5;
	//blue 
	newpixel |= (((uint16_t) (pixels[i].blue)) & 0xF800) >> 11;

	printf("new pixel %d: %x\n", i, newpixel);
    }

  
  } 
  catch( Exception &error_ ) 
    { 
      cout << "Caught exception: " << error_.what() << endl; 
      return 1; 
    } 
  return 0; 
}
