#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
using namespace std;

/*------------------------------------------------
EXCEPCION PERSONALIZADA
------------------------------------------------*/
class EntidadNoEncontrada : public runtime_error {
public:
    EntidadNoEncontrada(const string& msg) : runtime_error(msg) {}
};

/*------------------------------------------------
CLASE BASE (HERENCIA)
------------------------------------------------*/
class Persona {
protected:
    string nombre;

public:
    Persona(string n) {
        if(n.empty())
            throw invalid_argument("Nombre no puede estar vacío");
        nombre = n;
    }

    virtual void mostrar() const {
        cout << "Nombre: " << nombre << endl;
    }

    string getNombre() const { return nombre; }
};

/*------------------------------------------------
COMPOSICION
EntradaHistoria pertenece a HistoriaClinica
------------------------------------------------*/
class EntradaHistoria {
public:
    string fecha;
    string nota;

    EntradaHistoria(string f, string n) {
        fecha = f;
        nota = n;
    }
};

class HistoriaClinica {
private:
    vector<EntradaHistoria> entradas;

public:
    void agregarEntrada(string fecha, string nota) {
        entradas.emplace_back(fecha, nota);
    }

    void mostrar() {
        for(const auto& e : entradas)
            cout << e.fecha << " - " << e.nota << endl;
    }
};

/*------------------------------------------------
PACIENTE (HERENCIA + COMPOSICION)
------------------------------------------------*/
class Paciente : public Persona {
private:
    string dni;
    int edad;
    string telefono;
    HistoriaClinica historia;

public:
    Paciente(string n, string d, int e, string t)
        : Persona(n) {

        if(d.size() != 8)
            throw invalid_argument("DNI debe tener 8 digitos");

        if(e < 0 || e > 120)
            throw invalid_argument("Edad invalida");

        dni = d;
        edad = e;
        telefono = t;
    }

    string getDNI() const { return dni; }

    void mostrar() const override {
        cout << "Paciente: " << nombre << " DNI: " << dni << endl;
    }
};

/*------------------------------------------------
DOCTOR (HERENCIA)
------------------------------------------------*/
class Doctor : public Persona {
private:
    string codigo;
    string especialidad;
    double tarifa;

public:
    Doctor(string n, string c, string esp, double t)
        : Persona(n) {

        if(t <= 0)
            throw invalid_argument("Tarifa invalida");

        codigo = c;
        especialidad = esp;
        tarifa = t;
    }

    string getCodigo() const { return codigo; }

    string getEspecialidad() const { return especialidad; }

    void mostrar() const override {
        cout << "Doctor: " << nombre
             << " Especialidad: " << especialidad << endl;
    }
};

/*------------------------------------------------
ASOCIACION
Cita asociada a Paciente y Doctor
------------------------------------------------*/
class Cita {
    string getFecha(){ return fecha; }

string getHora(){ return hora; }

private:
    int id;
    Paciente* paciente;
    Doctor* doctor;
    string fecha;
    string hora;
    string motivo;

    public:
    Cita(int i, Paciente* p, Doctor* d,
         string f, string h, string m) {

        id = i;
        paciente = p;
        doctor = d;
        fecha = f;
        hora = h;
        motivo = m;
    }

    int getId() { return id; }

    Doctor* getDoctor() { return doctor; }

    Paciente* getPaciente() { return paciente; }

    void mostrar() {
        cout << "Cita ID: " << id
             << " Paciente: " << paciente->getNombre()
             << " Doctor: " << doctor->getNombre()
             << " Fecha: " << fecha
             << " Hora: " << hora << endl;
    }
};
bool validarFecha(const string& f){
    if(f.size()!=10) return false;

    if(f[4]!='-' || f[7]!='-') return false;

    return true;
}
bool validarHora(const string& h){
    if(h.size()!=5) return false;

    if(h[2]!=':') return false;

    return true;
}
/*------------------------------------------------
AGREGACION
Clinica administra doctores y pacientes
------------------------------------------------*/
class Clinica {
private:
    vector<unique_ptr<Paciente>> pacientes;
    vector<unique_ptr<Doctor>> doctores;
    vector<unique_ptr<Cita>> citas;

public:

