#ifndef OPENGLWIDGET_H
    #define OPENGLWIDGET_H

    #include<QWidget>
    #include<QOpenGLWidget>
    #include<QOpenGLExtraFunctions>
    #include"model.h"
    #include"light.h"
    #include "camera.h"

    class OpenGLWidget : public QOpenGLWidget , protected
    QOpenGLExtraFunctions
    {
        Q_OBJECT

        //é o que é enviado quando alguma coisa acontece
        signals :
            //barrinha de informação
            void statusBarMessage ( QString ) ;

            //conta os pontos
            void somaPonto(QString);

            void cronometro(QString);

            QVector2D teclado ();

        //slot pega o sinal e faz alguma coisa acontecer
        public slots :
            //faz aparecer a janela bunitinha para carregar o arquivo
            void showFileOpenDialog ();
            void giraX(int x);
            void giraY(int y);
            void updateModels();

        public :
            OpenGLWidget ( QWidget *parent = 0) ;

            //ponteiro do tipo modelo (poderia ser um array se tivesse muitos)
            std::unique_ptr<Model[]> models;

            bool verificaColisao();
            int calculaMortes();
            QTime getTime();

            Light light ;
            Camera camera ;

            //soma um ponto quando colide
            int pontos = 0;
            int morrendo = 0; //verifica se estamos morrendo

        protected :
            void initializeGL () ;
            void resizeGL ( int width , int height ) ;
            void paintGL () ;
            void gira(int x, int d);
            void criaModel(QString arq);

            //eventos
            void keyPressEvent(QKeyEvent *event);
            void mouseMoveEvent(QMouseEvent *event);
            void mouseDoubleClickEvent(QMouseEvent *event);
            bool eventFilter(QObject *watched, QEvent *event);

            QTimer *timer;
            QElapsedTimer tempo;

            //quantos models a app possui
            int nModels = 0;
            int maxModels = 20;
            int selModel = 0; //model selecionado
    };
# endif // OPENGLWIDGET_H
