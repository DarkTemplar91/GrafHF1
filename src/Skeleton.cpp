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

const char *const vertexShaderSource=R"(#version 330

	precision highp float;


	layout(location = 0) in vec2 position;
    uniform mat4 MVP;

void main(){
    vec4 mvPos=MVP*vec4(position,0,1);
    vec4 trans=vec4(mvPos.x,mvPos.y,sqrt(mvPos.x*mvPos.x+mvPos.y*mvPos.y+1),1);
    gl_Position=vec4(trans.x/(trans.z+1),trans.y/(trans.z+1),0,1);

}


)";
const char *const fragmentShaderSource=R"(
#version 330
	precision highp float;

	uniform vec3 color;
	out vec4 outColor;

	void main() {
		outColor = vec4(color, 1);
	}
)";

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


class Atom{
private:
    unsigned int vao;
    unsigned int vbo;
    static const size_t resolution=20;
    vec2 vertices[resolution];

    Atom* parent;
    Molecule* molecule;

    double mass;
    double charge;
    const double r=0.05;

    vec2 pos;
    vec3 color;

    vec2 force;
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

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);


        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(vec2)*resolution,
                     vertices,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,
                              2, GL_FLOAT, GL_FALSE,
                              0, NULL);
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
        color= charge >0 ? vec3(0.01*charge,0,0) : vec3(0,0,-0.01*charge);
        pos=vec2(0,0);
        do{
            pos.x=-1.0f + (double)rand()/ RAND_MAX * (1.f - -1.f);
            pos.y=-1.0f + (double)rand()/ RAND_MAX * (1.f - -1.f);

        }while(!isInsideRadius(0.7f));

        GLfloat doublePi=2*M_PI;
        for(int i=0;i<resolution;i++){
            vertices[i]=vec2(pos.x+r* cos(i*doublePi/resolution),pos.y+r*sin(i*doublePi/resolution));
        }

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);


        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(vec2)*resolution,
                     vertices,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,
                              2, GL_FLOAT, GL_FALSE,
                              0, NULL);


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
        glBindVertexArray(vao);
        int location = glGetUniformLocation(gpuProgram.getId(), "color");
        glUniform3f(location, color.x,color.y,color.z);



        glDrawArrays(GL_TRIANGLE_FAN, 0,resolution);



    }

    void setForce(vec2 f){force=f;}
    vec2 getForce(){return force;}

};
class Edge{
    static const size_t resolution=20;
    vec2 vertices[resolution];
    Atom* a1;
    Atom* a2;

    vec2 fromCenterOfMass=vec2(0,0);