    /* Registrar paciente */
    void registrarPaciente(string n,string dni,int edad,string tel) {
        pacientes.push_back(make_unique<Paciente>(n,dni,edad,tel));
    }

    /* Registrar doctor */
    void registrarDoctor(string n,string cod,string esp,double tarifa) {
        doctores.push_back(make_unique<Doctor>(n,cod,esp,tarifa));
    }

    Paciente* buscarPaciente(string dni) {
        for(auto& p: pacientes)
            if(p->getDNI()==dni)
                return p.get();

        throw EntidadNoEncontrada("Paciente no encontrado");
    }

    Doctor* buscarDoctor(string cod) {
        for(auto& d: doctores)
            if(d->getCodigo()==cod)
                return d.get();

        throw EntidadNoEncontrada("Doctor no encontrado");
    }

    /* Crear cita */
    void crearCita(int id,string dni,string cod,
                   string fecha,string hora,string motivo) {

        Paciente* p = buscarPaciente(dni);
        Doctor* d = buscarDoctor(cod);

        for(auto& c : citas) {
            if(c->getDoctor()==d)
                throw runtime_error("Cita duplicada para ese doctor");
        }

        citas.push_back(make_unique<Cita>(id,p,d,fecha,hora,motivo));
    }

    /* Cancelar cita */
    void cancelarCita(int id) {
        for(auto it = citas.begin(); it!=citas.end(); ++it) {
            if((*it)->getId()==id) {
                citas.erase(it);
                return;
            }
        }

        throw EntidadNoEncontrada("Cita no encontrada");
    }

    /* Listar citas */
    void listarCitas() {
        for(auto& c: citas)
            c->mostrar();
    }

    vector<unique_ptr<Cita>>& getCitas() { return citas; }
};

/*------------------------------------------------
DEPENDENCIA
Clase Reporte depende de Clinica
------------------------------------------------*/
class Reporte {
public:
    static void reporteEspecialidad(Clinica& c) {

        cout << "=== REPORTE CITAS ===" << endl;

        for(auto& cita : c.getCitas()) {
            cout << cita->getDoctor()->getEspecialidad()
                 << " -> cita registrada" << endl;
        }
    }
};

/*------------------------------------------------
MENU
------------------------------------------------*/
int main() {

    Clinica clinica;
    int opcion;

    do {
        cout << "\n1 Registrar paciente\n";
        cout << "2 Registrar doctor\n";
        cout << "3 Agendar cita\n";
        cout << "4 Cancelar cita\n";
        cout << "5 Listar citas\n";
        cout << "6 Reporte\n";
        cout << "7 Salir\n";

        cin >> opcion;

        try {

            if(opcion==1) {
                string n,dni,tel;
                int edad;

                cout<<"Nombre: "; cin>>n;
                cout<<"DNI: "; cin>>dni;
                cout<<"Edad: "; cin>>edad;
                cout<<"Telefono: "; cin>>tel;

                clinica.registrarPaciente(n,dni,edad,tel);
            }

            else if(opcion==2) {
                string n,cod,esp;
                double tarifa;

                cout<<"Nombre: "; cin>>n;
                cout<<"Codigo: "; cin>>cod;
                cout<<"Especialidad: "; cin>>esp;
                cout<<"Tarifa: "; cin>>tarifa;

                clinica.registrarDoctor(n,cod,esp,tarifa);
            }

            else if(opcion==3) {
                int id;
                string dni,cod,fecha,hora,motivo;

                cout<<"ID cita: "; cin>>id;
                cout<<"DNI paciente: "; cin>>dni;
                cout<<"Codigo doctor: "; cin>>cod;
                cout<<"Fecha: "; cin>>fecha;
                cout<<"Hora: "; cin>>hora;
                cout<<"Motivo: "; cin>>motivo;

                clinica.crearCita(id,dni,cod,fecha,hora,motivo);
            }

            else if(opcion==4) {
                int id;
                cout<<"ID cita: ";
                cin>>id;
                clinica.cancelarCita(id);
            }

            else if(opcion==5) {
                clinica.listarCitas();
            }

            else if(opcion==6) {
                Reporte::reporteEspecialidad(clinica);
            }

        }
        catch(exception& e) {
            cout<<"Error: "<<e.what()<<endl;
        }

    } while(opcion!=7);

}