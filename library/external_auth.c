//
// 	Copyright NetFoundry Inc.
//
// 	Licensed under the Apache License, Version 2.0 (the "License");
// 	you may not use this file except in compliance with the License.
// 	You may obtain a copy of the License at
//
// 	https://www.apache.org/licenses/LICENSE-2.0
//
// 	Unless required by applicable law or agreed to in writing, software
// 	distributed under the License is distributed on an "AS IS" BASIS,
// 	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// 	See the License for the specific language governing permissions and
// 	limitations under the License.
//

#include <ziti/ziti_events.h>
#include "zt_internal.h"
#include "oidc.h"

static void ext_oath_cfg_cb(oidc_client_t *oidc, int status, const char *err) {
    ziti_context ztx = oidc->data;
    if (status == 0) {
        ziti_event_t ev = {
                .type = ZitiAuthEvent,
                .auth = {
                        .action = ziti_auth_login_external,
                        .type = "oidc",
                        .detail = oidc->http.host,
                }
        };

        ziti_send_event(ztx, &ev);
    }
}

void ztx_init_external_auth(ziti_context ztx) {
    ziti_jwt_signer *oidc_cfg = ztx->config.id.oidc;
    if (oidc_cfg != NULL) {
        NEWP(oidc, oidc_client_t);
        oidc_client_init(ztx->loop, oidc, oidc_cfg, NULL);
        oidc->data = ztx;
        ztx->ext_auth = oidc;
        oidc_client_configure(oidc, ext_oath_cfg_cb);
    }
}

static void internal_link_cb(oidc_client_t *oidc, const char *url, void *ctx) {
    ziti_context ztx = oidc->data;
    ZITI_LOG(INFO, "received link request: %s", url);
    if (ztx->ext_launch_cb) {
        ztx->ext_launch_cb(ztx, url);
    }
}

static void ext_token_cb(oidc_client_t *oidc, int status, const char *token) {
    ziti_context ztx = oidc->data;
    ZITI_LOG(INFO, "received access token: %d\n%s", status, token);
    ztx->auth_method->set_ext_jwt(ztx->auth_method, token);
    ztx->auth_method->start(ztx->auth_method, ztx_auth_state_cb, ztx);
}

extern int ziti_ext_auth(ziti_context ztx, void (*ziti_ext_launch)(ziti_context, const char* url)) {
    if (ztx->ext_auth == NULL) {
        return ZITI_INVALID_STATE;
    }

    ztx->ext_launch_cb = ziti_ext_launch;
    oidc_client_set_link_cb(ztx->ext_auth, internal_link_cb, NULL);
    oidc_client_start(ztx->ext_auth, ext_token_cb);
    return ZITI_OK;
}

extern int ziti_ext_auth_token(ziti_context ztx, const char *token) {
    if (ztx->auth_method) {
        ztx->auth_method->set_ext_jwt(ztx->auth_method, token);
        return 0;
    }

    return ZITI_INVALID_STATE;
}