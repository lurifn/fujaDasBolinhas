#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget * parent):QOpenGLWidget(parent)
{

}

void OpenGLWidget::initializeGL ()
{
    initializeOpenGLFunctions () ;

    qDebug (" OpenGL version : %s", glGetString ( GL_VERSION ) ) ;
    qDebug (" GLSL %s", glGetString ( GL_SHADING_LANGUAGE_VERSION ) ) ;

    //testa a profundidade para saber que para apresentar o objeto final
    //ele tem que apresentar quem está mais próxima da câmera, não o último que foi desenhado
    glEnable ( GL_DEPTH_TEST ) ;

    //cria o vetor de models
    models = std::make_unique <Model[]>(maxModels);
}

//cria um novo model
void OpenGLWidget::criaModel(QString arq)
{
    //inicializa o model
    models[nModels].inicializa(this);

    if (nModels == 0)
        //indica que é um personagem do bem
        models[nModels].criaPersonagem();

    qDebug("\nModel %d:", nModels);

    //passa o ponteiro para a classe
    models[nModels].readOFFFile ( arq ) ;

    //seta a trackball
    models[nModels].trackBall . resizeViewport ( width () , height () ) ;

    //emite um sinal com o número de vértices e de faces
    emit statusBarMessage ( QString ("Vertices:\%1, Faces:\%2").arg ( models[nModels].numVertices ).arg ( models[nModels].numFaces ) ) ;

    //indica que o model foi criado
    nModels++;
}

void OpenGLWidget::resizeGL (int w , int h )
{
    glViewport (0 ,0 ,w , h ) ;
}

void OpenGLWidget::paintGL ()
{
    //limpa o buffer de cor e de profundidade
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

    QPainter painter(this);

    //verifica se estamos morrendo
    if(morrendo)
    {
        //incrementa o tempo que estamos morrendo
        morrendo++;

        //pinta a tela de vermelho
        painter.fillRect(rect(), Qt::red);

        //só fica morrendo por uns frames, depois reseta
        if(morrendo >= 10)
            morrendo = 0;
    }
    else
    {
        //verifica se o jogo começou
        if(models[0].inicializado(this))
        {
            int i;

            //desenha todos os models
            for(i=0;i<maxModels;i++)
            {
                //se tem modelo
                if (models[i].inicializado(this))
                {
                    //verifica se precisa desenhar o modelo
                    if(!models[i].colidiu)
                    {
                        //qDebug("%d", models[i].shaderProgram[models[i].shaderIndex]);
                        //seta luz do model
                        int shaderProgramID = models[i].shaderProgram[models[i].shaderIndex];
                        QVector4D ambientProduct = light.ambient * models[i].material.ambient ;
                        QVector4D diffuseProduct = light.diffuse * models[i].material.diffuse ;
                        QVector4D specularProduct = light.specular * models[i].material.specular ;

                        //seta propriedades da luz
                        GLuint locProjection = glGetUniformLocation ( shaderProgramID , "projection") ;
                        GLuint locView = glGetUniformLocation ( shaderProgramID , "view") ;
                        GLuint locLightPosition = glGetUniformLocation ( shaderProgramID , "lightPosition") ;

                        GLuint locAmbientProduct = glGetUniformLocation ( shaderProgramID ,"ambientProduct") ;
                        GLuint locDiffuseProduct = glGetUniformLocation ( shaderProgramID ,"diffuseProduct") ;
                        GLuint locSpecularProduct = glGetUniformLocation ( shaderProgramID ,"specularProduct") ;
                        GLuint locShininess = glGetUniformLocation ( shaderProgramID , "shininess") ;

                        glUseProgram ( shaderProgramID ) ;
                        glUniformMatrix4fv ( locProjection , 1 , GL_FALSE , camera.projectionMatrix.data () ) ;

                        //seta a camera
                        glUniformMatrix4fv ( locView , 1 , GL_FALSE , camera.viewMatrix.data () ) ;
                        glUniform4fv ( locLightPosition , 1 , &( light.position[0]) ) ;
                        glUniform4fv ( locAmbientProduct , 1 , &( ambientProduct [0]) ) ;
                        glUniform4fv ( locDiffuseProduct , 1 , &( diffuseProduct [0]) ) ;
                        glUniform4fv ( locSpecularProduct , 1 , &( specularProduct [0]) ) ;
                        glUniform1f ( locShininess , models[i].material.shininess ) ;
                    }

                    //desenha o modelo
                    models[i].drawModel() ;
                }
            }
        }
        else
        {
            painter.setPen(Qt::green);
            painter.setFont(QFont("Arial", 50));
            painter.drawText(rect(), Qt::AlignCenter, "Clique 2x para começar :)");
        }
    }
}

//inicia o jogo
void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    //verifica se o jogo já começou
    if(!models[0].inicializado(this))
    {
        //cria o model do personagem principal
        criaModel(QString(QCoreApplication::applicationDirPath()+"/formas/cube.off"));

        for(int i=1;i< maxModels; i++)
        {
            //cria os inimigos
            criaModel(QString(QCoreApplication::applicationDirPath()+"/formas/sphere.off"));

            //coloca em uma posição aleatória
            models[i].criaInimigo();
        }

        //instancia o timer
        timer = new QTimer(this);

        update();

        //começa o timer
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateModels()));
        timer->start(10);
        tempo.start();

        qApp->installEventFilter(this);
    }
}

