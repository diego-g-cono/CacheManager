#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

template <class T>
class CacheManager 
{
    private:
        int capacity;
        map <string, pair <T, int>> cache_data; // <Clave ,<Obj , Indice de Uso >>
        bool write_file (string, T);

    public :
        static const string class_name;
        CacheManager (int); // recibe la capacidad en el int
        CacheManager ();
        void insert (string key , T obj);
        T get(string key);
        void show_cache();
        void actualizar_archivo(string key);
        string get_archivo(string key);
};

template <class T>
const string CacheManager<T> :: class_name = "CacheManagerClass";

template <class T>
CacheManager <T> :: CacheManager(int cap) 
{
    capacity = cap;
};

template <class T>
CacheManager <T> :: CacheManager() {};

template <class T>
bool CacheManager <T> :: write_file(string key, T obj)
{
    ofstream archivo;
    
    archivo.open("RAM.txt", ios::app);

    if (!archivo) 
    {
        cerr << "ERROR! No se pudo abrir el archivo."<<endl;
        return false;
    }
    
    archivo<< "Clave: " << key<<" ";
    archivo<<obj.get_key()<<" ";
    archivo<<obj.get_value()<<" ";
    archivo<<obj.get_data();
    archivo<<"\n";
    
    archivo.close();
    if (archivo.fail()) {
        cerr << "Error al cerrar el archivo." <<endl;
        return false;
    }

    return true;
};

// INSERT
template <class T>
void CacheManager <T> :: insert(string key , T obj)
{
    int mru = 0; 
    int lru = 0;
    string archivar;
    T archivar2;
    for (auto &i : cache_data)
    {
        if(lru == 0)
        {
            lru = i.second.second;
            archivar = i.first;
            archivar2 = i.second.first;
        };
        if(mru<i.second.second)
        {
            mru = i.second.second;
        };
        if(lru>i.second.second)
        {
            lru = i.second.second;
            archivar = i.first;
            archivar2 = i.second.first;
        };
    };

    //Actualizacion
    for (auto &i : cache_data)
    {
        if(i.first == key)
        {
            actualizar_archivo(key);
            write_file(key, obj);
            cache_data[key] = make_pair(obj, mru+1);
            return;
        };
    };
    
    //Dato nuevo
    if(cache_data.size() == 0)//cache no llena
    {
        cache_data[key] = make_pair(obj, mru+1);
        write_file(key, obj);
    }
    else if((int)cache_data.size() < capacity)//cache no llena
    {
        cache_data[key] = make_pair(obj, mru+1);
        actualizar_archivo(key);
        write_file(key, obj);
    }
    else//cache llena
    {
        cache_data[key] = make_pair(obj, mru+1);
        actualizar_archivo(archivar);
        write_file(archivar, archivar2);
        actualizar_archivo(key);
        write_file(key, obj);
        cache_data.erase(archivar);
    };
};

//Metodo tipo Print de Cache
template <class T>
void CacheManager <T> :: show_cache()
{
    for (auto &i : cache_data)
    {
        cout << "Clave: " << i.first;
        i.second.first.print();
        cout<< ", Uso: " << i.second.second<< endl;
        cout<<"\n";
    };
};

//Obtiene el dato, los busca en cache o en el archivo.
template <class T>
T CacheManager <T> :: get(string key)
{
    int mru = 0;
    string dato;
    size_t startPos = 0;
    size_t endPos = 0;
    vector <string> datos;

    for (auto &i : cache_data)
    {
        if(mru<i.second.second)
        {
            mru = i.second.second;
        };
    };
    for (auto &i : cache_data)
    {
        if(i.first == key)
        {
            i.second.second = mru+1;
            return i.second.first;
        };
    };
    
    //Dato no encontrado en cache, lo busca en archivo
    dato = get_archivo(key);

    //Pasa el string a dato nuevamente
    while (startPos < dato.length()) {
        endPos = dato.find_first_of(" \n", startPos);
        if (endPos != string::npos && dato[endPos] == '\n') {
            break;
        }

        if (endPos == string::npos) {
            endPos = dato.length();
        }

        string substring = dato.substr(startPos, endPos - startPos);
        datos.push_back(substring);

        startPos = endPos + 1;
    }
    
    //Se inserta el dato nuevamente en cache
    insert(datos[1], T(stoi(datos[2]), stoi(datos[3]), (" "+datos[5]+" ")));
    
    //Se devuelve el dato
    return(T(stoi(datos[2]), stoi(datos[3]), (" "+datos[5]+" ")));
    
};

//Cuando se modifica una dato en cache se actualiza el dato en el archivo
template <class T>
void CacheManager <T> :: actualizar_archivo(string key)
{
    key = "Clave: " + key + " ";
    ifstream archivo;
    string line;
    vector <string> lines;

    archivo.open("RAM.txt", ios::in);

    if (archivo.fail()) 
    {
        cerr << "ERROR! No se pudo abrir el archivo."<<endl;
        return;
    };
    
    while (!archivo.eof())
    {
        getline(archivo, line);
        if (line.find(key) == string::npos)
        {
            lines.push_back(line);
        };
    };
    
    archivo.close();

    ofstream archivow;
    archivow.open("RAM.txt", ios::out | ios::trunc);

    if (!archivow) 
    {
        cerr << "ERROR! No se pudo abrir el archivo."<<endl;
        return;
    };
    
    for (const auto& l : lines)
    {
        if (!l.empty()) {
            archivow << l << endl;
        }
            
    };

    archivow.close();
    if (archivow.fail()) {
        cerr << "Error al cerrar el archivo de escritura." <<endl;
        return;
    };
};

//Buscar dato en archivo si no lo encuentra lo anuncia
template <class T>
string CacheManager <T> :: get_archivo(string key)
{
    key = "Clave: " + key + " ";
    ifstream archivo;
    string line;

    archivo.open("RAM.txt", ios::in);

    if (archivo.fail()) 
    {
        cerr << "ERROR! No se pudo abrir el archivo."<<endl;
        throw runtime_error("\n");
    };
    
    while (!archivo.eof())
    {
        getline(archivo, line);
        if (line.find(key) != string::npos)
        {
            return line;
        };
    };
    archivo.close();

    throw runtime_error("Clave no encontrada en la memoria");
};


