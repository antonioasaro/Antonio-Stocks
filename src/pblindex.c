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
TextLayer textLayer[2][NUM_LINES];

void set_display_fail(char *text) {
    text_layer_set_text(&textLayer[0][0], "Failed");
    text_layer_set_text(&textLayer[0][1], text);
    for (int i=2; i<NUM_LINES; i++)
        text_layer_set_text(&textLayer[0][i], "");
    for (int i=0; i<NUM_LINES; i++)
        text_layer_set_text(&textLayer[1][i], "");
}

void request_quotes() {
    DictionaryIterator *body;
//// #error Set URL below
    // http://nnnn/nnnn.names should return something like (not over 76 bytes)
    // {"0":"OMXS30","1":"Dow Jones","2":"Nasdaq","3":"DAX","4":"Nikkei"}
    if (http_out_get("http://antonio.comyr.com", false, PBLINDEX_COOKIE, &body) != HTTP_OK ||
//    if (http_out_get("http://192.168.0.182/Pebble/Katharine-Weather", false, PBLINDEX_COOKIE, &body) != HTTP_OK ||
//    if (http_out_get("http://192.168.0.182/Pebble/Antonio-Stocks", false, PBLINDEX_COOKIE, &body) != HTTP_OK ||
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

	text_layer_set_text(&textLayer[0][0], "Remote - success!!!!");
	text_layer_set_text(&textLayer[0][1], "");
    text_layer_set_text(&textLayer[0][3], "");
	
	static char name[3][16];  
    for (int i=0; i<3; i++) {
		Tuple *quotes = dict_find(dict,  i+1);
		if (quotes) {
			if (i==0) { memcpy(name[i], quotes->value->cstring, quotes->length); 
			} else {	memcpy(name[i], itoa(quotes->value->int32), 4);}	
			text_layer_set_text(&textLayer[0][i+2], name[i]);
		}
	}
}

void reconnect(void *ctx) {
    request_quotes();
}

void init_handler(AppContextRef ctx) {
    window_init(&window, "Antonio Stocks");
    window_set_background_color(&window, GColorBlack);
    window_stack_push(&window, true);
    
    for (int i=0; i<NUM_LINES; i++) {
        text_layer_init(&textLayer[0][i], GRect(5, 7+i*30, 144-5-COLUMN2_WIDTH, 30));
        text_layer_init(&textLayer[1][i], GRect(144-COLUMN2_WIDTH, 7+i*30, COLUMN2_WIDTH, 30));
        text_layer_set_font(&textLayer[0][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_font(&textLayer[1][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_background_color(&textLayer[0][i], GColorBlack);
        text_layer_set_background_color(&textLayer[1][i], GColorBlack);
        text_layer_set_text_color(&textLayer[0][i], GColorWhite);
        text_layer_set_text_color(&textLayer[1][i], GColorWhite);
        text_layer_set_text_alignment(&textLayer[1][i], GTextAlignmentRight);
    }
    
    text_layer_set_text(&textLayer[0][0], "Stocks");
    text_layer_set_text(&textLayer[0][1], "by Antonio");
    text_layer_set_text(&textLayer[0][3], "loading...");
    
    for (int i=0; i<NUM_LINES; i++) {
        layer_add_child(&window.layer, &textLayer[0][i].layer);
        layer_add_child(&window.layer, &textLayer[1][i].layer);
    }
    
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