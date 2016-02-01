#ifndef AVHAND_H
#define AVHAND_H

struct AVHand{
    bool            isLeft;
    int				hState; //Unknown = 0,  NotTracked = 1, Open = 2,   Closed = 3, Lasso = 4
    double          x;      //x coordinate of the hand where 0 is in the middle of the kinect camera
    double          y;      //y coordinate of the hand where 0 is in the middle of the kinect camera
    double          z;      //z coordinate of the hand where 0 is in the middle of the kinect camera
};
#endif // AVHAND_H