    unsigned int vao;
    unsigned int vbo;
public:
    Edge(Atom* a1=nullptr, Atom* a2=nullptr):a1(a1),a2(a2){
    }
    void setFromCenterOfMass(vec2 d){
        fromCenterOfMass=d;
    }
    void loadVertexData(){
        if(a1!= nullptr && a2!= nullptr) {
            double diffX = (double)a1->getPos().x+ - a2->getPos().x;
            double diffY = (double)a1->getPos().y+ - a2->getPos().y;

            double intervalX = (float) diffX / resolution;
            double intervalY = (float) diffY / resolution;
            for (int i = 0; i < resolution; i++) {
                vertices[i] = vec2(a2->getPos().x +fromCenterOfMass.x + intervalX * i,
                                   a2->getPos().y +fromCenterOfMass.y + intervalY * i);
            }
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);


            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(vec2) * resolution,
                         vertices,
                         GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0,
                                  2, GL_FLOAT, GL_FALSE,
                                  0, NULL);
        }
    }
    void draw(){
        glBindVertexArray(vao);
        int location = glGetUniformLocation(gpuProgram.getId(), "color");
        glUniform3f(location, 1,1,1);
        glDrawArrays(GL_LINE_STRIP, 0,resolution);
    }
};
class Molecule{
private:
    unsigned int count;
    Atom* atoms;
    Edge* edges;
    vec2 pos=vec2();
    vec2 vel=vec2();
    double angularVel=0;
    double phi=0;
    double moi=0;
    double mass=0;
    vec2 deltaCoordinate=vec2(0,0);
    vec2 cameraShift=vec2(0,0);
public:
    Molecule(){
        count=rand() % 7 +2;
        atoms=new Atom[count];
        edges=new Edge[count-1];
        atoms[0]=Atom(nullptr,this);
        double charge=atoms[0].getCharge();
        for(int i=1;i<count;i++){
            int parentIndex=rand()%i;
            atoms[i] = Atom(&atoms[parentIndex],this);
            charge+=atoms[i].getCharge();
            edges[i-1]=Edge(&atoms[parentIndex],&atoms[i]);
        }
        atoms[count-1].setCharge(-charge);
        calculateCenterMass();



    }
    int getAtomNumber(){return count;}
    Atom* getAtoms(){return atoms;}
    void drawAtoms(){

        const mat4 mvp= RotationMatrix(phi, vec3(0,0,1))* TranslateMatrix(vec3(deltaCoordinate)+vec3(cameraShift));

        glUniformMatrix4fv(0,1,GL_FALSE,static_cast<float*>(mvp));
        for(int i=0;i<count;i++){
            atoms[i].draw();
        }
    }
    void drawBonds(){
        const mat4 mvp= RotationMatrix(phi, vec3(0,0,1))* TranslateMatrix(vec3(deltaCoordinate)+vec3(cameraShift));
        glUniformMatrix4fv(0,1,GL_FALSE,static_cast<float*>(mvp));
        for(int i=0;i<count-1;i++){
            edges[i].draw();
        }
    }
    void calculateCenterMass() {
        mass=0;
        for (int i = 0; i < count; i++) {
            mass += atoms[i].getMass();
            pos.x += atoms[i].getMass() * atoms[i].getPos().x;
            pos.y += atoms[i].getMass() * atoms[i].getPos().y;
        }
        pos.x /= mass;
        pos.y /= mass;
        for (int i = 0; i < count; i++) {
            atoms[i].setPos(atoms[i].getPos() - pos);
        }
        for(int i=0;i<count-1;i++){
            edges[i].setFromCenterOfMass(pos);
            edges[i].loadVertexData();
        }
    }
    vec2 getCenter(){return pos;}
    ~Molecule(){
        delete atoms;
        delete edges;
    }
    void setCameraShift(vec2 delta){cameraShift=delta;}
    vec2 getCameraShift(){return cameraShift;}

    void setVelocity(vec2 v){this->vel=v;}
    vec2 getVelocity(){return vel;}
    double getMass(){return mass;}
    void setMomentOfInertia(double i){this->moi=i;}
    double getMomentOfInertia(){return moi;}

    void setAngularVel(double v){angularVel=v;}
    double getAngularVel(){return angularVel;}

    void setRotationAngle(double a){phi=a;}
    double getRotationAngle(){return phi;}
    void setDeltaCoordinates(vec2 d){deltaCoordinate=d;}
    vec2 getDeltaCoordinates(){return deltaCoordinate;}

};
void onInitialization() {
    glViewport(0, 0, windowWidth, windowHeight);
    gpuProgram.create(vertexShaderSource, fragmentShaderSource, "outColor");
}


