#include "stdafx.h"
#include "kmain.h"
#include "kUtils.h"
#include "kStyle.h"

void k_luaerr(lua_State* L, const char* text)
{
	lua_pushstring(L, text);
	lua_error(L);
}

kFontStruct getTextExtends(kStyle& style, std::wstring str)
{
	kFontStruct temp;
	temp.k_width = 0;
	temp.k_height = 0;
	temp.k_ascent = 0;
	temp.k_descent = 0;

	HDC kDC = CreateCompatibleDC(0);
	if (!kDC) return temp;
	SetMapMode(kDC, MM_TEXT);

	LOGFONTW lf;
	memset(&lf, 0, sizeof(lf));
	lf <<= style;
//	lf.lfHeight = (LONG)(style.n_fontsize+0.5);
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;

	HFONT nFont = CreateFontIndirect(&lf);
	if(!nFont)
		return temp;

	HFONT hOldFont = SelectFont(kDC, nFont);
	TEXTMETRIC tm;
	GetTextMetrics(kDC, &tm);
	temp.k_ascent = style.n_fontscaleY / 100 * tm.tmAscent;
	temp.k_descent = style.n_fontscaleY / 100 * tm.tmDescent;
	temp.k_extlead = tm.tmExternalLeading;

	double k_width = 0;
	double k_height = 0;
	if(style.n_fontspacing || (long)GetVersion() < 0)
	{
		for(LPCWSTR s = str.c_str(); *s; s++)
		{
			SIZE extent;
			if(!GetTextExtentPoint32W(kDC, s, 1, &extent)) { return temp; }
			k_width += extent.cx + style.n_fontspacing;
			k_height = max(k_height, extent.cy);
		}
	}
	else
	{
		SIZE extent;
		if(!GetTextExtentPoint32W(kDC, str.c_str(), (int)str.length(), &extent)) { return temp;}
		k_width += extent.cx;
		k_height = max(k_height, extent.cy);
	}
	DeleteObject(kDC);
	DeleteObject(nFont);

	int bluradjust = 0;
	if (style.n_be)
		bluradjust += 1;
	if (style.n_blur > 0)
		bluradjust += (int)(style.n_blur*3)|1;
	else if (style.n_blur==0)
		bluradjust += (int)style.n_outlineY;

	temp.k_width = style.n_fontscaleX / 100 * k_width + bluradjust*2;
	temp.k_height = style.n_fontscaleY / 100 * k_height + bluradjust*2;

	return temp;
}


int lua_getInt(lua_State* L, std::string fieldname, int& status)
{
	lua_getfield(L, 1, fieldname.c_str());
	if (!lua_isnumber(L, -1))
	{
		k_luaerr(L, ("Invalid number '" + fieldname + "' field in style").c_str());
		status = 0;
		return 0;
	}
	int res = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);
	return res;
}

double lua_getFloat(lua_State* L, std::string fieldname, int& status)
{
	lua_getfield(L, 1, fieldname.c_str());
	if (!lua_isnumber(L, -1))
	{
		k_luaerr(L, ("Invalid number '" + fieldname + "' field in style").c_str());
		status = 0;
		return 0;
	}
	double res = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return res;
}

std::wstring lua_getStr(lua_State* L, std::string fieldname, int& status)
{
	lua_getfield(L, 1, fieldname.c_str());
	if (!lua_isstring(L, -1))
	{
		k_luaerr(L, ("Invalid string '" + fieldname + "' field in style").c_str());
		status = 0;
		return L"";
	}
	std::wstring res = _SW(lua_tostring(L, -1));
	lua_pop(L, 1);
	return res;
}

bool lua_getBool(lua_State* L, std::string fieldname, int& status)
{
	lua_getfield(L, 1, fieldname.c_str());
	if (!lua_isboolean(L, -1))
	{
		k_luaerr(L, ("Invalid boolean '" + fieldname + "' field in style").c_str());
		status = 0;
		return false;
	}
	bool res = !!lua_toboolean(L, -1);
	lua_pop(L, 1);
	return res;
}

