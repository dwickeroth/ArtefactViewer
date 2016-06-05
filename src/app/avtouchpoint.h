#ifndef AVTOUCHPOINT_H
#define AVTOUCHPOINT_H

struct AVTouchPoint{
    unsigned short	point_event;//Indicates current action or event of the touch point.
    unsigned short	id;         //use id to distinguish different points on the screen.
    int				x;			//the x-coordinate of the center position of the point.In pixels.
    int				y;			//the y-coordinate of the center position of the point.In pixels.
    unsigned short	dx;		    //the x-width of the touch point.In pixels.
    unsigned short	dy;			//the y-width of the touch point.In pixels.
};
#endif // AVTOUCHPOINT_H