void onDisplay() {
    int error;
    while((error=glGetError())!=GL_NO_ERROR){
        printf("%d\n", error);
    }
    glClearColor(0.1, 0.1, 0.1, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    int location = glGetUniformLocation(gpuProgram.getId(), "color");
    glUniform3f(location, 0.0f, 1.0f, 0.0f);
    float MVPtransf[] = {       1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              0, 0, 0, 1 };

    location = glGetUniformLocation(gpuProgram.getId(), "MVP");	// Get the GPU location of uniform variable MVP
    glUniformMatrix4fv(location, 1, GL_FALSE, MVPtransf);
    if(molecules.size()>0)
    {
        for (const auto &item : molecules){
            item->drawBonds();
            item->drawAtoms();
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
        for (const auto &item : molecules) {
            {
                switch (key) {
                    case 's':
                        item->setCameraShift(item->getCameraShift() + vec2(-0.1, 0));
                        break;
                    case 'd':
                        item->setCameraShift(item->getCameraShift() + vec2(+0.1, 0));
                        break;
                    case 'x':
                        item->setCameraShift(item->getCameraShift() + vec2(0, -0.1));
                        break;
                    case 'e':
                        item->setCameraShift(item->getCameraShift() + vec2(0, 0.1));
                        break;


                }

            }
        }
    }
    glutPostRedisplay();
}
void onKeyboardUp(unsigned char key, int pX, int pY) {
}
void onMouseMotion(int pX, int pY) {
}
void onMouse(int button, int state, int pX, int pY) {
}
long lastUpdate=0;
void onIdle() {

    long time = glutGet(GLUT_ELAPSED_TIME);
    if(time-lastUpdate>10 && molecules.size()>0){
        double T=time-lastUpdate;
        lastUpdate=time;
        double deltaT=0.0001;
        for(double t=0;t<T;t+=deltaT*1000) {
            for (int i = 0; i < molecules.size(); i++) {
                vec2 netForce=vec2(0,0);
                vec3 torque=vec2(0,0);
                double phi=0;
                for (int j = 0; j < molecules[i]->getAtomNumber(); j++) {
                    vec2 forces[molecules[i]->getAtomNumber()];
                    for (int m = 0; m < molecules.size(); m++) {
                        if (m == i)
                            continue;
                        for (int a = 0; a < molecules[m]->getAtomNumber(); a++) {
                            //Calculate coulomb

                            vec2 vector = molecules[i]->getAtoms()[j].getPos() - molecules[m]->getAtoms()[a].getPos();
                            vec2 force =
                                    (molecules[i]->getAtoms()[j].getCharge() * molecules[m]->getAtoms()[a].getCharge() *
                                     1.60217663e-26 * normalize(vector) /
                                     (2 * M_PI * 8.85418782 * length(vector)));

                            //add force vector to net force acting on the atom
                            forces[i] = forces[i] + force;

                        }
                    }
                    double mag=10000;
                    forces[i]=forces[i]-(mag*0.47*molecules[i]->getVelocity()/molecules[i]->getMass()*1.6735575e-27*10);
                    molecules[i]->getAtoms()[j].setForce(forces[i]);
                    netForce=netForce+forces[i];
                    vec2 lever=molecules[i]->getCenter()-molecules[i]->getAtoms()[j].getPos();

                    torque=torque+cross(lever, forces[i]);
                    phi+=molecules[i]->getAtoms()[j].getMass()*1.6735575e-27*length(lever)*length(lever);
                }
                //Set velocity of molecule
                molecules[i]->setVelocity(molecules[i]->getVelocity()+(netForce/(molecules[i]->getMass()* 1.6735575e-27)*deltaT));

                //Set angular velocity
                double angularAcc = torque.z /phi;

                //assign angular velocity
                molecules[i]->setAngularVel(molecules[i]->getAngularVel() + angularAcc * deltaT);

                //new Positions
                molecules[i]->setDeltaCoordinates(molecules[i]->getDeltaCoordinates()+molecules[i]->getVelocity()*deltaT);
                molecules[i]->setRotationAngle(molecules[i]->getRotationAngle()+molecules[i]->getAngularVel()*deltaT);

                for(int j=0;j<molecules[i]->getAtomNumber();j++){
                    vec4 newPos=vec4(molecules[i]->getAtoms()[j].getPos().x,molecules[i]->getAtoms()[j].getPos().y,0,1)*RotationMatrix(molecules[i]->getAngularVel()*deltaT,vec3(0,0,1));
                    molecules[i]->getAtoms()[j].setPos(vec2(newPos.x,newPos.y));
                }

            }
        }
        glutPostRedisplay();

    }

}


