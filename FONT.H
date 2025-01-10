#ifndef _FONT 
#define _FONT


#include<afxwin.h>
#include<gl\gl.h>

typedef struct
    {
    GLuint base;        /* Display list number of first character */
    int    widths[256]; /* Width of each character in pixels */
    int    height;      /* Height of characters */
    } GLFONT;


class font 
{

private:

	GLFONT *myfont;

public:

	font();
	GLFONT	*FontCreate(HDC hdc, const char *typeface, int height,
		            int weight, DWORD italic);
	void	FontDestroy(GLFONT *font);
	void	FontPrintf(GLFONT *font, int align, const char *format, ...);
	void	FontPuts(GLFONT *font, const char *s);
	void	FontDelete(GLFONT *font);
	GLFONT	*returnfont();
			~font();

};


#endif 