int getStyleFromLua(lua_State* L, kStyle& style)
{
	int status = 1;
	style.n_fontname = lua_getStr(L, "fontname", status);
	style.n_fontsize = lua_getFloat(L, "fontsize", status);
	style.n_colors[0] = hextoint(lua_getStr(L, "color1", status));
	style.n_colors[1] = hextoint(lua_getStr(L, "color2", status));
	style.n_colors[2] = hextoint(lua_getStr(L, "color3", status));
	style.n_colors[3] = hextoint(lua_getStr(L, "color4", status));
	style.n_fontweight = (lua_getBool(L, "bold", status)) ? FW_BOLD : FW_NORMAL;
	style.n_f_italic = lua_getBool(L, "italic", status);
	style.n_f_underline = lua_getBool(L, "underline", status);
	style.n_f_strikeout = lua_getBool(L, "strikeout", status);
	style.n_fontscaleX = lua_getFloat(L, "scale_x", status);
	style.n_fontscaleY = lua_getFloat(L, "scale_y", status);
	style.n_fontspacing = lua_getFloat(L, "spacing", status);
	style.n_angleZ = lua_getFloat(L, "angle", status);
	style.n_outlineY = lua_getFloat(L, "outline", status);
	style.n_shadowY = lua_getFloat(L, "shadow", status);
	style.n_charset = lua_getInt(L, "encoding", status);
	return status; // ok
}

int lua_kh_textextent(lua_State* L)
{
	// check argument #1 = style, table "style"
	if (!lua_istable(L, 1))
	{
		k_luaerr(L, "Argument #1 is invalid (need table 'style')");
		return 0;
	}
	lua_getfield(L, 1, "class");
	if (!lua_isstring(L, -1))
	{
		k_luaerr(L, "Argument #1 is invalid, class field is not string.");
		return 0;
	}
	// get class of table
	std::wstring argclass = LowerCase(_SW(lua_tostring(L, -1)));
	lua_pop(L, 1);

	kStyle style;
	if (argclass == L"style")
	{
		if(getStyleFromLua(L, style)==0) return 0; // error
	}
	else
	{
		argclass = L"Argument #1 is invalid, need class table. Passed: " + argclass;
		k_luaerr(L, _WS(argclass).c_str());
		return 0;
	}
	// check argument #2 = text, string
	if (!lua_isstring(L, 2))
	{
		k_luaerr(L, "Argument #2 is invalid, need string 'text to render'.");
		return 0;
	}
	// get text
	std::wstring rend_text = _SW(lua_tostring(L, 2));

	rend_text = parseLine(rend_text, style); // parse tags

	kFontStruct kFS = getTextExtends(style, rend_text);

	lua_newtable(L);

	// width
	lua_pushnumber(L, kFS.k_width);
	lua_setfield(L, -2, "width");

	// height
	lua_pushnumber(L, kFS.k_height);
	lua_setfield(L, -2, "height");

	// ascent
	lua_pushnumber(L, kFS.k_ascent);
	lua_setfield(L, -2, "ascent");

	// descent
	lua_pushnumber(L, kFS.k_descent);
	lua_setfield(L, -2, "descent");

	// external leading
	lua_pushnumber(L, kFS.k_extlead);
	lua_setfield(L, -2, "extlead");

	double bluradjust = 0;
	if (style.n_be)
		bluradjust += 1;
	if (style.n_blur==0)
	{
		bluradjust += style.n_outlineY;
		lua_pushnumber(L, bluradjust);
		lua_setfield(L, -2, "offsetx");
	}
	else if (style.n_blur > 0)
	{
		bluradjust += (int)(style.n_blur*3)|1;
		lua_pushnumber(L, bluradjust);
		lua_setfield(L, -2, "offsetx");
	}

	return 1;
}

