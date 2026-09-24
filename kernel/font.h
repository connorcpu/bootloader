#define PSF1_FONT_MAGIC 0x0436

/*
    For PSF1 glyph width is always = 8 bits
    and glyph height = characterSize
*/
typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

/*
    PSF2
*/
typedef struct psfFont{
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
}__attribute__((packed)) PSF_font;

void initFont();
void renderChar(char c, uint16_t x, uint16_t y);
void rendChar(char c, uint32_t offset);
