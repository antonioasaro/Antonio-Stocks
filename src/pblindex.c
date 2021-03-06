/* ===========================================================================
 
 Copyright (c) 2013 Edward Patel
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 =========================================================================== */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "resource_ids.auto.h"

#include "util.h"
#include "http.h"
#include "httpcapture.h"

#define MAKE_SCREENSHOT 0

PBL_APP_INFO(HTTP_UUID,
             "Antonio Stocks", "Antonio Asaro",
             1, 0,
             RESOURCE_ID_WATCH_MENU_ICON,
             APP_INFO_WATCH_FACE);   // Based off of "pbl-index" by "Edward Patel"

#define NUM_LINES 5
#define COLUMN2_WIDTH 65

// #error Set values below, should be different, one unique for each request
#define PBLINDEX_COOKIE 9997
//// #define PBLINDEX_VALUES_COOKIE some-unique-value

Window window;
TextLayer textLayer[3][NUM_LINES];

void set_display_fail(char *text) {
    text_layer_set_text(&textLayer[0][0], "Failed");
    text_layer_set_text(&textLayer[0][1], text);
	for (int i=0; i<NUM_LINES; i++) {
		if (i>1) text_layer_set_text(&textLayer[0][i], "");
        text_layer_set_text(&textLayer[1][i], "");
        text_layer_set_text(&textLayer[2][i], "");
	}
}

char *ftoa(int i, bool j)
{
  	static char buf[8];
	
	strcpy(buf, "");
	if (!j) strcat(buf, "$");
	if (i<0) { i = -i; strcat(buf, "-"); }
	if (!j) strcat(buf, itoa(i/100)); else strcat(buf, itoa(i/10));
	strcat(buf, ".");
	if (!j) strcat(buf, itoa(i%100)); else strcat(buf, itoa(i%10));
	if (j) strcat(buf, "%");
    return(buf);
}

// Stock List is in the form ?stock1=name1& ... Must have 2 names!!
void request_quotes() {
    DictionaryIterator *body;
    if (http_out_get("http://antonioasaro.site50.net/?stock1=AMD&stock2=INTC", false, PBLINDEX_COOKIE, &body) != HTTP_OK ||
        http_out_send() != HTTP_OK) {
        set_display_fail("QT fail()");
    }
}

void failed(int32_t cookie, int http_status, void *ctx) {
    if (cookie == 0 ||
		cookie == PBLINDEX_COOKIE) {
        set_display_fail("BT fail()");
    }
}

void success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx) {
    if (cookie != PBLINDEX_COOKIE) return;
	text_layer_set_text(&textLayer[0][0], "Success!!");
	text_layer_set_text(&textLayer[0][1], "");

	static char stock1[3][16];  
	static char stock2[3][16];  
    for (int i=0; i<3+3; i++) {
		Tuple *quotes = dict_find(dict,  i+1);
		if (quotes) {
			if (i==0) memcpy(stock1[i-0], quotes->value->cstring, quotes->length); 
			if (i==1) memcpy(stock1[i-0], ftoa(quotes->value->int32, 0), 8);	
			if (i==2) memcpy(stock1[i-0], ftoa(quotes->value->int32, 1), 8);	
			if (i==3) memcpy(stock2[i-3], quotes->value->cstring, quotes->length); 
			if (i==4) memcpy(stock2[i-3], ftoa(quotes->value->int32, 0), 8);	
			if (i==5) memcpy(stock2[i-3], ftoa(quotes->value->int32, 1), 8);	
			if (i<3) text_layer_set_text(&textLayer[i-0][2], stock1[i-0]);
			if (i>2) text_layer_set_text(&textLayer[i-3][3], stock2[i-3]);
		}
	}
    light_enable_interaction();
}

void reconnect(void *ctx) {
    request_quotes();
}

void init_handler(AppContextRef ctx) {
    window_init(&window, "Antonio Stocks");
    window_set_background_color(&window, GColorBlack);
    window_stack_push(&window, true);
    
    for (int i=0; i<NUM_LINES; i++) {
        if (i<2) text_layer_init(&textLayer[0][i], GRect(5+00, 7+i*30, 135, 30));
        if (i>1) text_layer_init(&textLayer[0][i], GRect(5+00, 7+i*30, 45, 30));
        text_layer_init(&textLayer[1][i], GRect(5+45, 7+i*30, 45, 30));
        text_layer_init(&textLayer[2][i], GRect(5+90, 7+i*30, 45, 30));
        text_layer_set_font(&textLayer[0][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_font(&textLayer[1][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_font(&textLayer[2][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_background_color(&textLayer[0][i], GColorBlack);
        text_layer_set_background_color(&textLayer[1][i], GColorBlack);
        text_layer_set_background_color(&textLayer[2][i], GColorBlack);
        text_layer_set_text_color(&textLayer[0][i], GColorWhite);
        text_layer_set_text_color(&textLayer[1][i], GColorWhite);
        text_layer_set_text_color(&textLayer[2][i], GColorWhite);
        text_layer_set_text_alignment(&textLayer[1][i], GTextAlignmentLeft);
        text_layer_set_text_alignment(&textLayer[2][i], GTextAlignmentRight);
        layer_add_child(&window.layer, &textLayer[0][i].layer);
        if (i>1) layer_add_child(&window.layer, &textLayer[1][i].layer);
        if (i>1) layer_add_child(&window.layer, &textLayer[2][i].layer);
    }
	text_layer_set_text(&textLayer[0][0], "Antonio Stocks");
	text_layer_set_text(&textLayer[0][1], "loading ...");
    
    http_set_app_id(PBLINDEX_COOKIE); 
    http_register_callbacks((HTTPCallbacks){
        .failure = failed,
        .success = success,
        .reconnect = reconnect,
    }, NULL);

#if MAKE_SCREENSHOT
	http_capture_init(ctx);
#endif
	
	request_quotes();
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &init_handler,
        .messaging_info = {
            .buffer_sizes = {
                .inbound = 124, // 124 safe for Android
                .outbound = 256,
            }
        },
    };
    
#if MAKE_SCREENSHOT
	http_capture_main(&handlers);
#endif
	
    app_event_loop(params, &handlers);
}