int lua_kh_outlinetext(lua_State* L)
{
	// check argument #1 = style, table "style"
	if (!lua_istable(L, 1))
	{
		k_luaerr(L, "Argument #1 is invalid (need table 'style')");
		return 0;
	}
	lua_getfield(L, 1, "class");
	if (!lua_isstring(L, -1))
	{
		k_luaerr(L, "Argument #1 is invalid, class field is not string.");
		return 0;
	}
	// get class of table
	std::wstring argclass = LowerCase(_SW(lua_tostring(L, -1)));
	lua_pop(L, 1);

	kStyle style;
	if (argclass == L"style")
	{
		if(getStyleFromLua(L, style)==0) return 0; // error
	}
	else
	{
		argclass = L"Argument #1 is invalid, need class table. Passed: " + argclass;
		k_luaerr(L, _WS(argclass).c_str());
		return 0;
	}
	// check argument #2 = text, string
	if (!lua_isstring(L, 2))
	{
		k_luaerr(L, "Argument #2 is invalid, need string 'text to render'.");
		return 0;
	}
	// get text
	std::wstring rend_text = _SW(lua_tostring(L, 2));

	// init DC
	HDC n_DC = CreateCompatibleDC(NULL);
	SetBkMode(n_DC, TRANSPARENT); 
	SetTextColor(n_DC, 0xffffff); 
	SetMapMode(n_DC, MM_TEXT);

	kRenderer* rend = new kRenderer(n_DC);

	std::wstring temp = rend->Render(style, rend_text);

	DeleteDC(n_DC);

	//lua_newtable(L);

	// width
	lua_pushstring(L, _WS(temp).c_str());
	//lua_setfield(L, -2, "drawing");

	return 1;
}

