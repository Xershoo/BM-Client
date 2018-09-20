#include "fitz.h"
#include "MPdfAPP.h"
#include <ctype.h> /* for tolower() */

#define ZOOMSTEP (12)
#define BEYOND_THRESHHOLD 40

#define true        (1)
#define false       (0)

static LPPdfEventCallback g_eventCallback = NULL;

enum panning
{
	DONT_PAN = 0,
	PAN_TO_TOP,
	PAN_TO_BOTTOM
};

static void pdfapp_showpage(LPPDFAPP app, int loadpage, int drawpage, int repaint);

static void pdfapp_warn(LPPDFAPP app, const char *fmt, ...)
{
	char buf[1024] = { 0 };
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	
    printf("PDF APP Warning: %s \n",buf);    
}

static void pdfapp_error(LPPDFAPP app, char *msg)
{
	if(msg && '\0' != msg[0])
	{
        printf("PDF APP Error: %s \n",msg);		
	}
}

static void pdfapp_callback(pdf_callback_param cbParam)
{
    if(NULL == g_eventCallback)
    {
        return;
    }

    (*g_eventCallback)(cbParam);
}

static void pdfapp_repaint(LPPDFAPP app)
{
    pdf_callback_param cbParam;
    cbParam.app = app;
    cbParam.event = PDFAPP_REFRESH_UI;

    pdfapp_callback(cbParam);
}

static void pdfapp_setcursor(LPPDFAPP app,cursor_type cursor)
{
    pdf_callback_param cbParam;
    cbParam.app = app;
    cbParam.event = PDFAPP_SET_CURSOR;
    cbParam.data.cursor = cursor;

    pdfapp_callback(cbParam);
}

static void pdfapp_settitle(LPPDFAPP app,char* title)
{
    pdf_callback_param cbParam;

    if( NULL == title || '\0' == title[0])
    {
        return;
    }

    cbParam.app = app;
    cbParam.event = PDFAPP_SET_TITLE;
    strcpy(cbParam.data.title,title);

    pdfapp_callback(cbParam);
}

static fz_bool pdfapp_load(LPPDFAPP app,fz_document * doc, char* password,int reload)
{
	int ret = 0;
	if(NULL == doc) {
		return false;
	};

	fz_try(app->ctx)
	{
		app->doc = doc;
		if (fz_needs_password(app->doc))
		{
			if (!password)
			{
				fz_throw(app->ctx, "Needs a password");
				return false;
			}

			ret = fz_authenticate_password(app->doc, password);
			if (!ret)
			{
				pdfapp_warn(app, "Invalid password.");
				return false;
			}
		
		}
		
		app->pagecount = fz_count_pages(app->doc);
		app->outline = fz_load_outline(app->doc);
	}
	fz_catch(app->ctx)
	{
		pdfapp_error(app, "cannot open document");
		return false;
	}

	if (app->pageno < 1)
		app->pageno = 1;
	if (app->pageno > app->pagecount)
		app->pageno = app->pagecount;
	if (app->resolution < MINRES)
		app->resolution = MINRES;
	if (app->resolution > MAXRES)
		app->resolution = MAXRES;

	app->fitsln = app->resolution;
	if (!reload)
	{
		app->shrinkwrap = 1;
		app->rotate = 0;
		app->panx = 0;
		app->pany = 0;
	}

	pdfapp_showpage(app, 1, 1, 1);
	return true;
};

static fz_matrix pdfapp_viewctm(LPPDFAPP app)
{
	fz_matrix ctm;
	ctm = fz_scale(app->resolution/72.0f, app->resolution/72.0f);
	ctm = fz_concat(ctm, fz_rotate(app->rotate));
	return ctm;
}

