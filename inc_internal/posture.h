// Copyright (c) 2020-2022.  NetFoundry Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ZITI_SDK_POSTURE_H
#define ZITI_SDK_POSTURE_H

#include <uv.h>
#include "zt_internal.h"
#include "deadline.h"

#ifdef __cplusplus
extern "C" {
#endif

struct posture_checks {
    deadline_t deadline;
    uint64_t send_period;

    // map<type/process_path,response>
    model_map responses;

    // map<type/process_path, is errored
    model_map error_states;

    char *previous_api_session_id;
    char *controller_instance_id;
    bool must_send;
    bool must_send_every_time;

    model_map active_work;
};

void ziti_posture_init(ziti_context ztx, long interval_secs);

void ziti_posture_checks_free(struct posture_checks *pcs);

void ziti_send_posture_data(ziti_context ztx);

bool ziti_service_has_query_with_timeout(ziti_service *service);

#ifdef __cplusplus
}
#endif


#endif //ZITI_SDK_POSTURE_H
