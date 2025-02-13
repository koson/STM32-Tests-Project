/**
 * @file
 * MDNS responder
 */

/*
 * Copyright (c) 2015 Verisure Innovation AB
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Erik Ekman <erik.ekman@verisure.com>
 *
 */
#ifndef LWIP_HDR_MDNS_H
#define LWIP_HDR_MDNS_H

#include "lwip/apps/mdns_opts.h"
#include "lwip/netif.h"

#if LWIP_MDNS_RESPONDER

enum mdns_sd_proto
{
   DNSSD_PROTO_UDP = 0,
   DNSSD_PROTO_TCP = 1
};

#   define MDNS_LABEL_MAXLEN 63

struct mdns_host;
struct mdns_service;

/** Callback function to add text to a reply, called when generating the reply */
typedef void (*service_get_txt_fn_t)(struct mdns_service* service, void* txt_userdata);

void mdns_resp_init(void);

err_t mdns_resp_add_netif(struct netif* netif, const char* hostname, u32_t dns_ttl);
err_t mdns_resp_remove_netif(struct netif* netif);

err_t mdns_resp_add_service(struct netif* netif,
                            const char* name,
                            const char* service,
                            enum mdns_sd_proto proto,
                            u16_t port,
                            u32_t dns_ttl,
                            service_get_txt_fn_t txt_fn,
                            void* txt_userdata);
err_t mdns_resp_add_service_txtitem(struct mdns_service* service, const char* txt, u8_t txt_len);
void mdns_resp_netif_settings_changed(struct netif* netif);

#endif /* LWIP_MDNS_RESPONDER */

#endif /* LWIP_HDR_MDNS_H */