static void pdfapp_panview(LPPDFAPP app, int newx, int newy)
{
	int image_w = fz_pixmap_width(app->ctx, app->image);
	int image_h = fz_pixmap_height(app->ctx, app->image);

	if (newx > 0)
		newx = 0;
	if (newy > 0)
		newy = 0;

	if (newx + image_w < app->winw)
		newx = app->winw - image_w;
	if (newy + image_h < app->winh)
		newy = app->winh - image_h;

	if (app->winw >= image_w)
		newx = (app->winw - image_w) / 2;
	if (app->winh >= image_h)
		newy = (app->winh - image_h) / 2;

	if (newx != app->panx || newy != app->pany)
	{
		pdfapp_repaint(app);
	}

	app->panx = newx;
	app->pany = newy;
}

static void pdfapp_loadpage(LPPDFAPP app)
{
	fz_device *mdev = NULL;
	int errored = 0;
	fz_cookie cookie = { 0 };

	fz_var(mdev);

	if (app->page_list)
		fz_free_display_list(app->ctx, app->page_list);
	if (app->page_text)
		fz_free_text_page(app->ctx, app->page_text);
	if (app->page_sheet)
		fz_free_text_sheet(app->ctx, app->page_sheet);
	if (app->page_links)
		fz_drop_link(app->ctx, app->page_links);
	if (app->page)
		fz_free_page(app->doc, app->page);

	app->page_list = NULL;
	app->page_text = NULL;
	app->page_sheet = NULL;
	app->page_links = NULL;
	app->page = NULL;
	app->page_bbox.x0 = 0;
	app->page_bbox.y0 = 0;
	app->page_bbox.x1 = 100;
	app->page_bbox.y1 = 100;

	fz_try(app->ctx)
	{
		app->page = fz_load_page(app->doc, app->pageno - 1);

		app->page_bbox = fz_bound_page(app->doc, app->page);
	}
	fz_catch(app->ctx)
	{
		pdfapp_warn(app, "Cannot load page");
		return;
	}

	fz_try(app->ctx)
	{
		/* Create display list */
		app->page_list = fz_new_display_list(app->ctx);
		mdev = fz_new_list_device(app->ctx, app->page_list);
		fz_run_page(app->doc, app->page, mdev, fz_identity, &cookie);
		if (cookie.errors)
		{
			pdfapp_warn(app, "Errors found on page");
			errored = 1;
		}
	}
	fz_always(app->ctx)
	{
		fz_free_device(mdev);
	}
	fz_catch(app->ctx)
	{
		pdfapp_warn(app, "Cannot load page");
		errored = 1;
	}

	fz_try(app->ctx)
	{
		app->page_links = fz_load_links(app->doc, app->page);
	}
	fz_catch(app->ctx)
	{
		if (!errored)
			pdfapp_warn(app, "Cannot load page");
	}

	app->errored = errored;	
}

