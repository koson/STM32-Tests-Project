/**
 * \addtogroup exampleapps
 * @{
 */

/**
 * \defgroup httpd Web server
 * @{
 *
 * The uIP web server is a very simplistic implementation of an HTTP
 * server. It can serve web pages and files from a read-only ROM
 * filesystem, and provides a very small scripting language.
 *
 * The script language is very simple and works as follows. Each
 * script line starts with a command character, either "i", "t", "c",
 * "#" or ".".  The "i" command tells the script interpreter to
 * "include" a file from the virtual file system and output it to the
 * web browser. The "t" command should be followed by a line of text
 * that is to be output to the browser. The "c" command is used to
 * call one of the C functions from the httpd-cgi.c file. A line that
 * starts with a "#" is ignored (i.e., the "#" denotes a comment), and
 * the "." denotes the last script line.
 *
 * The script that produces the file statistics page looks somewhat
 * like this:
 *
 \code
i /header.html
t <h1>File statistics</h1><br><table width="100%">
t <tr><td><a href="/index.html">/index.html</a></td><td>
c a /index.html
t </td></tr> <tr><td><a href="/cgi/files">/cgi/files</a></td><td>
c a /cgi/files
t </td></tr> <tr><td><a href="/cgi/tcp">/cgi/tcp</a></td><td>
c a /cgi/tcp
t </td></tr> <tr><td><a href="/404.html">/404.html</a></td><td>
c a /404.html
t </td></tr></table>
i /footer.plain
.
 \endcode
 *
 */

/**
 * \file
 * HTTP server.
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: httpd.c 60080 2012-12-07 14:07:49Z anderslu $
 *
 */

#include "includes.h"
#include "stm32f10x.h"
#include "uip.h"
#include "httpd.h"
#include "fs.h"
#include "fsdata.h"
#include "cgi.h"

#ifndef NULL
#define NULL (void*)0
#endif

/* The HTTP server states: */
#define HTTP_NOGET 0
#define HTTP_FILE 1
#define HTTP_TEXT 2
#define HTTP_FUNC 3
#define HTTP_END 4

#ifdef UIP_DEBUG
#include <stdio.h>
#define PRINT(x) printf("%s", x)
#define PRINTLN(x) printf("%s\n", x)
#else /* UIP_DEBUG */
#define PRINT(x)
#define PRINTLN(x)
#endif /* DEBUG */

struct httpd_state* hs;

extern const struct fsdata_file file_index_html;
extern const struct fsdata_file file_404_html;
extern const struct fsdata_file_noconst file_STM32_StatusBar_html;

static void next_scriptline(void);
static void next_scriptstate(void);

#define ISO_G 0x47
#define ISO_E 0x45
#define ISO_T 0x54
#define ISO_slash 0x2f
#define ISO_c 0x63
#define ISO_g 0x67
#define ISO_i 0x69
#define ISO_space 0x20
#define ISO_nl 0x0a
#define ISO_cr 0x0d
#define ISO_a 0x61
#define ISO_t 0x74
#define ISO_hash 0x23
#define ISO_period 0x2e

char dataPresent;
char Digit1 = 0, Digit2 = 0, Digit3 = 0;
int ADCVal = 0;

/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the web server.
 *
 * Starts to listen for incoming connection requests on TCP port 80.
 */
