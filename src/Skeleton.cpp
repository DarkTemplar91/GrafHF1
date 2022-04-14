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
    double cX;
    double cY;
    double vX=0;
    double vY=0;

public:
    Atom(){
        mass=0.1;
        charge=0.1;
        cX=-1;
        cY=-1;
        parent= nullptr;
    }
    Atom(Atom* parent){
        mass=rand()%300+1;
        charge=(rand()%200-100);
        this->parent=parent;



        do{
            cX=-1.0f + (double)rand()/ RAND_MAX * (1.f - -1.f);
            cY=-1.0f + (double)rand()/ RAND_MAX * (1.f - -1.f);

        }while(!isInsideRadius(0.5));

    }
    bool isInsideRadius(double radius){
        if(parent==nullptr)
            return true;
        if((cX-parent->cX)*(cX-parent->cX)+(cY-parent->cY)*(cY-parent->cY)<=radius*radius)
            return true;
        return false;

    }
    double getX() {return cX;}
    double getY() {return cY;}
    double getCharge(){return charge;}
    double getMass(){return mass;}
    void setCharge(double value){charge=value;}
    void setX(double val){
        cX=val;
    }
    void setY(double val){
        cY=val;
    }
    void draw(double r, unsigned int res){
        GLfloat doublePi=2*M_PI;

        GLfloat circleVerticesX[res];
        GLfloat circleVerticesY[res];

        for(int i=0;i<res;i++){
            circleVerticesX[i]=cX+r* cos(i*doublePi/res);
            circleVerticesY[i]=cY+r*sin(i*doublePi/res);
        }
        Vertex vertices[res];
        for(int i=0;i<res;i++){
            vertices[i]=Vertex(vec2(circleVerticesX[i],circleVerticesY[i]),(charge<0 ? vec3(0,0,100/(-1*charge)): vec3(100/charge,0,0)));
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * res, vertices, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0,res);



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
            vertices[i]=Vertex(vec2(cX+intervalX*i,cY+intervalY*i),vec3(1,1,1));
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * res, vertices, GL_STATIC_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0,res);

    }

    void increaseVelX(double val){vX+=val;}
    double getVelX(){return vX;}
    void increaseVelY(double val){vY+=val;}
    double getVelY(){return vY;}

};
class Molecule{
private:
    unsigned int count;
    Atom* atoms;

    double cmX;
    double cmY;

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

        calculateCenterMass();


    }
    int getAtomNumber(){return count;}
    Atom* getAtoms(){return atoms;}
    void drawAtoms(double r, unsigned int res){
        for(int i=0;i<count;i++){
            atoms[i].draw(r,res);
        }
    }
    void drawBonds(unsigned int res){
        for(int i=0;i<count;i++){
            atoms[i].drawLineToParent(res);
        }
    }
    void calculateCenterMass(){
        double mass=0;
        cmX=0;
        cmY=0;
        for(int i=0;i<count;i++){
            mass+=atoms[i].getMass();
            //TODO handle small numbers, or change the Atom class;
            cmX+=atoms[i].getMass()*atoms[i].getX();
            cmY+=atoms[i].getMass()*atoms[i].getY();
        }
        cmX/=mass;
        cmY/=mass;
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
            item->drawAtoms(0.05f,20);
        }
    }
    glutSwapBuffers();
}
void onKeyboard(unsigned char key, int pX, int pY) {
    if(key==' '){
        molecules.push_back(new Molecule());
        molecules.push_back(new Molecule());

    }
    else if(key=='s' || key=='d' || key=='x' || key=='e'){
        for (const auto &item : molecules){
            for(int i=0;i<item->getAtomNumber();i++){
                switch(key){
                    case 's':
                        item->getAtoms()[i].setX(item->getAtoms()[i].getX()-0.1);
                        break;
                    case 'd':
                        item->getAtoms()[i].setX(item->getAtoms()[i].getX()+0.1);
                        break;
                    case 'x':
                        item->getAtoms()[i].setY(item->getAtoms()[i].getY()-0.1);
                        break;
                    case 'e':
                        item->getAtoms()[i].setY(item->getAtoms()[i].getY()+0.1);
                        break;
                }
            }
        }
    }
    glutPostRedisplay();



}


void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {

}


void onMouse(int button, int state, int pX, int pY) {
    printf("\n%d, %d",pX,pY);
}

double calculateDistSq(double x1, double y1,double x2, double y2){
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}
float calculateAngle(double x1, double y1, double x2, double y2){
    return atan(fabs((y1-y2)/(x1-x2)))*180/M_PI;
}


void onIdle() {


    if(molecules.size()>0) {
        std::vector<Atom> allAtoms=std::vector<Atom>();

        for(int m=0;m<molecules.size();m++){
            for(int i=0;i<molecules[m]->getAtomNumber();i++){
                allAtoms.push_back(molecules[m]->getAtoms()[i]);
            }
        }
        double forcesX[allAtoms.size()];
        double forcesY[allAtoms.size()];
        printf("");

        for (double t = 0; t < 0.01; t += 0.0001) {

            //Coulomb
            for (int i=0;i<allAtoms.size();i++){


                for(int j=0;j<allAtoms.size();j++){
                    if(j==i)
                        continue;
                    //calculate force between the two atoms
                    double force = allAtoms[i].getCharge() * allAtoms[j].getCharge()*pow(10,-10)*1.60217663 * 8.988 /
                            (calculateDistSq(allAtoms[i].getX(), allAtoms[i].getY(), allAtoms[j].getX(),
                                                 allAtoms[j].getY()));

                    //calculate the angle
                    double a= calculateAngle(allAtoms[i].getX(), allAtoms[i].getY(), allAtoms[j].getX(),
                                             allAtoms[j].getY());

                    //calculate the x and y vectors of the force
                    if(allAtoms[i].getX()<=allAtoms[j].getX())
                        forcesX[i]=forcesX[i]+ cos(a)*force*(force>0 ? 1 :-1);
                    else
                        forcesX[i]=forcesX[i]+ cos(a)*force*(force>0 ? -1 :1);

                    if(allAtoms[i].getY()<allAtoms[j].getY())
                        forcesY[i]=forcesY[i]+ sin(a)*force*(force>0 ? 1 :-1);
                    else
                        forcesY[i]=forcesY[i]+ sin(a)*force*(force>0 ? -1 :1);

                }
                //apply drag
                double mag=1; //magic variable
                forcesX[i]-=0.47*mag*allAtoms[i].getVelX()*allAtoms[i].getVelX()/2;
                forcesX[i]-=0.47*mag*allAtoms[i].getVelY()*allAtoms[i].getVelY()/2;

                //change velocity
                allAtoms[i].increaseVelX(forcesX[i]/allAtoms[i].getMass());
                allAtoms[i].increaseVelY(forcesY[i]/allAtoms[i].getMass());

            }
            //Move the atoms
            //TODO make it work... MVP?
            for(int i=0;i<allAtoms.size();i++){
                allAtoms[i].setX(allAtoms[i].getX()+allAtoms[i].getVelX());
                allAtoms[i].setY(allAtoms[i].getY()+allAtoms[i].getVelY());
            }

        }




    }

}