static void pdfapp_showpage(LPPDFAPP app, int loadpage, int drawpage, int repaint)
{
	char title[MAX_TEXT_LENGTH] = { 0 };
	fz_device *idev = NULL;
	fz_device *tdev = NULL;
	fz_colorspace *colorspace = NULL;
	fz_matrix ctm;
	fz_bbox bbox;
	fz_cookie cookie = { 0 };

	pdfapp_setcursor(app,CURSOR_WAIT);

	if (loadpage)
	{
		pdfapp_loadpage(app);

		/* Zero search hit position */
		app->hit = -1;
		app->hitlen = 0;

		/* Extract text */
		app->page_sheet = fz_new_text_sheet(app->ctx);
		app->page_text = fz_new_text_page(app->ctx, app->page_bbox);
		if (app->page_list)
		{
			tdev = fz_new_text_device(app->ctx, app->page_sheet, app->page_text);
			fz_run_display_list(app->page_list, tdev, fz_identity, fz_infinite_bbox, &cookie);
			fz_free_device(tdev);
		}
	}

	if (drawpage)
	{
		char tmp_buf[64] = { 0 };
		int  len;

		sprintf(tmp_buf, " %d/%d (%d dpi)",app->pageno, app->pagecount, app->resolution);

		len = MAX_TEXT_LENGTH-strlen(tmp_buf);
		if(NULL != app->doctitle && strlen(app->doctitle)>0)
		{
			if ((int)strlen(app->doctitle) > len)
			{
				snprintf(title, len-3, "%s", app->doctitle);
				strcat(title, "... - ");
				strcat(title, tmp_buf);
			}
			else
				sprintf(title, "%s - %s", app->doctitle, tmp_buf);
		}

		pdfapp_settitle(app,title);
        
		ctm = pdfapp_viewctm(app);
		bbox = fz_round_rect(fz_transform_rect(ctm, app->page_bbox));

		/* Draw */
		if (app->image)
			fz_drop_pixmap(app->ctx, app->image);
		if (app->grayscale)
			colorspace = fz_device_gray;
		else
#ifdef _WIN32
			colorspace = fz_device_bgr;
#else
			colorspace = fz_device_rgb;
#endif
		app->image = fz_new_pixmap_with_bbox(app->ctx, colorspace, bbox);
		fz_clear_pixmap_with_value(app->ctx, app->image, 255);
		if (app->page_list)
		{
			idev = fz_new_draw_device(app->ctx, app->image);			
			fz_run_display_list(app->page_list, idev, ctm, bbox, &cookie);
			fz_free_device(idev);
		}
		if (app->invert)
			fz_invert_pixmap(app->ctx, app->image);
	}

	if (repaint)
	{
		pdfapp_panview(app, app->panx, app->pany);

		if (app->shrinkwrap)
		{
			int w = fz_pixmap_width(app->ctx, app->image);
			int h = fz_pixmap_height(app->ctx, app->image);
			if (app->winw == w)
				app->panx = 0;
			if (app->winh == h)
				app->pany = 0;
			if (w > app->scrw * 90 / 100)
				w = app->scrw * 90 / 100;
			if (h > app->scrh * 90 / 100)
				h = app->scrh * 90 / 100;
			if (w != app->winw || h != app->winh)
			{
                pdf_callback_param cbParam;
                cbParam.app = app;
                cbParam.event = PDFAPP_RESIZE;
                cbParam.data.size.cx = w;
                cbParam.data.size.cy = h;

                pdfapp_callback(cbParam);                
            }
		}

		pdfapp_repaint(app);		
        pdfapp_setcursor(app,CURSOR_ARROW);
	}

	if (cookie.errors && app->errored == 0)
	{
		app->errored = 1;
		pdfapp_warn(app, "Errors found on page. Page rendering may be incomplete.");
	}

	fz_flush_warnings(app->ctx);
}

static void pdfapp_gotouri(LPPDFAPP app, char *url)
{
    pdf_callback_param cbParam;

    if(NULL == url || '\0' == url[0])
    {
        return;
    }

    cbParam.app = app;
    cbParam.event = PDFAPP_OPEN_URL;
    strcpy(cbParam.data.url,url);

    pdfapp_callback(cbParam);
}

static fz_text_char textcharat(fz_text_page *page, int idx)
{
	fz_text_char emptychar = { {0,0,0,0}, ' ' };
	fz_text_block *block;
	fz_text_line *line;
	fz_text_span *span;
	int ofs = 0;

	for (block = page->blocks; block < page->blocks + page->len; block++)
	{
		for (line = block->lines; line < block->lines + block->len; line++)
		{
			for (span = line->spans; span < line->spans + line->len; span++)
			{
				if (idx < ofs + span->len)
					return span->text[idx - ofs];
				/* pseudo-newline */
				if (span + 1 == line->spans + line->len)
				{
					if (idx == ofs + span->len)
						return emptychar;
					ofs++;
				}
				ofs += span->len;
			}
		}
	}
	return emptychar;
}

