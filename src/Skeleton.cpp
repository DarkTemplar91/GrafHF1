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
    unsigned int vao;
    unsigned int vbo;
    static constexpr size_t resolution=20;
    vec2 vertices[resolution];
    vec2 edge[resolution];

    Atom* parent;
    Molecule* molecule;

    double mass;
    double charge;
    const double r=0.2;

    vec2 pos;
    vec3 color;
public:

    Atom& operator=(Atom other){
        this->pos=other.pos;
        this->color=other.color;
        this->mass=other.mass;
        this->charge=other.charge;
        this->parent=other.parent;
        this->molecule=other.molecule;

        this->vbo=other.vbo;
        this->vao=other.vao;
        GLfloat doublePi=2*M_PI;
        for(int i=0;i<resolution;i++){
            vertices[i]=vec2(pos.x+r* cos(i*doublePi/resolution),pos.y+r*sin(i*doublePi/resolution));
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * resolution, vertices, GL_STATIC_DRAW);
        return *this;
    }
    Atom(){}
    Atom(Atom* parent, Molecule* mo){
        mass=rand()%300+1;
        charge=(rand()%200-100);
        if(charge==0)
            charge=0.1;
        this->parent=parent;
        this->molecule=mo;

        color= charge >0 ? vec3(1/100*charge,0,0) : vec3(0,0,1/100*charge);
        pos=vec2(0,0);
        do{
            pos.x=-1.0f + (double)rand()/ RAND_MAX * (1.f - -1.f);
            pos.y=-1.0f + (double)rand()/ RAND_MAX * (1.f - -1.f);

        }while(!isInsideRadius(0.5));

        GLfloat doublePi=2*M_PI;
        for(int i=0;i<resolution;i++){
            vertices[i]=vec2(pos.x+r* cos(i*doublePi/resolution),pos.y+r*sin(i*doublePi/resolution));
        }
        //TODO uploadVertices!!!!
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * resolution, vertices, GL_STATIC_DRAW);

    }
    bool isInsideRadius(double radius){
        if(parent==nullptr)
            return true;
        if((pos.x-parent->pos.x)*(pos.x-parent->pos.x)+(pos.y-parent->pos.y)*(pos.y-parent->pos.y)<=radius*radius)
            return true;
        return false;

    }
    //Do not forget to add molecules position as well!
    vec2 getPos(){return pos;}
    void setPos(vec2 p){pos=p;}
    double getCharge(){return charge;}
    double getMass(){return mass;}
    void setCharge(double value){charge=value;}

    void draw()const{
        int location=glGetUniformLocation(gpuProgram.getId(),"color");
        glUniform3f(location,color.x,color.y, color.z);
        glBindVertexArray(vao);

        //RotationMatrix()
        //ScaleMatrix()
        //TranslateMatrix()
        /*const mat4 mvp= RotationMatrix()* TranslateMatrix();
        TranslateMatrix(vec3(deltaC));
        mvp=mvp*RotationMatrix(phi,vec3(cX,cY,0));
        glUniformMatrix4fv(0,1,GL_FALSE,mvp);
        */


        glDrawArrays(GL_TRIANGLE_FAN, 0,resolution);



    }
    void drawLineToParent(unsigned int res){
        //TODO initialize in constructor
        if(parent== nullptr)
            return;
        double diffX=(double)parent->pos.x-pos.x;
        double diffY=(double)parent->pos.y-pos.y;

        double intervalX=(float)diffX/res;
        double intervalY=(float)diffY/res;
        Vertex vertices[res+1];
        //TODO translate
        for(int i=0;i<=res;i++){
            vertices[i]=Vertex(vec2(pos.x+intervalX*i,pos.x+intervalY*i),vec3(1,1,1));
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * res, vertices, GL_STATIC_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0,res);

    }
};
class Molecule{
private:
    unsigned int count;
    Atom* atoms;

