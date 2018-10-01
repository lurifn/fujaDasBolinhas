#include "camera.h"

Camera::Camera ()
{
    projectionMatrix . setToIdentity () ;
    computeViewMatrix () ;
}

void Camera::computeViewMatrix ()
{
    viewMatrix . setToIdentity () ;
    viewMatrix . lookAt ( eye , center , up ) ;
}

void Camera::resizeViewport (int width , int height )
{
    projectionMatrix . setToIdentity () ;
    float aspectRatio = static_cast <float>( width ) / static_cast <float>( height ) ;
    projectionMatrix . perspective (60.0 , aspectRatio , -20.0 , 30.0) ;
}
