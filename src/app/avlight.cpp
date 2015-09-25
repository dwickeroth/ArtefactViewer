#include "avlight.h"

AVLight::AVLight()
{
    m_isOn = false;
    m_intensity = 0;
    m_hRotation = 0.0;
    m_vRotation = 0.0;
    m_distanceToOrigin = 10.0;

    calculateTransformation();
}

AVLight::AVLight(bool isOn, int intensity, float hRot, float vRot, float distToOrigin)
    : m_isOn(isOn),
      m_intensity(intensity),
      m_hRotation(hRot),
      m_vRotation(vRot),
      m_distanceToOrigin(distToOrigin)
{
    calculateTransformation();
}

bool AVLight::getIsOn() const
{
    return m_isOn;
}
void AVLight::setIsOn(bool value)
{
    m_isOn = value;
}

int AVLight::getIntensity() const
{
    return m_intensity;
}
void AVLight::setIntensity(int value)
{
    m_intensity = value;
}

float AVLight::getVRotation() const
{
    return m_vRotation;
}
void AVLight::setVRotation(float value)
{
    m_vRotation = value;
    calculateTransformation();
}

float AVLight::getHRotation() const
{
    return m_hRotation;
}
void AVLight::setHRotation(float value)
{
    m_hRotation = value;
    calculateTransformation();
}

float AVLight::getDistanceToOrigin() const
{
    return m_distanceToOrigin;
}
void AVLight::setDistanceToOrigin(float value)
{
    m_distanceToOrigin = value;
    calculateTransformation();
}

QVector3D AVLight::getPosition() const
{
    return m_position;
}
QMatrix4x4 AVLight::getTransformation() const
{
    return m_transformation;
}

void AVLight::calculateTransformation()
{
    m_transformation.setToIdentity();
    m_transformation.rotate(m_hRotation, 0, 1, 0);
    m_transformation.rotate(m_vRotation, 1, 0, 0);
    m_transformation.translate(QVector3D(0, 0, m_distanceToOrigin));
    m_transformation.scale(5);
    m_transformation.rotate(90, 1, 0, 0);
    m_position = m_transformation * QVector3D(0, 0, 0);
}