static int textlen(fz_text_page *page)
{
	fz_text_block *block;
	fz_text_line *line;
	fz_text_span *span;
	int len = 0;
	for (block = page->blocks; block < page->blocks + page->len; block++)
	{
		for (line = block->lines; line < block->lines + block->len; line++)
		{
			for (span = line->spans; span < line->spans + line->len; span++)
				len += span->len;
			len++; /* pseudo-newline */
		}
	}
	return len;
}

static inline int charat(fz_text_page *page, int idx)
{
	return textcharat(page, idx).c;
}

static inline fz_bbox bboxcharat(fz_text_page *page, int idx)
{
	return fz_bbox_covering_rect(textcharat(page, idx).bbox);
}

//////////////////////////////////////////////////////////////////////////
//
// interface 
//

XPDFVIEW_API fz_bool pdfapp_init(LPPDFAPP app)
{
	fz_context* ctx = NULL;
	if(NULL == app)
	{
		return false;
	}

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if(!ctx) {
		return false;
	};

	memset(app, 0, sizeof(PDFAPP));
	app->scrw = 640;
	app->scrh = 480;
	app->resolution = DEFRES;
	app->fitsln = DEFRES;

	app->ctx = ctx;
	
	return true;
}

XPDFVIEW_API fz_bool pdfapp_open_with_file(LPPDFAPP app, char *filename, char* password,int reload)
{
	fz_bool br = false;
	char * pszTitle = NULL;

	if(NULL==app || NULL==filename ||'\0'==filename[0]) {
		return false;
	};

	fz_try(app->ctx)
	{
		fz_document * doc = fz_open_document(app->ctx, filename);
		if(NULL == doc) {
			return false;
		};

		br = pdfapp_load(app,doc,password,reload);
		if(!br) {
			return false;
		}
		
		if (strrchr(filename, '\\'))
			pszTitle = strrchr(filename, '\\') + 1;
		if (strrchr(filename, '/'))
			pszTitle = strrchr(filename, '/') + 1;

		app->doctitle = fz_strdup(app->ctx, pszTitle);
	}
	fz_catch(app->ctx)
	{
		pdfapp_error(app, "cannot open document");
		return false;
	}

	return true;
}

XPDFVIEW_API fz_bool pdfapp_open_with_file_w(LPPDFAPP app, wchar_t *filename, char* password,int reload)
{
	fz_bool br = false;
	if(NULL==app || NULL==filename ||'\0'==filename[0]) {
		return false;
	};

	fz_try(app->ctx)
	{
		fz_document * doc = fz_open_document_w(app->ctx, filename);
		if(NULL == doc) {
			return false;
		};

		br = pdfapp_load(app,doc,password,reload);
		if(!br) {
			return false;
		}	
	}
	fz_catch(app->ctx)
	{
		pdfapp_error(app, "cannot open document");
		return false;
	}

	return true;
}

XPDFVIEW_API fz_bool pdfapp_open_with_data(LPPDFAPP app, char *data,int len,char* password,int reload)

{
	fz_bool br = false;
	if(NULL==app || NULL==data ||0==len) {
		return false;
	};

	fz_try(app->ctx)
	{
		fz_document * doc = fz_open_document_with_data(app->ctx, data,len);
		if(NULL == doc) {
			return false;
		};

		br = pdfapp_load(app,doc,password,reload);
		if(!br) {
			return false;
		}
	}
	fz_catch(app->ctx)
	{
		pdfapp_error(app, "cannot open document");
		return false;
	}

	return true;
}

XPDFVIEW_API void pdfapp_invert(LPPDFAPP app, fz_bbox rect)
{
	if(NULL == app)
	{
		return;
	}

	fz_invert_pixmap_rect(app->image, rect);
}

