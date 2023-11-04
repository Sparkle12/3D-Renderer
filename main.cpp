#include <SFML/Graphics.hpp>
#include<iostream>
#include <cmath>
#include <thread>
#include <chrono>

struct Matrix
{
    float m[4][4] = {0};
};

class vec3D
{
    float x,y,z;

    public:

    vec3D(float cx = 0,float cy = 0,float cz = 0)
    {
        x = cx; y = cy; z = cz;
    }

    vec3D(const vec3D& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
    }

    vec3D& operator=(const vec3D& vec)
    {
        x = vec.x; y = vec.y; z = vec.z;
        return *this; 
    }

    vec3D operator*(const Matrix mat)
    {
        vec3D result;
        result.x = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + mat.m[3][0];
        result.y = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + mat.m[3][1];
        result.z = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + mat.m[3][2];
        float w = x * mat.m[0][3] + y * mat.m[1][3] + z * mat.m[2][3] + mat.m[3][3];

        if(w != 0)
        {
            result.x /= w; result.y /= w; result.z /= w;
        }

        return result;
    }

    float getX(){return x;}

    float getY(){return y;}

    float getZ(){return z;}

    void setX(float cx){x = cx;}

    void setY(float cy){y = cy;}

    void setZ(float cz){this->z = cz;}
};

class Triangle
{
    std::vector<vec3D> points;

    public:

    Triangle()
    {
        points.emplace_back(vec3D());
        points.emplace_back(vec3D());
        points.emplace_back(vec3D());
    }

    Triangle(const vec3D& A,const vec3D& B,const vec3D& C)
    {
        points.emplace_back(A);
        points.emplace_back(B);
        points.emplace_back(C);
    }

    Triangle(const vec3D cpoints[3])
    {
        points.emplace_back(cpoints[0]);
        points.emplace_back(cpoints[1]);
        points.emplace_back(cpoints[2]);
    }

    std::vector<vec3D> getPoints(){return points;}

    vec3D& operator[](int i){return points[i];}
};

class Mesh
{
    std::vector<Triangle> triangles;

    public:

    Mesh(std::vector<Triangle> ctriangles = {})
    {
        for(Triangle tri : ctriangles)
        {
            triangles.emplace_back(tri);
        }
    }

    Mesh operator*(const Matrix &rotationMatrix)
    {
        std::vector<Triangle> temp;

        for(Triangle &tri : triangles)
        {
            Triangle aux;

            aux[0] = tri[0] * rotationMatrix;
            aux[1] = tri[1] * rotationMatrix;
            aux[2] = tri[2] * rotationMatrix;

            temp.emplace_back(aux);
        }
        return Mesh(temp);
    }

    Mesh offset(vec3D offset)
    {

        std::vector<Triangle> temp;
        for(Triangle &tri : triangles)
        {
            Triangle aux;

            aux[0].setX(tri[0].getX() + offset.getX());
            aux[0].setY(tri[0].getY() + offset.getY());
            aux[0].setZ(tri[0].getZ() + offset.getZ());

            aux[1].setX(tri[1].getX() + offset.getX());
            aux[1].setY(tri[1].getY() + offset.getY());
            aux[1].setZ(tri[1].getZ() + offset.getZ());

            aux[2].setX(tri[2].getX() + offset.getX());
            aux[2].setY(tri[2].getY() + offset.getY());
            aux[2].setZ(tri[2].getZ() + offset.getZ());

            temp.emplace_back(aux);
        }
        return Mesh(temp);
    }

    Mesh scale(vec3D scale)
    {
        std::vector<Triangle> temp;
        for(Triangle &tri : triangles)
        {
            Triangle aux;

            aux[0].setX(tri[0].getX() * scale.getX());
            aux[0].setY(tri[0].getY() * scale.getY());
            aux[0].setZ(tri[0].getZ() * scale.getZ());

            aux[1].setX(tri[1].getX() * scale.getX());
            aux[1].setY(tri[1].getY() * scale.getY());
            aux[1].setZ(tri[1].getZ() * scale.getZ());

            aux[2].setX(tri[2].getX() * scale.getX());
            aux[2].setY(tri[2].getY() * scale.getY());
            aux[2].setZ(tri[2].getZ() * scale.getZ());

            temp.emplace_back(aux);
        }
        return Mesh(temp);
    }

    void setTris(std::vector<Triangle> ctriangles){triangles = ctriangles;}
    
    std::vector<Triangle> getTris(){return triangles;}

    void Draw(sf::RenderWindow &window)
    {
        for(Triangle tri: triangles)
        {
            sf::ConvexShape temp;
            temp.setPointCount(3);
            temp.setPoint(0,sf::Vector2f(tri[0].getX(),tri[0].getY()));
            temp.setPoint(1,sf::Vector2f(tri[1].getX(),tri[1].getY()));
            temp.setPoint(2,sf::Vector2f(tri[2].getX(),tri[2].getY()));



          /*  int wtf;
            std::cin>>wtf;*/

            temp.setFillColor(sf::Color::White);
            window.draw(temp);

        }
    }

