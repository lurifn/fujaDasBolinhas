#ifndef MATERIAL_H
#define MATERIAL_H
#include <QVector4D>

class Material
{
    public :
        Material () ;
        QVector4D ambient = QVector4D (0.06 , 0.06 , 0.06 , 1.0) ;
        QVector4D diffuse = QVector4D (0.7 , 0.7 , 0.8 , 1.0) ;
        QVector4D specular = QVector4D (1.0 , 1.0 , 1.0 , 1.0) ;
        float shininess = 20.0;
};
#endif // MATERIAL_H