XPDFVIEW_API void pdfapp_close(LPPDFAPP app)
{
	if(NULL == app)
	{
		return;
	}

	if (app->page_list)
		fz_free_display_list(app->ctx, app->page_list);
	app->page_list = NULL;

	if (app->page_text)
		fz_free_text_page(app->ctx, app->page_text);
	app->page_text = NULL;

	if (app->page_sheet)
		fz_free_text_sheet(app->ctx, app->page_sheet);
	app->page_sheet = NULL;

	if (app->page_links)
		fz_drop_link(app->ctx, app->page_links);
	app->page_links = NULL;

	if (app->doctitle)
		fz_free(app->ctx, app->doctitle);
	app->doctitle = NULL;

	if (app->image)
		fz_drop_pixmap(app->ctx, app->image);
	app->image = NULL;

	if (app->outline)
		fz_free_outline(app->ctx, app->outline);
	app->outline = NULL;

	if (app->page)
		fz_free_page(app->doc, app->page);
	app->page = NULL;

	if (app->doc)
	{
		fz_close_document(app->doc);
		app->doc = NULL;
	}

	fz_flush_warnings(app->ctx);
	fz_free_context(app->ctx);

}

XPDFVIEW_API void pdfapp_onresize(LPPDFAPP app, int w,int h,fz_bool fc)
{
	fz_matrix ctm;
	fz_bbox bbox;
	int sln = 0;
	int orsl = 0;
	
	if(NULL ==app || w <= 0 || h <= 0)
	{
		return;
	}

	orsl = app->resolution;

	if (app->winw != w || app->winh != h || fc)
	{
		app->winw = w;
		app->winh = h;

		do 
		{			
			ctm = pdfapp_viewctm(app);
			bbox = fz_round_rect(fz_transform_rect(ctm, app->page_bbox));

			if(bbox.x1 - bbox.x0 > w || bbox.y1- bbox.y0 > h)
			{
				if(sln == 2)
				{
					app->resolution -= 2;
					break;
				}

				sln = 1;
			}
			else if(bbox.x1 - bbox.x0 < w && bbox.y1- bbox.y0 < h)
			{
				if(sln == 1)
				{
					break;
				}

				sln = 2;
			}
			else
			{
				break;
			}

			app->resolution += sln == 1 ? (-2) : 2;
			if(app->resolution>MAXRES)
			{
				app->resolution = MAXRES;
				break;
			}

			if(app->resolution<MINRES)
			{
				app->resolution = MINRES;
				break;
			}
		} while (true);
				
		pdfapp_panview(app, app->panx, app->pany);
		if(orsl != app->resolution)
		{
			app->fitsln = app->resolution;
			pdfapp_showpage(app, 0, 1, 1);
		}
	}
}

XPDFVIEW_API void pdfapp_oncopy(LPPDFAPP app, unsigned short *buf, int len)
{
	fz_bbox hitbox;
	fz_matrix ctm;
	fz_text_page *page = NULL;
	fz_text_block *block;
	fz_text_line *line;
	fz_text_span *span;
	int c, i, p;
	int seen = 0;

	int x0 = 0;
	int x1 = 0;
	int y0 = 0;
	int y1 = 0;

	if(NULL ==app || NULL == buf || 0 >= len)
	{
		return;
	}

	page = app->page_text;

	x0 = app->selr.x0;
	x1 = app->selr.x1;
	y0 = app->selr.y0;
	y1 = app->selr.y1;

	ctm = pdfapp_viewctm(app);

	p = 0;

	for (block = page->blocks; block < page->blocks + page->len; block++)
	{
		for (line = block->lines; line < block->lines + block->len; line++)
		{
			for (span = line->spans; span < line->spans + line->len; span++)
			{
				if (seen)
				{
#ifdef _WIN32
					if (p < len - 1)
						buf[p++] = '\r';
#endif
					if (p < len - 1)
						buf[p++] = '\n';
				}

				seen = 0;

				for (i = 0; i < span->len; i++)
				{
					hitbox = fz_bbox_covering_rect(span->text[i].bbox);
					hitbox = fz_transform_bbox(ctm, hitbox);
					c = span->text[i].c;
					if (c < 32)
						c = '?';
					if (hitbox.x1 >= x0 && hitbox.x0 <= x1 && hitbox.y1 >= y0 && hitbox.y0 <= y1)
					{
						if (p < len - 1)
							buf[p++] = c;
						seen = 1;
					}
				}

				seen = (seen && span + 1 == line->spans + line->len);
			}
		}
	}

	buf[p] = 0;
}