    void ToViewport(sf::RenderWindow &window)
    {
        *this = this->offset(vec3D(1.0f,1.0f,0.0f));
       *this =  this->scale(vec3D(0.5f* (float)window.getSize().x,0.5f * (float)window.getSize().y,1.0f));
    }


    void print()
    {
        for(Triangle tri :triangles)
        {
            std::cout<<std::endl;
            std::cout<<"("<<tri[0].getX()<<","<<tri[0].getY()<<")"<<std::endl;
            std::cout<<"("<<tri[1].getX()<<","<<tri[1].getY()<<")"<<std::endl;
            std::cout<<"("<<tri[2].getX()<<","<<tri[2].getY()<<")"<<std::endl;
            std::cout<<std::endl;
        }

    }

};

class my3DEngine {
    Mesh cube, pyramid;
    Matrix projectionMatrix;
    float fTheta;
public:

    my3DEngine() {};


    bool OnUserCreate(sf::RenderWindow &window) {


        std::vector<Triangle> cubeTris = {

                Triangle(vec3D(0.0f, 0.0f, 0.0f), vec3D(0.0f, 1.0f, 0.0f), vec3D(0.0f, 1.0f, 0.0f)),
                Triangle(vec3D(0.0f, 0.0f, 0.0f), vec3D(0.0f, 1.0f, 0.0f), vec3D(1.0f, 0.0f, 0.0f)),

                Triangle(vec3D(1.0f, 0.0f, 0.0f), vec3D(1.0f, 1.0f, 0.0f), vec3D(1.0f, 1.0f, 1.0f)),
                Triangle(vec3D(1.0f, 0.0f, 0.0f), vec3D(1.0f, 1.0f, 1.0f), vec3D(1.0f, 0.0f, 1.0f)),

                Triangle(vec3D(1.0f, 0.0f, 1.0f), vec3D(1.0f, 1.0f, 1.0f), vec3D(0.0f, 1.0f, 1.0f)),
                Triangle(vec3D(1.0f, 0.0f, 1.0f), vec3D(0.0f, 1.0f, 1.0f), vec3D(0.0f, 0.0f, 1.0f)),

                Triangle(vec3D(0.0f, 0.0f, 1.0f), vec3D(0.0f, 1.0f, 1.0f), vec3D(0.0f, 1.0f, 0.0f)),
                Triangle(vec3D(0.0f, 0.0f, 1.0f), vec3D(0.0f, 1.0f, 0.0f), vec3D(0.0f, 0.0f, 0.0f)),

                Triangle(vec3D(0.0f, 1.0f, 0.0f), vec3D(0.0f, 1.0f, 1.0f), vec3D(1.0f, 1.0f, 1.0f)),
                Triangle(vec3D(0.0f, 1.0f, 0.0f), vec3D(1.0f, 1.0f, 1.0f), vec3D(1.0f, 1.0f, 0.0f)),

                Triangle(vec3D(1.0f, 0.0f, 1.0f), vec3D(0.0f, 0.0f, 1.0f), vec3D(0.0f, 0.0f, 0.0f)),
                Triangle(vec3D(1.0f, 0.0f, 1.0f), vec3D(0.0f, 0.0f, 0.0f), vec3D(1.0f, 0.0f, 0.0f))
        };

        std::vector<Triangle> pyramidTris =
                {
                        Triangle(vec3D(0.0f, 0.0f, 0.0f), vec3D(0.0f, 0.0f, 1.0f), vec3D(1.0f, 0.0f, 0.0f)),
                        Triangle(vec3D(1.0f, 0.0f, 0.0f), vec3D(0.0f, 0.0f, 1.0f), vec3D(1.0f, 0.0f, 1.0f)),

                        Triangle(vec3D(0.0f, 0.0f, 0.0f), vec3D(0.5f, 1.0f, 0.5f), vec3D(1.0f, 0.0f, 0.0f)),

                        Triangle(vec3D(1.0f, 0.0f, 0.0f), vec3D(0.5f, 1.0f, 0.5f), vec3D(1.0f, 0.0f, 1.0f)),

                        Triangle(vec3D(1.0f, 0.0f, 1.0f), vec3D(0.5f, 1.0f, 0.5f), vec3D(0.0f, 0.0f, 1.0f)),

                        Triangle(vec3D(0.0f, 0.0f, 1.0f), vec3D(0.5f, 1.0f, 0.5f), vec3D(0.0f, 0.0f, 0.0f))
                };

        cube.setTris(cubeTris);
        pyramid.setTris(pyramidTris);

        float Near = 0.1f;
        float Far = 1000.0f;
        float fov = 90.0f;
        float aspectRatio = (float) window.getSize().y / (float) window.getSize().x;
        float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);