int lua_kh_render(lua_State* L)
{
	// check argument #1 = style, table "style"
	if (!lua_istable(L, 1))
	{
		k_luaerr(L, "Argument #1 is invalid (need table 'style')");
		return 0;
	}
	lua_getfield(L, 1, "class");
	if (!lua_isstring(L, -1))
	{
		k_luaerr(L, "Argument #1 is invalid, class field is not string.");
		return 0;
	}
	// get class of table
	std::wstring argclass = LowerCase(_SW(lua_tostring(L, -1)));
	lua_pop(L, 1);

	kStyle style;
	if (argclass == L"style")
	{
		if(getStyleFromLua(L, style)==0) return 0; // error
	}
	else
	{
		argclass = L"Argument #1 is invalid, need class table. Passed: " + argclass;
		k_luaerr(L, _WS(argclass).c_str());
		return 0;
	}
	// check argument #2 = text, string
	if (!lua_isstring(L, 2))
	{
		k_luaerr(L, "Argument #2 is invalid, need string 'text to render'.");
		return 0;
	}
	// get text
	std::wstring rend_text = _SW(lua_tostring(L, 2));

	rend_text = parseLine(rend_text, style); // parse tags

	kFontStruct kFS = getTextExtends(style, rend_text);

	int width = (int)kFS.k_width;
	int height = (int)kFS.k_height;
	std::wstring tempass = style.getAssFile(rend_text, width, height);
	// init CSRI
	kCSRI* kcsri_rend = new kCSRI();
	int ret = kcsri_rend->Open(tempass);
	
	switch(ret)
	{
	case 1: k_luaerr(L, "Cannot load CSRI."); return 0;
	case 2: k_luaerr(L, "Internal error: No data for CSRI."); return 0;
	}

	videoframe frame;
	frame.width = width;
	frame.height = height;
	frame.data = (BYTE*)malloc(width * height * 4);
	memsetd(frame.data, 0xFF000000, width * height * 4);

	ret = kcsri_rend->Draw(frame);
	switch(ret)
	{
	case 1: k_luaerr(L, "No CSRI renderer."); return 0;
	case 2: k_luaerr(L, "Incompatible format."); return 0;
	}

	kcsri_rend->Close();

	// return table:
	// image
	// * width
	// * height
	// * ascent
	// * descent
	// * extlead
	// * offsetx
	// * pixels[x][y]
	// * alpha[x][y]
	lua_newtable(L);

	// width
	lua_pushnumber(L, kFS.k_width);
	lua_setfield(L, -2, "width");

	// height
	lua_pushnumber(L, kFS.k_height);
	lua_setfield(L, -2, "height");

	// ascent
	lua_pushnumber(L, kFS.k_ascent);
	lua_setfield(L, -2, "ascent");

	// descent
	lua_pushnumber(L, kFS.k_descent);
	lua_setfield(L, -2, "descent");

	// external leading
	lua_pushnumber(L, kFS.k_extlead);
	lua_setfield(L, -2, "extlead");

	double bluradjust = 0;
	if (style.n_be)
		bluradjust += 1;
	if (style.n_blur==0)
	{
		bluradjust += style.n_outlineY;
		lua_pushnumber(L, bluradjust);
		lua_setfield(L, -2, "offsetx");
	}
	else if (style.n_blur > 0)
	{
		bluradjust += (int)(style.n_blur*3)|1;
		lua_pushnumber(L, bluradjust);
		lua_setfield(L, -2, "offsetx");
	}
	// pixels data
	lua_newtable(L);
	DWORD* bits = (DWORD*)frame.data;
	for(size_t x = 0; x < (size_t)width; x++)
	{
		lua_pushnumber(L, (int)x+1); // index x
		lua_newtable(L); // value: array with y
		for(size_t y = 0; y < (size_t)height; y++)
		{
			DWORD col = bits[(y*(width)) + x];
			// CSRI_F_BGR_
			DWORD r = (col<<16)&0xff0000;
			DWORD g = (col)&0xff00;
			DWORD b = (col>>16)&0xff;
			col = b|g|r;
			lua_pushnumber(L, (int)y+1); // index y
			lua_pushnumber(L, col); // value: color
			lua_settable(L, -3);
		}
		lua_settable(L, -3);
	}
	lua_setfield(L, -2, "color");

	// alpha data
	lua_newtable(L);

	for(size_t x = 0; x < (size_t)width; x++)
	{
		lua_pushnumber(L, x+1); // index x
		lua_newtable(L); // value: array with y
		for(size_t y = 0; y < (size_t)height; y++)
		{
			DWORD col = bits[(y*(width)) + x];

			lua_pushnumber(L, (int)y+1); // index y
			lua_pushnumber(L, (col>>24)&0xff); // value: alpha
			lua_settable(L, -3);
		}
		lua_settable(L, -3);
	}
	lua_setfield(L, -2, "alpha");
	
	free(frame.data);

	return 1;
}