    vec2 pos=vec2();
    vec2 vel=vec2();
    double angularVel=0;
    double phi=0;
    double sumTorque=0;
    double moi=0;

public:
    Molecule(){
        count=rand() % 7 +2;
        atoms=new Atom[count];
        atoms[0]=Atom(nullptr,this);
        double charge=atoms[0].getCharge();
        for(int i=1;i<count;i++){
            int parentIndex=rand()%i;
            atoms[i] = Atom(&atoms[parentIndex],this);
            charge+=atoms[i].getCharge();
        }
        atoms[count-1].setCharge(-charge);
        calculateCenterMass();


    }
    int getAtomNumber(){return count;}
    Atom* getAtoms(){return atoms;}
    void drawAtoms(double r, unsigned int res){
        for(int i=0;i<count;i++){
            atoms[i].draw();
        }
    }
    void drawBonds(unsigned int res){
        for(int i=0;i<count;i++){
            atoms[i].drawLineToParent(res);
        }
    }
    void calculateCenterMass() {
        double mass = 0;
        for (int i = 0; i < count; i++) {
            mass += atoms[i].getMass();
            pos.x += atoms[i].getMass() * atoms[i].getPos().x;
            pos.y += atoms[i].getMass() * atoms[i].getPos().y;
        }
        pos.x /= mass;
        pos.y /= mass;
        for (int i = 0; i < count; i++) {
            //TODO check if order of operation is correct;
            atoms[i].setPos(atoms[i].getPos()-pos);
        }
    }

    vec2 getCenter(){return pos;}
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
                    /*
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
                        */

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

void onIdle() {

//
//    if (molecules.size() > 0) {
//        std::vector<Atom*> allAtoms = std::vector<Atom*>();
//
//        for (int m = 0; m < molecules.size(); m++) {
//            for (int i = 0; i < molecules[m]->getAtomNumber(); i++) {
//                allAtoms.push_back(&molecules[m]->getAtoms()[i]);
//            }
//        }
//        vec2 forces[allAtoms.size()];
//        double angularAcc[allAtoms.size()];
//        double deltaT = 0.0001;
//        for (double t = 0; t < 0.01; t += deltaT) {
//
//            for (int i = 0; i < allAtoms.size(); i++) {
//                for (int j = 0; j < allAtoms.size(); j++) {
//                    if (j == i)
//                        continue;
//
//                    //Calculate Coulomb force
//                    vec2 vector=vec2(allAtoms[i]->getX(), allAtoms[i]->getY())
//                                - vec2(allAtoms[j]->getX(), allAtoms[j]->getY());
//                    vec2 force =
//                            (allAtoms[i]->getCharge() * allAtoms[j]->getCharge() *1.60217663e-26 * normalize(vector)/
//                             (2 * M_PI * 8.85418782*length(vector)));
//
//                    //add force vector to net force acting on the atom
//                    forces[i] = forces[i] + force;
//                }
//                //Add drag force to net force
//                double mag = 1; //magic variable
//                //forces[i] = forces[i] - 0.47 * mag * allAtoms[i]->getVelocity();
//                //change velocity by acceleration * delta t
//                vec2 preVelocity=allAtoms[i]->getVelocity();
//                allAtoms[i]->setVelocity(allAtoms[i]->getVelocity() +
//                                        (forces[i] / (allAtoms[i]->getMass() * 1.6735575e-27)) * deltaT);
//                //get the angle between the lever and force vector
//                vec2 lever = vec2(allAtoms[i]->getMolecule().getCenter().x - allAtoms[i]->getX(),
//                                  allAtoms[i]->getMolecule().getCenter().y - allAtoms[i]->getY());
//                vec2 fVector = forces[i];
//                //calculate torque
//                vec3 torque=cross(vec3(lever), vec3(fVector));
//                //calculate angular acceleration
//
//                angularAcc[i] = torque.z /
//                                ((allAtoms[i]->getMass() * 1.6735575e-27) * length(lever)*length(lever));
//
//                //assign angular velocity
//                allAtoms[i]->setAngularVel(allAtoms[i]->getAngularVel() + angularAcc[i] * deltaT);
//
//
//                //Set delta coordinates and angle
//                allAtoms[i]->setDeltaC(allAtoms[i]->getDeltaC()+(allAtoms[i]->getVelocity()*deltaT));
//                allAtoms[i]->setPhi(allAtoms[i]->getPhi()+allAtoms[i]->getAngularVel()*deltaT);
//
//                //TODO check these out
//                //RotationMatrix()
//                //ScaleMatrix()
//                //TranslateMatrix()
//
//
//
//            }
//        }
//        glutPostRedisplay();
// }
}