XPDFVIEW_API void pdfapp_inverthit(LPPDFAPP app)
{
	fz_bbox hitbox, bbox;
	fz_matrix ctm;
	int i;

	if (NULL == app ||app->hit < 0) {
		return;
	}

	hitbox = fz_empty_bbox;
	ctm = pdfapp_viewctm(app);

	for (i = app->hit; i < app->hit + app->hitlen; i++)
	{
		bbox = bboxcharat(app->page_text, i);
		if (fz_is_empty_rect(bbox))
		{
			if (!fz_is_empty_rect(hitbox))	{
				pdfapp_invert(app, fz_transform_bbox(ctm, hitbox));
			};

			hitbox = fz_empty_bbox;
		} else	{
			hitbox = fz_union_bbox(hitbox, bbox);
		}
	}

	if (!fz_is_empty_rect(hitbox)) {
		pdfapp_invert(app, fz_transform_bbox(ctm, hitbox));
	};
}

XPDFVIEW_API fz_bool pdfapp_get_showparam(LPPDFAPP app,pdf_show_param* show)
{
	int image_w = 0;
	int image_h = 0;
	int image_n = 0;
	unsigned char *samples = NULL;

	if (NULL == app || NULL == app->image || NULL == app->image || NULL == show)
	{
		return false;
	}

	image_w = fz_pixmap_width(app->ctx,app->image);
	image_h = fz_pixmap_height(app->ctx,app->image);
	image_n = fz_pixmap_components(app->ctx,app->image);
	samples = fz_pixmap_samples(app->ctx,app->image);	

    if (image_n != 2 && image_n != 4)
    {
        return false;
    }

	show->panx = app->panx;
    show->pany = app->pany;
    show->image_w = image_w;
    show->image_h = image_h;

	if (app->iscopying)
	{
		pdfapp_invert(app, app->selr);			
	}

	pdfapp_inverthit(app);
    
    show->samples_l = image_w * image_h * 4;
    show->samples = (unsigned char*)malloc(show->samples_l);
	if (image_n == 2)
	{
		int i = image_w * image_h;		
		unsigned char *s = samples;
		unsigned char *d = show->samples;
		for (; i > 0 ; i--)
		{
			d[2] = d[1] = d[0] = *s++;
			d[3] = *s++;
			d += 4;
		}
	}
    else
	{
         memcpy(show->samples,samples,show->samples_l);
	}
    
    pdfapp_inverthit(app);
    if (app->iscopying)
    {
        pdfapp_invert(app, app->selr);			
    }

    return true;
}

XPDFVIEW_API void pdfapp_free_showparam(pdf_show_param* show)
{
    if(NULL == show)
    {
        return;
    }

    if(show->samples)
    {
        free(show->samples);
        show->samples = NULL;
    }

    memset(show,0,sizeof(pdf_show_param));
}

XPDFVIEW_API fz_bool pdfapp_zoomin(LPPDFAPP app)
{
	if(NULL == app || app->resolution == MAXRES)
	{
		return false;
	}
	
	app->resolution += ZOOMSTEP;
	if (app->resolution > MAXRES)
	{		
		app->resolution = MAXRES;
	}


	pdfapp_showpage(app, 0, 1, 1);

	return true;
}

XPDFVIEW_API fz_bool pdfapp_zoomout(LPPDFAPP app)
{
	if(NULL == app ||app->resolution == MINRES)
	{
		return false;
	}

	app->resolution -= ZOOMSTEP;
	if (app->resolution < MINRES)
	{	
		app->resolution = MINRES;
	}
	
	pdfapp_showpage(app, 0, 1, 1);

	return true;
}