/*-----------------------------------------------------------------------------------*/
void httpd_init(void)
{
    fs_init();

    /* Listen to port 80. */
    uip_listen(HTONS(80));

    dataPresent = 0;
}
/*-----------------------------------------------------------------------------------*/
void httpd_appcall(void)
{
    struct fs_file fsfile;

    u8_t i;

    switch(uip_conn->lport) {
            /* This is the web server: */
        case HTONS(80):
            /* Pick out the application state from the uip_conn structure. */
            hs = (struct httpd_state*)(uip_conn->appstate);

            /* We use the uip_ test functions to deduce why we were
         called. If uip_connected() is non-zero, we were called
         because a remote host has connected to us. If
         uip_newdata() is non-zero, we were called because the
         remote host has sent us new data, and if uip_acked() is
         non-zero, the remote host has acknowledged the data we
         previously sent to it. */
            if(uip_connected()) {
                /* Since we have just been connected with the remote host, we
            reset the state for this connection. The ->count variable
            contains the amount of data that is yet to be sent to the
            remote host, and the ->state is set to HTTP_NOGET to signal
            that we haven't received any HTTP GET request for this
            connection yet. */
                hs->state = HTTP_NOGET;
                hs->count = 0;
                return;
            }
            else if(uip_poll()) {
                /* If we are polled 200 times, we abort the connection. This is
            because we don't want connections lingering indefinately in
            the system. */
                if(hs->count++ >= 200) {
                    //	uip_abort();
                }
                return;
            }
            else if(uip_newdata() && hs->state == HTTP_NOGET) {
                /* This is the first data we receive, and it should contain a
       GET. */

                /* Check for GET. */
                if(uip_appdata[0] != ISO_G || uip_appdata[1] != ISO_E || uip_appdata[2] != ISO_T ||
                    uip_appdata[3] != ISO_space) {
                    /* If it isn't a GET, we abort the connection. */
                    uip_abort();
                    return;
                }

                /* Find the file we are looking for. */
                for(i = 4; i < 40; ++i) {
                    if(uip_appdata[i] == ISO_space || uip_appdata[i] == ISO_cr || uip_appdata[i] == ISO_nl) {
                        uip_appdata[i] = 0;
                        break;
                    }
                }

                PRINT("request for file ");
                PRINTLN(&uip_appdata[4]);

                /* Check for a request for "/". */
                if(uip_appdata[4] == ISO_slash && uip_appdata[5] == 0) {
                    fs_open(file_index_html.name, &fsfile);
                }
                else {
                    if(!fs_open((const char*)&uip_appdata[4], &fsfile)) {
                        PRINTLN("couldn't open file");
                        fs_open(file_404_html.name, &fsfile);
                    }
                }

                if(uip_appdata[4] == ISO_slash && uip_appdata[5] == ISO_c && uip_appdata[6] == ISO_g &&
                    uip_appdata[7] == ISO_i && uip_appdata[8] == ISO_slash) {
                    /* If the request is for a file that starts with "/cgi/", we
                    prepare for invoking a script. */
                    hs->script = fsfile.data;
                    next_scriptstate();
                }
                else {
                    hs->script = NULL;
                    /* The web server is now no longer in the HTTP_NOGET state, but
               in the HTTP_FILE state since is has now got the GET from
               the client and will start transmitting the file. */
                    hs->state = HTTP_FILE;

                    /* Point the file pointers in the connection state to point to
               the first byte of the file. */
                    hs->dataptr = fsfile.data;
                    hs->count = fsfile.len;
                }
            }

            if(hs->state != HTTP_FUNC) {
                /* Check if the client (remote end) has acknowledged any data that
       we've previously sent. If so, we move the file pointer further
       into the file and send back more data. If we are out of data to
       send, we close the connection. */
                if(uip_acked()) {
                    if(hs->count >= uip_conn->len) {
                        hs->count -= uip_conn->len;
                        hs->dataptr += uip_conn->len;
                    }
                    else {
                        hs->count = 0;
                    }

                    if(hs->count == 0) {
                        if(hs->script != NULL) {
                            next_scriptline();
                            next_scriptstate();
                        }
                        else {
                            uip_close();
                        }
                    }
                }
            }
            else {
                /* Call the CGI function. */
                if(cgitab[hs->script[2] - ISO_a](uip_acked())) {
                    /* If the function returns non-zero, we jump to the next line
                    in the script. */
                    next_scriptline();
                    next_scriptstate();
                }
            }

            if(hs->state != HTTP_FUNC && !uip_poll()) {
                /* Send a piece of data, but not more than the MSS of the
       connection. */
                uip_send((u8_t*)hs->dataptr, hs->count);

                dataPresent = 1;
            }

            /* Finally, return to uIP. Our outgoing packet will soon be on its
         way... */
            return;

        default:
            /* Should never happen. */
            uip_abort();
            break;
    }
}
/*-----------------------------------------------------------------------------------*/
/* next_scriptline():
 *
 * Reads the script until it finds a newline. */
static void next_scriptline(void)
{
    /* Loop until we find a newline character. */
    do {
        ++(hs->script);
    } while(hs->script[0] != ISO_nl);

    /* Eat up the newline as well. */
    ++(hs->script);
}
/*-----------------------------------------------------------------------------------*/
/* next_sciptstate:
 *
 * Reads one line of script and decides what to do next.
 */
static void next_scriptstate(void)
{
    struct fs_file fsfile;
    u8_t i;

again:
    switch(hs->script[0]) {
        case ISO_t:
            /* Send a text string. */
            hs->state = HTTP_TEXT;
            hs->dataptr = &hs->script[2];

            /* Calculate length of string. */
            for(i = 0; hs->dataptr[i] != ISO_nl; ++i)
                ;
            hs->count = i;
            break;
        case ISO_c:
            /* Call a function. */
            hs->state = HTTP_FUNC;
            hs->dataptr = NULL;
            hs->count = 0;
            cgitab[hs->script[2] - ISO_a](0);
            break;
        case ISO_i:
            /* Include a file. */
            hs->state = HTTP_FILE;
            if(!fs_open(&hs->script[2], &fsfile)) {
                uip_abort();
            }
            hs->dataptr = fsfile.data;
            hs->count = fsfile.len;
            break;
        case ISO_hash:
            /* Comment line. */
            next_scriptline();
            goto again;
            // break;
        case ISO_period:
            /* End of script. */
            hs->state = HTTP_END;
            uip_close();
            break;
        default:
            uip_abort();
            break;
    }
}
/*-----------------------------------------------------------------------------------*/
/** @} */
/** @} */
