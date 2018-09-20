#ifndef _MPDFAPP_H
#define _MPDFAPP_H

#ifdef XPDFVIEW_EXPORTS

#ifndef _WIN32
#define XPDFVIEW_API
#else
#define XPDFVIEW_API __declspec(dllexport)
#endif

#include "fitz.h"
#else
#ifndef _WIN32
#define XPDFVIEW_API
#else
#define XPDFVIEW_API __declspec(dllimport)
#endif

struct fz_document;
struct fz_outline;
struct fz_pixmap;
struct fz_page;
struct fz_display_list;
struct fz_text_page;
struct fz_text_sheet;
struct fz_link;
struct fz_context;

typedef struct fz_bbox_s fz_bbox;
struct fz_bbox_s
{
	int x0, y0;
	int x1, y1;
};

typedef struct fz_rect_s fz_rect;
struct fz_rect_s
{
	float x0, y0;
	float x1, y1;
};

typedef struct fz_size_s fz_size;
struct fz_size_s
{
    int cx;
    int cy;
};

#endif

#define MINRES	24
#define MAXRES	500
#define DEFRES	72

#define MAX_TEXT_LENGTH (512)

typedef enum 
{
    PDFAPP_EVENT_NONE = 0,
    PDFAPP_SET_CURSOR,
    PDFAPP_SET_TITLE,
    PDFAPP_RESIZE,
    PDFAPP_OPEN_URL,
    PDFAPP_REFRESH_UI,
}pdf_event;

typedef enum 
{ 
	CURSOR_ARROW = 0, 
	CURSOR_HAND, 
	CURSOR_WAIT 
}cursor_type;

enum
{
	PDFSHOW_ZOOM_NONE = 0,
	PDFSHOW_ZOOM_IN,
	PDFSHOW_ZOOM_OUT,
};

typedef struct _pdf_app PDFAPP,*LPPDFAPP;
struct _pdf_app
{
	/* current document params */
	fz_document *doc;
	char* doctitle;
	fz_outline *outline;

	int pagecount;

	/* current view params */
	int resolution;
	int fitsln;
	int rotate;
	fz_pixmap *image;
	int grayscale;
	int invert;

	/* current page params */
	int pageno;
	fz_page *page;
	fz_rect page_bbox;
	fz_display_list *page_list;
	fz_text_page *page_text;
	fz_text_sheet *page_sheet;
	fz_link *page_links;
	int errored;

	/* snapback history */
	int hist[256];
	int histlen;
	int marks[10];

	/* window system sizes */
	int winw, winh;
	int scrw, scrh;
	int shrinkwrap;
	int fullscreen;

	/* event handling state */
	char number[256];
	int numberlen;

	int ispanning;
	int panx, pany;

	int iscopying;
	int selx, sely;
	/* TODO - While sely keeps track of the relative change in
	* cursor position between two ticks/events, beyondy shall keep
	* track of the relative change in cursor position from the
	* point where the user hits a scrolling limit. This is ugly.
	* Used in pdfapp.c:pdfapp_onmouse.
	*/
	int beyondy;
	fz_bbox selr;

	/* search state */
	int isediting;
	int searchdir;
	char search[512];
	int hit;
	int hitlen;

	/* client context storage */
	void *userdata;

	fz_context *ctx;

	//BITMAPINFO * pBmpInfo;
};

typedef struct _pdf_callback_param pdf_callback_param;
struct _pdf_callback_param
{
    LPPDFAPP        app;
    unsigned int    event;

    union
    {
        unsigned int   cursor;
        fz_size        size;
        char           url[MAX_TEXT_LENGTH];
        char           title[MAX_TEXT_LENGTH];
    }data;

};

typedef struct _pdf_show_param pdf_show_param;
struct _pdf_show_param
{
    int panx;
    int pany;
    int image_w;
    int image_h;
    
    unsigned char * samples;
    int             samples_l;
};

typedef unsigned char fz_bool;
typedef unsigned int  fz_uint;

typedef void (* LPPdfEventCallback)(pdf_callback_param cbParam);

#ifdef __cplusplus
extern "C" {
#endif
    
	XPDFVIEW_API fz_bool pdfapp_init(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_open_with_file(LPPDFAPP app, char *filename, char* password,int reload);
	XPDFVIEW_API fz_bool pdfapp_open_with_file_w(LPPDFAPP app, wchar_t *filename, char* password,int reload);
	XPDFVIEW_API fz_bool pdfapp_open_with_data(LPPDFAPP app, char *data,int len,char* password,int reload);
	XPDFVIEW_API void pdfapp_close(LPPDFAPP app);

	XPDFVIEW_API fz_size pdfapp_pagesize(LPPDFAPP app);
	XPDFVIEW_API int  pdfapp_pagecount(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_get_showparam(LPPDFAPP app,pdf_show_param * show);
    XPDFVIEW_API void    pdfapp_free_showparam(pdf_show_param * show);
	XPDFVIEW_API fz_bool pdfapp_zoomin(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_zoomout(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_zoom(LPPDFAPP app,float fMulti);
	XPDFVIEW_API fz_uint pdfapp_zoomstate(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_nextpage(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_prevpage(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_lastpage(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_homepage(LPPDFAPP app);
	XPDFVIEW_API fz_bool pdfapp_gotopage(LPPDFAPP app,int nPage);

	XPDFVIEW_API void pdfapp_onkey(LPPDFAPP app, int c);
	XPDFVIEW_API void pdfapp_onmouse(LPPDFAPP app, int x, int y, int btn, int modifiers, int state);
	XPDFVIEW_API void pdfapp_oncopy(LPPDFAPP app, unsigned short *ucsbuf, int ucslen);
	XPDFVIEW_API void pdfapp_onresize(LPPDFAPP app, int w, int h, fz_bool force);

	XPDFVIEW_API void pdfapp_invert(LPPDFAPP app, fz_bbox rect);
	XPDFVIEW_API void pdfapp_inverthit(LPPDFAPP app);

    XPDFVIEW_API void pdfapp_setcallback(LPPdfEventCallback func);
#ifdef __cplusplus
}
#endif

#endif