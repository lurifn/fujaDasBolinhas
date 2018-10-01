//classe que armazena informações referentes ao objeto 3d
#include "model.h"

//recebe ponteiro para o widget mãe
Model::Model(QOpenGLWidget *_glWidget)
{
    glWidget = _glWidget;
    glWidget->makeCurrent();

    //ativa as funções do opengl para esta classse
    initializeOpenGLFunctions();

    //seta o número de shaders
    numShaders = 1;
}

//nao faz nada
Model::Model()
{
}

//inicializa certo
void Model::inicializa(QOpenGLWidget *_glWidget)
{
    glWidget = _glWidget;
    glWidget->makeCurrent();

    //ativa as funções do opengl para esta classse
    initializeOpenGLFunctions();

    //adiciona as texturas
    loadTextures();
}

//destrutor versão 3d
Model::~Model()
{
    destroyVBOs();
    destroyShaders();
}

void Model::destroyVBOs()
{
    glDeleteBuffers (1 , &vboVertices ) ;
    glDeleteBuffers (1 , &vboCoordText ) ;
    glDeleteBuffers (1 , &vboIndices ) ;
    glDeleteVertexArrays (1 , &vao ) ;
    glDeleteBuffers (1 , & vboTexCoords ) ;
    glDeleteVertexArrays (1 , & vao ) ;

    vboVertices = 0;
    vboIndices = 0;
    vboCoordText = 0;
    vboTexCoords = 0;

    vao = 0;
}

void Model::destroyShaders()
{
    for ( GLuint shaderProgramID:shaderProgram )
    {
        glDeleteProgram ( shaderProgramID ) ;
    }

    shaderProgram.clear () ;
}

//pega os vértices e os indices lidos no arquivo e mandam pros buffers
//quem lê esses buffers são os shaders
void Model::createVBOs ()
{
    glWidget->makeCurrent();

    destroyVBOs () ;

    glGenVertexArrays (1 , &vao ) ;
    glBindVertexArray ( vao ) ;

    //cria buffers
    glGenBuffers (1 , &vboVertices ) ;
    glBindBuffer ( GL_ARRAY_BUFFER , vboVertices ) ;
    glBufferData ( GL_ARRAY_BUFFER , numVertices * sizeof ( QVector4D ), vertices.get() , GL_STATIC_DRAW ) ;

    //coloca na location 0
    glVertexAttribPointer (0 , 4 , GL_FLOAT , GL_FALSE , 0 , nullptr ) ;
    glEnableVertexAttribArray (0) ;

    //buffers das normais
    glGenBuffers (1 , &vboNormals ) ;
    glBindBuffer ( GL_ARRAY_BUFFER , vboNormals ) ;
    glBufferData ( GL_ARRAY_BUFFER , numVertices * sizeof ( QVector3D ), normals . get () , GL_STATIC_DRAW ) ;
    glVertexAttribPointer (1 , 3 , GL_FLOAT , GL_FALSE , 0 , nullptr ) ;
    glEnableVertexAttribArray (1) ;

    //buffers dos índices
    glGenBuffers (1 , &vboIndices ) ;
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , vboIndices ) ;
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER , numFaces * 3 * sizeof (unsigned int) , indices.get() , GL_STATIC_DRAW ) ;

    //buffers da textura
    glGenBuffers (1 , &vboTexCoords ) ;
    glBindBuffer ( GL_ARRAY_BUFFER , vboTexCoords ) ;
    glBufferData ( GL_ARRAY_BUFFER , numVertices * sizeof ( QVector2D ), texCoords . get() , GL_STATIC_DRAW ) ;
    glBindBuffer ( GL_ARRAY_BUFFER , vboTexCoords ) ;
    glVertexAttribPointer (2 , 2 , GL_FLOAT , GL_FALSE , 0 , nullptr ) ;
    glEnableVertexAttribArray (2) ;
    texCoords . reset () ;
}

void Model::createNormals()
{
    normals = std::make_unique<QVector3D[]>(numVertices);

    for ( unsigned int i = 0; i < numFaces ; ++i )
    {
        unsigned int a , b , c ;

        //captura os índices da face
        a = indices [ i * 3 + 0];
        b = indices [ i * 3 + 1];
        c = indices [ i * 3 + 2];

        //calcula a normal da face
        QVector3D normal = QVector3D::normal(vertices[a].toVector3D(),vertices[b].toVector3D(),vertices[c].toVector3D());

        //soma em cada vértice desta face
        normals[a] += normal;
        normals[b] += normal;
        normals[c] += normal;
    }

    //normaliza tudo depois
    for ( unsigned int i = 0; i < numVertices ; ++i )
    {
        normals[i].normalize();
    }
}