void OpenGLWidget::updateModels()
{
    QTime tempo = getTime();

    if(models[maxModels - 1].inicializado(this))
    {
        //move todos os inimigos no eixo x
        for(int i =1;i<maxModels;i++)
        {
            models[i].mX += 0.2;
        }

        update();

        //adiciona um ponto se tiver colidido
        if (verificaColisao())
        {
            //soma um ponto
            pontos++;

            //indica que estamos morrendo
            morrendo = 1;

            //emitie o ponto
            emit somaPonto(QString("Mortes: %1").arg(QString::number(pontos)));
        }
    }

    emit cronometro(QString("%1:%2:%3").arg(tempo.hour(), 2, 10, QChar('0')).arg(tempo.minute(), 2, 10, QChar('0')).arg(tempo.second(), 2, 10, QChar('0')));
}

//verifica se o personagem principal colidiu com algum inimigo
bool OpenGLWidget::verificaColisao()
{
    for(int i = 1; i<maxModels; i++)
    {
        //só roda para inimigos que ainda não colidiram para não
        //colidir infinitas vezes sem parar
        if(!models[i].colidiu)
        {
            //modelMatrix(0,3) = mX;
            //modelMatrix(1,3) = mY;

            //verifica o começo do range de x
            double axComeco = models[i].modelMatrix(0,3) - (models[i].invDiag/2);
            double bxComeco = models[0].modelMatrix(0,3) - (models[0].invDiag/2);

            //verifica o fim do range de x
            double axFim = models[i].modelMatrix(0,3) + (models[i].invDiag/2);
            double bxFim = models[0].modelMatrix(0,3) + (models[0].invDiag/2);

            //verifica o começo do range de y
            double ayComeco = models[i].modelMatrix(1,3) - (models[i].invDiag/2);
            double byComeco = models[0].modelMatrix(1,3) - (models[0].invDiag/2);

            //verifica o fim do range de y
            double ayFim = models[i].modelMatrix(1,3) + (models[i].invDiag/2);
            double byFim = models[0].modelMatrix(1,3) + (models[0].invDiag/2);

            //verifica se estão colidindo
            if((axComeco <= bxFim) && (axFim >= bxComeco) && (ayComeco <= byFim) && (ayFim >= byComeco))
            {
                //seta o inimigo como colidido
                models[i].colidiu = true;

                return true;
            }
        }
    }

    return false;
}

void OpenGLWidget::showFileOpenDialog()
{
    QByteArray fileFormat = "off";
    QString fileName = QFileDialog::getOpenFileName (this ,
                                                     " Open File ", QDir::homePath () ,
                                                     QString (" \%1 Files (*.\%2) ").arg ( QString ( fileFormat.toUpper () ) ).arg ( QString ( fileFormat ) ) , 0
                                                 # ifdef Q_OS_LINUX
                                                     , QFileDialog::DontUseNativeDialog
                                                 # endif
                                                     ) ;

    qDebug() << fileName;
    if (!fileName.isEmpty() )
    {
        //cria um modelo que vale para todos os métodos da classe
        //model = std::make_shared <Model>( this ) ;

        //passa o ponteiro para a classe
        //model[]->readOFFFile ( fileName ) ;

        //cria um model com o arquivo escolhido
        criaModel(fileName);

        //emite um sinal com o número de vértices e de faces
        emit statusBarMessage ( QString ("Vertices:\%1, Faces:\%2").arg ( models[nModels].numVertices ).arg ( models[nModels].numFaces ) ) ;
    }

    //chama o paintgl
    update () ;
}

//gira o objeto x graus no eixo d
void OpenGLWidget::gira(int x, int d)
{
    //se não tem modelo, retorna
    if (!models )
        return ;

    if(d == 0)
    {
        models[selModel].giraX(x);
    }
    else
    {
        models[selModel].giraY(x);
    }

    //chama o paintgl
    update () ;
}

//gira o objeto x graus no eixo d
void OpenGLWidget::giraX(int x)
{
    this->gira(x, 0);
}

//gira o objeto x graus no eixo d
void OpenGLWidget::giraY(int y)
{
    this->gira(y, 1);
}

void OpenGLWidget :: keyPressEvent ( QKeyEvent * event )
{
    switch ( event-> key () )
    {
    case Qt::Key_Escape :
        qApp-> quit () ;
        break;

    case Qt::Key_Up :
        this->gira(1, 0);
        break;

    case Qt::Key_Down:
        this->gira(-1,0);
        break;

    case Qt::Key_Left:
        this->gira(1, -1);
        break;

    case Qt::Key_Right:
        this->gira(-1, 1);
        break;
    }
}

//quando o mouse se move
void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint posicao = event->pos();

    double xx = (posicao.x() - (width()/2));
    double yy = height()/2 - posicao.y();

    xx = xx/width() * 10;
    yy = yy/height() * 10;

    //movimenta o model principal
    models[0].mX = xx;
    models[0].mY = yy;

    //seta a luz para seguir o model principal
    light.position = QVector4D(xx, yy, 2, 0);

    //update();
}

bool OpenGLWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEvent(mouseEvent);
    }

    return false;
}

//retorna o tempo
QTime OpenGLWidget::getTime()
{
    return QTime(0,0).addMSecs(tempo.elapsed());
}