int lua_kh_loadpng(lua_State* L)
{
	// check argument #1 = filename, string
	if (!lua_isstring(L, 1))
	{
		k_luaerr(L, "loadpng: Argument #1 is invalid, need string 'filename'.");
		return 0;
	}
	// get text
	std::string filename = lua_tostring(L, 1);

	char header[8];	// 8 is the maximum size that can be check
	png_structp png_ptr;
	
	FILE *fp = fopen(filename.c_str(), "rb");
	if (!fp) // File could not be opened for reading
	{
		k_luaerr(L, "loadpng: File could not be opened for reading.");
		return 0;
	}
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_bytep)header, 0, 8)) // File is not recognized as a PNG file
	{
		k_luaerr(L, "loadpng: File is not recognized as a PNG file.");
		return 0;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) // png_create_read_struct failed
	{
		k_luaerr(L, "loadpng: png_create_read_struct failed.");
		return 0;
	}
	png_init_io(png_ptr, fp);

	png_infop info_ptr;
	int number_of_passes;

	/* initialize stuff */
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) // png_create_info_struct failed
	{
		k_luaerr(L, "loadpng: png_create_info_struct failed.");
		return 0;
	}

	if (setjmp(png_jmpbuf(png_ptr))) // Error during init_io
	{
		k_luaerr(L, "loadpng: Error during init_io.");
		return 0;
	}

	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	int k_width = info_ptr->width;
	int k_height = info_ptr->height;
	int k_color_type = info_ptr->color_type;
	int k_bit_depth = info_ptr->bit_depth;

	// palette
	if (k_color_type==PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// expand to 8 bits
	if (k_color_type == PNG_COLOR_TYPE_GRAY && k_bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// Strip 16 bit depth files to 8 bit depth
	if (k_bit_depth == 16)
        png_set_strip_16(png_ptr);

	// grayscale -> RGB
	if (k_color_type == PNG_COLOR_TYPE_GRAY || k_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr))) // Error during read_image
	{
		k_luaerr(L, "loadpng: Error during read_image.");
		return 0;
	}

	// bytes per pixel (3,4)
	int k_bpp = (int)info_ptr->rowbytes / k_width;
	png_bytep* data = (png_bytep*) malloc(sizeof(png_bytep) * k_height);
	for (int y=0; y<k_height; y++)
		data[y] = (png_byte*) malloc(info_ptr->rowbytes);
	
	png_read_image(png_ptr, data);
	fclose(fp);

	// now we have loaded image
	lua_newtable(L);

	// width
	lua_pushnumber(L, k_width);
	lua_setfield(L, -2, "width");

	// height
	lua_pushnumber(L, k_height);
	lua_setfield(L, -2, "height");

	// pixels data
	lua_newtable(L);
	for(size_t x = 0; x < (size_t)k_width; x++)
	{
		lua_pushnumber(L, (int)x+1); // index x
		lua_newtable(L); // value: array with y
		for(size_t y = 0; y < (size_t)k_height; y++)
		{
			BYTE* col = data[y]+x*k_bpp;
			// CSRI_F_BGR_
			BYTE r = col[0];
			BYTE g = col[1];
			BYTE b = col[2];
			DWORD color = (b<<16)|(g<<8)|r;
			lua_pushnumber(L, (int)y+1); // index y
			lua_pushnumber(L, color); // value: color
			lua_settable(L, -3);
		}
		lua_settable(L, -3);
	}
	lua_setfield(L, -2, "color");

	// alpha data
	lua_newtable(L);

	if(k_bpp==4)
	{
		for(size_t x = 0; x < (size_t)k_width; x++)
		{
			lua_pushnumber(L, (int)x+1); // index x
			lua_newtable(L); // value: array with y
			for(size_t y = 0; y < (size_t)k_height; y++)
			{
				BYTE* col = data[y]+x*k_bpp;

				lua_pushnumber(L, (int)y+1); // index y
				lua_pushnumber(L, 255-col[3]); // value: alpha
				lua_settable(L, -3);
			}
			lua_settable(L, -3);
		}
	}
	else
	{
		for(size_t x = 0; x < (size_t)k_width; x++)
		{
			lua_pushnumber(L, (int)x+1); // index x
			lua_newtable(L); // value: array with y
			for(size_t y = 0; y < (size_t)k_height; y++)
			{
				lua_pushnumber(L, (int)y+1); // index y
				lua_pushnumber(L, 0); // value: alpha
				lua_settable(L, -3);
			}
			lua_settable(L, -3);
		}
	}
	lua_setfield(L, -2, "alpha");
	free(data);
	return 1;
}

KMAIN_API int KHlibinit(lua_State* L)
{
	lua_newtable(L); // -1
	lua_pushcfunction(L, lua_kh_render);
	lua_setfield(L, -2, "render");
	lua_pushcfunction(L, lua_kh_outlinetext);
	lua_setfield(L, -2, "text_outline");
	lua_pushcfunction(L, lua_kh_textextent);
	lua_setfield(L, -2, "text_extents");
	lua_pushcfunction(L, lua_kh_loadpng);
	lua_setfield(L, -2, "loadpng");
	lua_setglobal(L, "karahelper");
	return 0;
}