//cada modelo vai ter um jeito de se desenhar diferente, aqui ele é descrito
//chama sempre pelo paintgl
void Model::drawModel()
{
    //se for inimigo e já tiver desaparecido, reseta o local de nascimento dele
    if((mX > 11) && (personagem == 1))
        criaInimigo();

    //verifica se precisa desenhar o model
    if(!colidiu)
    {
        //matriz de transformação de matriz em uma matriz identidade
        modelMatrix.setToIdentity ();

        //faz a identidade vezes a matriz escala uniforme xyz
        modelMatrix.scale ( invDiag , invDiag , invDiag ) ;

        modelMatrix.translate (-midPoint) ;

        //seta a posição da matriz
        //modelMatrix(0,3) = mX;
        //modelMatrix(1,3) = mY;
        modelMatrix.translate(mX, mY, mZ);

        //seta para onde o objeto deve rodar
        modelMatrix.rotate(this->x, 1, 0, 0);
        modelMatrix.rotate(this->y, 0, 1, 0);
        modelMatrix.rotate(this->z, 1, 1, 1);

        if(!personagem)
            modelMatrix.rotate(90, 1, 0, 0);

        GLuint locModel = 0;
        GLuint locNormalMatrix = 0;
        GLuint locShininess = 0;
        GLuint locColorTexture = 0;

        //seleciona a textura certa do personagem
        glActiveTexture ( GL_TEXTURE + personagem ) ;

        //aplica a textura
        locColorTexture = glGetUniformLocation ( shaderProgram [shaderIndex] , "colorTexture") ;
        glUniform1i ( locColorTexture , personagem) ;

        glBindTexture ( GL_TEXTURE_2D, texturas[personagem]) ;

        locModel = glGetUniformLocation ( shaderProgram [ shaderIndex ] , "model");
        locNormalMatrix = glGetUniformLocation ( shaderProgram [shaderIndex ] , "normalMatrix") ;
        locShininess = glGetUniformLocation ( shaderProgram [ shaderIndex] , "shininess") ;

        glBindVertexArray ( vao ) ;
        glUseProgram ( shaderProgram [ shaderIndex ]) ;
        glUniformMatrix4fv ( locModel , 1 , GL_FALSE , modelMatrix.data () );

        glUniformMatrix3fv ( locNormalMatrix , 1 , GL_FALSE , modelMatrix.normalMatrix ().data () ) ;
        glUniform1f ( locShininess , static_cast < GLfloat >( material.shininess ) ) ;
        glDrawElements ( GL_TRIANGLES , numFaces * 3 , GL_UNSIGNED_INT,0) ;
    }

}

void Model::criaInimigo()
{
    //escolhe uma posição de nascimento aleatória
    mX = -13 - ((double)rand() / RAND_MAX) * 30;
    mY = ((double)rand() / RAND_MAX) * 20 - 10;
    mZ = ((double)rand() / RAND_MAX) * 20 - 10;

    //indica que o personagem é um inimigo
    personagem = 1;

    //um inimigo que acabou de nascer ainda não colidiu
    colidiu = false;

    material.diffuse = QVector4D(5, 5, 5, 1);
    material.specular = QVector4D(0.1, 0.1, 0.1, 1);
}

void Model::criaPersonagem()
{
    //indica que o personagem é um personagem
    personagem = 0;

    material.ambient = QVector4D(0.4, 0.5, 0.5, 1);
    material.shininess= 40;
}

