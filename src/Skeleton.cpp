//=============================================================================================
// Mintaprogram: Zöld háromszög. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!!
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Szász Erik
// Neptun : B7RBBU
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"
#include <iostream>

class Molecule;
class Atom;
std::vector<Molecule*> molecules=std::vector<Molecule*>();

unsigned int buffer;
unsigned int vertexArray;
unsigned int program;

GPUProgram gpuProgram;

struct Vertex{
    vec2 pos;
    vec3 col;
    Vertex(){}
    Vertex(vec2 p, vec3 c):pos(p),col(c){}

};
Vertex* allAtomVertices;
class Atom{
private:
    Atom* parent;
    double mass;
    double charge;
    unsigned int cX;
    unsigned int cY;


public:
    Atom(){
        mass=0.1;
        charge=0.1;
        cX=0;
        cY=0;
        parent= nullptr;
    }
    Atom(Atom* parent){
        mass=(rand()%100+1)*(pow(1.6735575,-24) );
        charge=(rand()%200-100)*(pow(1.60217662, -19));
        this->parent=parent;
        do{
            cX=((rand() %600 +1));
            cY=(rand()%600+1);

        }while(!isInsideRadius(cX,cY,100));

    }
    bool isInsideRadius(unsigned int x, unsigned int y, unsigned int radius){
        if(parent==nullptr)
            return true;
        if((x-parent->cX)*(x-parent->cX)+(y-parent->cY)*(y-parent->cY)<=radius*radius)
            return true;
        return false;

    }
    unsigned int getX() {return cX;}
    unsigned int getY() {return cY;}
    double getCharge(){return charge;}
    double getMass(){return mass;}
    void setCharge(double value){charge=value;}

    void draw(unsigned int r, unsigned int res){
        GLfloat doublePi=2*M_PI;

        GLfloat circleVerticesX[res];
        GLfloat circleVerticesY[res];

        for(int i=0;i<res;i++){
            circleVerticesX[i]=cX+r* cos(i*doublePi/res);
            circleVerticesY[i]=cY+r*sin(i*doublePi/res);
        }
        Vertex* vertices=new Vertex[res];
        for(int i=0;i<res;i++){
            vertices[i]=Vertex(vec2(2*circleVerticesX[i]/600-1,2*circleVerticesY[i]/600-1),(charge<0 ? vec3(0,0,pow(1.60217662, -19)*100/(-1*charge)): vec3(pow(1.60217662, -19)*100/charge,0,0)));
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * res, vertices, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0,res);
        delete[] vertices;


    }
    void drawLineToParent(unsigned int res){
        if(parent== nullptr)
            return;
        double diffX=(double)parent->cX-cX;
        double diffY=(double)parent->cY-cY;

        double intervalX=(float)diffX/res;
        double intervalY=(float)diffY/res;
        Vertex vertices[res+1];
        for(int i=0;i<=res;i++){
            vertices[i]=Vertex(vec2(2*(cX+intervalX*i)/600-1,2*(cY+intervalY*i)/600-1),vec3(1,1,1));
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * res, vertices, GL_STATIC_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0,res);

    }

};
class Molecule{
private:
    unsigned int count;
    Atom* atoms;

public:
    Molecule(){
        count=rand() % 7 +2;
        atoms=new Atom[count];
        atoms[0]=Atom(nullptr);
        double charge=atoms[0].getCharge();
        for(int i=1;i<count;i++){
            int parentIndex=rand()%i;
            atoms[i] = Atom(&atoms[parentIndex]);
            charge+=atoms[i].getCharge();
        }
        atoms[count-1].setCharge(-charge);


    }
    int getAtomNumber(){return count;}
    Atom* getAtoms(){return atoms;}
    void drawAtoms(unsigned int r, unsigned int res){
        for(int i=0;i<count;i++){
            atoms[i].draw(r,res);
        }
    }
    void drawBonds(unsigned int res){
        for(int i=0;i<count;i++){
            atoms[i].drawLineToParent(res);
        }
    }

    ~Molecule(){
        delete atoms;
    }

};


const char *const vertexShaderSource=R"(#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec3 color;

uniform mat4 MVP;
out vec3 outColor;

void main(){
    vec4 mvPos=MVP*vec4(position,0,1);
    vec4 trans=vec4(position,sqrt(mvPos.x*mvPos.x+mvPos.y*mvPos.y+1),1);
    gl_Position=vec4(trans.x/(trans.z+1),trans.y/(trans.z+1),0,1);
    outColor=color;

}


)";

const char *const fragmentShaderSource=R"(
#version 330
in vec3 outColor;
out vec4 fragmentColor;

void main(){
    fragmentColor=vec4(outColor,1);
}
)";

void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    int t=offsetof(Vertex,col);
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)offsetof(Vertex, col));
    glEnableVertexAttribArray(1);

    unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader,1,(const GLchar**)&vertexShaderSource, nullptr);
    glShaderSource(fragmentShader,1,(const GLchar**)&fragmentShaderSource, nullptr);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    program=glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);



    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glutPostRedisplay();

}


void onDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    int error;
    while((error=glGetError())!=GL_NO_ERROR){
        printf("%d\n", error);
    }

    glBindVertexArray(vertexArray);
    glUseProgram(program);
    int colorLocation=glGetUniformLocation(program, "color");
    glUniform3f(colorLocation,0,0,1);
    int mvpLocation=glGetUniformLocation(program, "MVP");
    float matrix[] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
    };
    glUniformMatrix4fv(mvpLocation,1,GL_FALSE,matrix);


    if(molecules.size()>0)
    {

        for (const auto &item : molecules){
            item->drawBonds(20);
            item->drawAtoms(10,20);
        }
    }
    glutSwapBuffers();
}
void onKeyboard(unsigned char key, int pX, int pY) {
    if(key==' '){
        molecules.push_back(new Molecule());
        molecules.push_back(new Molecule());
        glutPostRedisplay();
    }


}


void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {

}


void onMouse(int button, int state, int pX, int pY) {
    printf("\n%d, %d",pX,pY);
}


void onIdle() {
}


