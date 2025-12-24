#include <stdint.h>
int initFrame();
void drawRect(int x, int y, int width, int height, uint32_t colour);
void putpixel(uint8_t* screen, uint16_t x, unsigned short y, uint32_t colour);
void putPixel(uint16_t x, unsigned short y, uint32_t colour);

#define WHITE 0xFFFFFF
#define RED   0xFF0000
#define CYAN  0x00FFFF
#define BLUE  0x0000FF
#define DBLUE 0x0000A0
#define LBLUE 0xADD8E6
#define purpl 0x800080
#define yelow 0xFFFF00
#define LIME  0x00FF00
#define MAGEN 0xFF00FF
#define SILVE 0xC0C0C0
#define GRAY  0x808080
#define ORANG 0xFFA500
#define BROWN 0xA52A2A
#define MARON 0x800000
#define GREEN 0x008000
#define OLIVE 0x808000
#define BLACK 0x000000

typedef struct VbeInfoStructure{

   int8_t signature[4];	// must be "VESA" to indicate valid VBE support
	uint16_t version;			// VBE version; high byte is major version, low byte is minor version
	uint32_t oem;			// segment:offset pointer to OEM
	uint32_t capabilities;		// bitfield that describes card capabilities
	uint16_t video_modes[2];		// segment:offset pointer to list of supported video modes
	uint16_t video_memory;		// amount of video memory in 64KB blocks
	uint16_t software_rev;		// software revision
	uint32_t vendor;			// segment:offset to card vendor string
	uint32_t product_name;		// segment:offset to card model name
	uint32_t product_rev;		// segment:offset pointer to product revision
	char reserved[222];		// reserved for future expansion
	char oem_data[256];		// OEM BIOSes store their strings in this area
                           
} __attribute__ ((packed)) VbeInfoStructure_t;

typedef struct VbeModeInfoStructure{

   uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];

}__attribute__((packed)) VbeModeInfoStructure_t;