void Model::createShaders()
{
    QString vertexShaderFile (":/shaders/vshader1.glsl") ;
    QString fragmentShaderFile (":/shaders/fshader1.glsl") ;

    destroyShaders () ;
    shaderProgram . clear () ;

    QFile vs ( vertexShaderFile ) ;
    QFile fs ( fragmentShaderFile ) ;

    vs.open ( QFile::ReadOnly | QFile::Text ) ;
    fs.open ( QFile::ReadOnly | QFile::Text ) ;

    QTextStream streamVs (&vs ) , streamFs (&fs ) ;

    QString qtStringVs = streamVs.readAll() ;
    QString qtStringFs = streamFs.readAll() ;

    std::string stdStringVs = qtStringVs.toStdString () ;
    std::string stdStringFs = qtStringFs.toStdString () ;

    // Create an empty vertex shader handle
    GLuint vertexShader = 0;

    vertexShader = glCreateShader( GL_VERTEX_SHADER ) ;

    // Send the vertex shader source code to GL
    const GLchar *source = stdStringVs.c_str();
    glShaderSource ( vertexShader , 1 , &source , 0) ;

    // Compile the vertex shader
    glCompileShader ( vertexShader ) ;
    GLint isCompiled = 0;

    glGetShaderiv ( vertexShader , GL_COMPILE_STATUS , &isCompiled ) ;
    if ( isCompiled == GL_FALSE )
    {
        GLint maxLength = 0;
        glGetShaderiv ( vertexShader , GL_INFO_LOG_LENGTH , &maxLength) ;

        // The maxLength includes the NULL character
        std::vector <GLchar> infoLog (maxLength) ;

        glGetShaderInfoLog ( vertexShader , maxLength , &maxLength , &infoLog[0]);

        qDebug ("Vertex Shader: %s", &infoLog [0]) ;
        glDeleteShader ( vertexShader ) ;

        return ;
    }

    // Create an empty fragment shader handle
    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader ( GL_FRAGMENT_SHADER ) ;

    // Send the fragment shader source code to GL
    // Note that std :: string ’s . c_str is NULL character terminated.
    source = stdStringFs.c_str();
    glShaderSource ( fragmentShader , 1 , &source , 0) ;

    // Compile the fragment shader
    glCompileShader ( fragmentShader ) ;
    glGetShaderiv ( fragmentShader , GL_COMPILE_STATUS , &isCompiled );

    if ( isCompiled == GL_FALSE )
    {
        GLint maxLength = 0;
        glGetShaderiv ( fragmentShader , GL_INFO_LOG_LENGTH , &maxLength ) ;

        std::vector <GLchar> infoLog ( maxLength ) ;

        glGetShaderInfoLog ( fragmentShader , maxLength , &maxLength ,&infoLog[0]) ;

        qDebug ("Fragment Shader: %s", &infoLog [0]) ;

        glDeleteShader ( fragmentShader ) ;
        glDeleteShader ( vertexShader ) ;

        return ;
    }

    GLuint shaderProgramID = 0;
    shaderProgramID = glCreateProgram () ;
    shaderProgram.push_back ( shaderProgramID ) ;

    // Attach our shaders to our program
    glAttachShader ( shaderProgramID , vertexShader ) ;
    glAttachShader ( shaderProgramID , fragmentShader ) ;

    // Link our program
    glLinkProgram ( shaderProgramID ) ;

    // Note the different functions here : glGetProgram * instead of glGetShader *.
    GLint isLinked = 0;
    glGetProgramiv ( shaderProgramID , GL_LINK_STATUS , ( int *) &isLinked );

    if ( isLinked == GL_FALSE )
    {
        GLint maxLength = 0;
        glGetProgramiv ( shaderProgramID , GL_INFO_LOG_LENGTH , &maxLength ) ;

        // The maxLength includes the NULL character
        std::vector < GLchar > infoLog ( maxLength ) ;

        glGetProgramInfoLog ( shaderProgramID , maxLength , &maxLength , &infoLog [0]) ;

        qDebug ("%s", &infoLog [0]) ;

        glDeleteProgram ( shaderProgramID ) ;
        glDeleteShader ( vertexShader ) ;
        glDeleteShader ( fragmentShader ) ;
        return ;
    }

    glDetachShader ( shaderProgramID , vertexShader ) ;
    glDetachShader ( shaderProgramID , fragmentShader ) ;
    glDeleteShader ( vertexShader ) ;
    glDeleteShader ( fragmentShader );

    vs.close();
    fs.close();
}

