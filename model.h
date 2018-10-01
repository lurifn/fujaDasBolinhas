#ifndef MODEL_H
    #define MODEL_H
    #include <QtOpenGL>
    #include <QOpenGLWidget>
    #include <QOpenGLExtraFunctions>
    #include <QTextStream>
    #include <QFile>
    #include <fstream>
    #include <limits>
    #include <iostream>
    #include <memory>
    #include "material.h"
    #include "trackball.h"

    class Model : public QOpenGLExtraFunctions
    {
        public :
            Model(QOpenGLWidget *_glWidget);
            Model();
            void inicializa(QOpenGLWidget *_glWidget);
            ~Model();

            std::unique_ptr <QVector4D[]> vertices ;
            std::unique_ptr <unsigned int[]> indices ;
            std::unique_ptr <QVector3D[]> normals ;
            std::unique_ptr < QVector2D [] > texCoords ;

            std::vector <GLuint> shaderProgram ;

            QOpenGLWidget * glWidget ;

            unsigned int numVertices ;
            unsigned int numFaces ;

            Material material ;
            int shaderIndex = 0;
            int numShaders ;

            GLuint vao = 0;
            GLuint vboVertices = 0;
            GLuint vboIndices = 0;
            GLuint vboNormals = 0;
            GLuint vboTexCoords = 0;
            GLuint vboCoordText = 0;
            GLuint textureID = 0;
            GLuint texturas[2];

            QMatrix4x4 modelMatrix ;
            QVector3D midPoint ;
            double invDiag ;

            TrackBall trackBall ;

            void createVBOs () ;
            void createShaders () ;
            void destroyVBOs () ;
            void destroyShaders () ;
            void readOFFFile ( const QString &fileName ) ;
            void loadTextures () ;
            void drawModel () ;
            void giraX(int x);
            void giraY(int y);
            void giraZ(int z);
            void criaInimigo();
            void criaPersonagem();
            void createNormals();
            void createTexCoords () ;

            //verifica se está inicializado
            bool inicializado(QOpenGLWidget *_glWidget);
            int personagem = 0; //0 = principal, 1 = inimigo

            //posição do objeto
            double mX = 0;
            double mY = 0;
            double mZ = 0;

            bool colidiu = false;

        private:
            int x = 0;
            int y = 0;
            int z = 0;
            const QString *fileName = NULL;
    };
# endif // MODEL_H
