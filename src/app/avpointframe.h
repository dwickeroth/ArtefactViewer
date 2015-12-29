#ifndef AVPOINTFRAME_H
#define AVPOINTFRAME_H
#include "PQMTClient.h"
using namespace PQ_SDK_MultiTouch;

struct AVPointFrame{
    int pf_frame_id;
    int pf_time_stamp;
    int pf_moving_point_count;
    const TouchPoint * pf_moving_point_array;
};
#endif // AVPOINTFRAME_H