        projectionMatrix.m[0][0] = aspectRatio * fovRad;
        projectionMatrix.m[1][1] = fovRad;
        projectionMatrix.m[2][2] = Far / (Far - Near);
        projectionMatrix.m[3][2] = (-Far * Near) / (Far - Near);
        projectionMatrix.m[2][3] = 1.0f;
        projectionMatrix.m[3][3] = 0.0f;


        return true;
    }

    bool OnUserUpdate(float fElaspedTime, sf::RenderWindow &window) {
        Mesh copy_cube = cube, cube2 = cube, copy_py = pyramid;

        window.clear(sf::Color::Black);
        Matrix rotationMatrix_X, rotatinMatrix_Y, rotationMatrix_Z;

        fTheta += 1.0f * fElaspedTime;

        std::this_thread::sleep_for(std::chrono::milliseconds (2));

        //std::cout<<fElaspedTime<<std::endl;
        

        rotationMatrix_X.m[0][0] = 1.0f;
        rotationMatrix_X.m[1][1] = cosf(fTheta);
        rotationMatrix_X.m[1][2] = sinf(fTheta);
        rotationMatrix_X.m[2][1] = -sinf(fTheta);
        rotationMatrix_X.m[2][2] = cosf(fTheta);
        rotationMatrix_X.m[3][3] = 1.0f;

        rotatinMatrix_Y.m[0][0] = cosf(fTheta);
        rotatinMatrix_Y.m[0][2] = sinf(fTheta);
        rotatinMatrix_Y.m[2][0] = -sinf(fTheta);
        rotatinMatrix_Y.m[1][1] = 1.0f;
        rotatinMatrix_Y.m[2][2] = cosf(fTheta);
        rotatinMatrix_Y.m[3][3] = 1.0f;

        rotationMatrix_Z.m[0][0] = cosf(fTheta);
        rotationMatrix_Z.m[0][1] = sinf(fTheta);
        rotationMatrix_Z.m[1][0] = -sinf(fTheta);
        rotationMatrix_Z.m[1][1] = cosf(fTheta);
        rotationMatrix_Z.m[2][2] = 1.0f;
        rotationMatrix_Z.m[3][3] = 1.0f;

                copy_cube = copy_cube.offset(vec3D(-0.5f,-0.5f,-0.5f));

                copy_cube = copy_cube * rotationMatrix_Z;

                copy_cube = copy_cube *  rotationMatrix_X;

                copy_cube = copy_cube * rotatinMatrix_Y;

                copy_cube = copy_cube.offset(vec3D(0.0f,0.0f,3.0f));

                copy_cube = copy_cube * projectionMatrix;

                copy_cube = copy_cube.offset(vec3D(1.0f,1.0f,0.0f));

                copy_cube = copy_cube.scale(vec3D(0.5f* (float)window.getSize().x,0.5f * (float)window.getSize().y,1.0f));

                cube2 = cube2.offset(vec3D(-0.5f,-0.5f,-0.5f));

                cube2 = cube2 * rotationMatrix_X;

                cube2 = cube2.offset(vec3D(0.0f,0.0f,5.0f));

                cube2 = cube2 * projectionMatrix;

                cube2 = cube2.offset(vec3D(1.0f,1.0f,0.0f));

                cube2 = cube2.scale(vec3D(0.5f* (float)window.getSize().x,0.5f * (float)window.getSize().y,1.0f));

                cube2 = cube2.offset(vec3D(500.0f,250.0f,0.0f));

                cube2.Draw(window);

                copy_cube.Draw(window);

        copy_py = copy_py.offset(vec3D(-0.5f, -0.5f, -0.5f));

        copy_py = copy_py * rotationMatrix_Z;
        copy_py = copy_py * rotationMatrix_X;

        copy_py = copy_py.offset(vec3D(0.0f, 0.0f, 3.0f));

        copy_py = copy_py * projectionMatrix;

        copy_py.ToViewport(window);

        copy_py = copy_py.offset(vec3D(-250.0f,-100.0f,-1.0f));

        copy_py.Draw(window);

        window.display();

        return true;
    }

    bool Start(sf::RenderWindow &window) {
        OnUserCreate(window);
        sf::Clock clock;
        sf::Event event{};
        while (window.isOpen()) {
            if (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            float fElapsedTime = clock.restart().asSeconds();
            OnUserUpdate(fElapsedTime, window);
        }
        return true;
    }

};

int main() 
{
    my3DEngine engine;
    sf::ContextSettings sett;
    sett.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1712,800),"3D Engine",sf::Style::Default,sett);
    engine.Start(window);

        return 0;
}