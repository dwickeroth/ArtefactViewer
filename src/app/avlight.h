#ifndef AVLIGHT_H
#define AVLIGHT_H

#include <QVector3D>
#include <QMatrix4x4>

class AVLight
{
public:
    AVLight();
    AVLight(bool isOn, int intensity, float hRot, float vRot, float distToOrigin);


    bool getIsOn() const;
    void setIsOn(bool value);

    int getIntensity() const;
    void setIntensity(int value);

    float getVRotation() const;
    void setVRotation(float value);

    float getHRotation() const;
    void setHRotation(float value);

    float getDistanceToOrigin() const;
    void setDistanceToOrigin(float value);

    QVector3D getPosition() const;

    QMatrix4x4 getTransformation() const;

private:
    //private members with getters and setters
    bool m_isOn;
    int m_intensity;
    float m_hRotation;
    float m_vRotation;
    float m_distanceToOrigin;

    //private members with getters only
    QVector3D m_position;
    QMatrix4x4 m_transformation;

    void calculateTransformation();

};

#endif // AVLIGHT_H