XPDFVIEW_API fz_bool pdfapp_zoom(LPPDFAPP app,float fMulti)
{
	if(NULL == app)
	{
		return false;
	}

	app->resolution = (float)app->fitsln * fMulti;
	if (app->resolution < MINRES)
	{	
		app->resolution = MINRES;
	}
	else if (app->resolution > MAXRES)
	{		
		app->resolution = MAXRES;
	}

	pdfapp_showpage(app, 0, 1, 1);

	return true;
}

XPDFVIEW_API fz_uint pdfapp_zoomstate(LPPDFAPP app)
{
	if(NULL == app)
	{
		return PDFSHOW_ZOOM_NONE;
	}

	return (app->resolution == app->fitsln) ? PDFSHOW_ZOOM_NONE : ((app->resolution > DEFRES) ? PDFSHOW_ZOOM_IN : PDFSHOW_ZOOM_OUT);
}

XPDFVIEW_API fz_bool pdfapp_nextpage(LPPDFAPP app)
{
	if(NULL == app ||app->pageno == app->pagecount)
	{
		return false;
	}

	app->pageno ++;
	pdfapp_showpage(app, 1, 1, 1);
	pdfapp_onresize(app,app->winw,app->winh,true);
	return true;
}

XPDFVIEW_API fz_bool pdfapp_prevpage(LPPDFAPP app)
{
	if(NULL == app ||app->pageno == 1)
	{
		return false;
	}

	app->pageno --;
	pdfapp_showpage(app, 1, 1, 1);
	pdfapp_onresize(app,app->winw,app->winh,true);
	return true;
}

XPDFVIEW_API fz_bool pdfapp_lastpage(LPPDFAPP app)
{
	if(NULL == app ||app->pageno == app->pagecount)
	{
		return false;
	}

	app->pageno = app->pagecount;
	pdfapp_showpage(app, 1, 1, 1);
	pdfapp_onresize(app,app->winw,app->winh,true);
	return true;
}

XPDFVIEW_API fz_bool pdfapp_homepage(LPPDFAPP app)
{
	if(NULL == app ||app->pageno == 1)
	{
		return false;
	}

	app->pageno =1;
	pdfapp_showpage(app, 1, 1, 1);
	pdfapp_onresize(app,app->winw,app->winh,true);
	return true;
}

XPDFVIEW_API fz_bool pdfapp_gotopage(LPPDFAPP app,int nPage)
{
	if(NULL == app ||app->pageno == nPage)
	{
		return true;
	}

	if(nPage < 1 || nPage > app->pagecount)
	{
		return false;
	}

	app->isediting = 0;

	if (app->histlen + 1 == 256)
	{
		memmove(app->hist, app->hist + 1, sizeof(int) * 255);
		app->histlen --;
	}
	app->hist[app->histlen++] = app->pageno;
	app->pageno = nPage;
	pdfapp_showpage(app, 1, 1, 1);
	pdfapp_onresize(app,app->winw,app->winh,true);
	return true;
}

XPDFVIEW_API int  pdfapp_pagecount(LPPDFAPP app)
{
	return NULL == app ? 0 : app->pagecount;
}

XPDFVIEW_API fz_size pdfapp_pagesize(LPPDFAPP app)
{
	fz_size szPage  = { 0 };
	fz_matrix ctm;
	fz_bbox bbox;

	if(NULL == app)
	{
		return szPage;
	}

	ctm = pdfapp_viewctm(app);
	bbox = fz_round_rect(fz_transform_rect(ctm, app->page_bbox));

	szPage.cx = bbox.x1 - bbox.x0;
	szPage.cy = bbox.y1 - bbox.y0;

	return szPage;
}

XPDFVIEW_API void pdfapp_setcallback(LPPdfEventCallback func)
{
    g_eventCallback = func;
}