//formato mais simples de todos
void Model::readOFFFile ( QString const &fileName )
{

    qDebug() << fileName;

    std::ifstream stream ;
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    stream.open ( fileName.toUtf8 () , std::ifstream::in) ;

    /*QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream stream(&file);*/

    if (!stream.is_open())
    {
        qWarning ("Cannot open file.") ;
        return ;
    }

    std::string line ;
    stream>>line ;
    stream>>numVertices>>numFaces>>line ;

    qDebug() << numVertices << numFaces;

    vertices = std::make_unique <QVector4D[]>(numVertices ) ;
    indices = std::make_unique <unsigned int[]>(numFaces * 3) ;

    if ( numVertices > 0)
    {
        float minLim = std::numeric_limits <float >::lowest () ;
        float maxLim = std::numeric_limits <float >::max () ;

        QVector4D max ( minLim , minLim , minLim , 1.0) ;
        QVector4D min ( maxLim , maxLim , maxLim , 1.0) ;

        for ( unsigned int i = 0; i < numVertices ; ++i )
        {
            float x , y , z ;
            stream>>x>>y>>z;

            max.setX ( std::max ( max.x () , x ) );
            max.setY ( std::max ( max.y () , y ) );
            max.setZ ( std::max ( max.z () , z ) );
            min.setX ( std::min ( min.x () , x ) );
            min.setY ( std::min ( min.y () , y ) );
            min.setZ ( std::min ( min.z () , z ) );

            vertices[i] = QVector4D (x , y , z , 1.0);
        }

        //invdiag é a escala, que é calculada com os extremos do objeto
        //ele fica um pouco menor que a tela
        this->midPoint = QVector3D (( min + max ) * 0.5) ;
        this->invDiag = 0.3 / ( max - min ).length () ;

        //lê os índices que compoem a malha
        for ( unsigned int i = 0; i < numFaces ; ++i )
        {
            unsigned int a , b , c ;

            //ignora o primeiro valor e lê os 3 seguintes
            stream>>line>>a>>b>>c ;

            indices [ i * 3 + 0] = a ;
            indices [ i * 3 + 1] = b ;
            indices [ i * 3 + 2] = c ;
        }

        stream.close () ;

        createNormals();
        createTexCoords () ;
        createShaders () ;
        createVBOs () ;
    }
}

void Model :: createTexCoords ()
{
    texCoords = std :: make_unique < QVector2D [] >( numVertices ) ;

    // Compute minimum and maximum values
    auto minz = std :: numeric_limits <float >:: max () ;
    auto maxz = std :: numeric_limits <float >:: lowest () ;

    for ( unsigned int i = 0; i < numVertices ; ++ i )
    {
        minz = std :: min ( vertices [ i ]. z () , minz ) ;
        maxz = std :: max ( vertices [ i ]. z () , maxz ) ;
    }

    for ( unsigned int i = 0; i < numVertices ; ++ i )
    {
        auto s = ( std :: atan2 ( vertices [ i ]. y () , vertices [ i ]. x () ) + M_PI ) / (2 * M_PI ) ;
        auto t = 1.0f - ( vertices [ i ].z () - minz ) / ( maxz - minz ) ;
        texCoords [ i ] = QVector2D (s , t ) ;
    }
}

void Model :: loadTextures ()
{
    //dicas em https://open.gl/drawing

    if (texturas[personagem])
    {
        glDeleteTextures (2 , texturas) ;
    }

    //carrega as texturas (avisa que vão ser 2)
    glGenTextures (2 , texturas);

    //carrega imagens
    QImage image0 ;
    //personagem
    image0.load(QString(QCoreApplication::applicationDirPath()+"/texturas/metal.jpg")) ;

    image0 = image0.convertToFormat(QImage::Format_RGBA8888);

    QImage image1 ;
    //inimigo
    image1.load(QString(QCoreApplication::applicationDirPath()+"/texturas/lava.jpg")) ;

    image1 = image1.convertToFormat(QImage::Format_RGBA8888);

    //seta como ativa textura 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture ( GL_TEXTURE_2D , texturas[0]) ;
    glTexImage2D ( GL_TEXTURE_2D , 0 , GL_RGBA , image0 . width () , image0 .height () , 0 , GL_RGBA , GL_UNSIGNED_BYTE , image0 . bits () ) ;

    //seta a repetição da imagem como espelhada
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_MIRRORED_REPEAT ) ;
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_MIRRORED_REPEAT ) ;
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR ) ;
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER ,  GL_LINEAR_MIPMAP_LINEAR ) ;
    glGenerateMipmap ( GL_TEXTURE_2D ) ;

    //seta como ativa textura 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture ( GL_TEXTURE_2D , texturas[1]) ;
    glTexImage2D ( GL_TEXTURE_2D , 0 , GL_RGBA , image1 . width () , image1 .height () , 0 , GL_RGBA , GL_UNSIGNED_BYTE , image1 . bits () ) ;

    //seta a repetição da imagem como espelhada
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_MIRRORED_REPEAT ) ;
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_MIRRORED_REPEAT ) ;
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR ) ;
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER ,  GL_LINEAR_MIPMAP_LINEAR ) ;
    glGenerateMipmap ( GL_TEXTURE_2D ) ;
}

//gira o model
void Model::giraX(int x)
{
    this->x += x;
}

//gira o model
void Model::giraY(int y)
{
    this->y += y;
}

//gira o model
void Model::giraZ(int z)
{
    this->z = z;
}

//verifica se foi inicializado
bool Model::inicializado(QOpenGLWidget *_glWidget)
{
    return _glWidget == glWidget;
}
