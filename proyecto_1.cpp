// PROYECTO FINAL ALSE TERCER CORTE

//    INTEGRANTES
//1-) DANIEL DIAZ
//2-) ANDRES CORREA
//3-) JULIAN ROLDAN

//RESUMEN DE CODIGO
//PROGRAMA QUE TOMA DATOS DE UNA TARJETA RASPBERRY PARA COMPARARLOS CON UNOS
//PARAMETROS DE TOLERANCIA Y POSTERIORMENTE HACER USO DE LA INTERFAZ GRAFICA

//DEFINICION DE LIBRERIAS

#include "proyecto_1.h"
#include "ui_proyecto_1.h"
#include <iostream>         //LIBRERIA PARA USAR "cout" y "cin"
#include <fstream>          //LIBRERIA PARA MANEJO DE ARCHIVOS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         //LIBRERIA PARA USARR "execv"
#include <fcntl.h>
#include <stdint.h>
#include <string.h>         //LIBRERIA PARA TRABAJAR CON CADENAS DE CARACTERES
#include <QLCDNumber>       //LIBRERIA PARA MODIFICAR LA APARIENCIA DE LOS DISPLAYS QLCD
#include <QIntValidator>    //LIBRERIA PARA VALIDAR EL TAMAÑO DE CARNET INGRESADO
#include <sys/wait.h>       //LIBRERIA PARA "wait();"
#include <ctime>            //LIBRERIA PARA HALLAR EL TIEMPO EN TIMESTAMP
#include <time.h>           //LIBRERIA PARA HALLAR EL TIEMPO EN TIMESTAMP
#include <math.h>           //LIBRERIA DE LA FUNCION "modf" PARA TOMAR NUMEROS ENTEROS Y SU PARTE DECIMAL


//DEFINICION DE CONSTANTES
#define MAXPARAMETERS 4
using namespace std;

//VENTANA DE LA INTERFAZ GRAFICA
proyecto_1::proyecto_1(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::proyecto_1)
{
    ui->setupUi(this);
    setWindowTitle("Información Motor AC");

    ui -> carnetLine -> setMaxLength(7);         //CANTIDAD DE NÚMEROS DEL CARNET.
    ui -> carnetLine -> setValidator(new QIntValidator(this)); //VALIDA NO CARACTERES.

    //INICIALIZACION DE DISPLAYS
    ui -> velLCD ->setPalette(Qt:: blue);       //PONEMOS EL DISPLAY DE VELOCIDAD EN AZUL.
    ui -> frecLCD ->setPalette(Qt:: blue);      //PONEMOS EL DISPLAY DE FRECUENCIA EN AZUL.
    ui -> volLCD ->setPalette(Qt:: blue);       //PONEMOS EL DISPLAY DE VOLTAJE EN AZUL.
    ui -> tempLCD ->setPalette(Qt:: blue);      //PONEMOS EL DISPLAY DE TEMPERATURA EN AZUL.

    //INICIALIZACION DE DISPLAYS TEORICOS
    ui -> tVel ->setPalette(Qt:: blue);         //PONEMOS EL DISPLAY DE VELOCIDAD TEORICA EN AZUL.
    ui -> tfrec ->setPalette(Qt:: blue);        //PONEMOS EL DISPLAY DE FRECUENCIA TEORICA EN AZUL.
    ui -> tvol ->setPalette(Qt:: blue);         //PONEMOS EL DISPLAY DE VOLTAJE TEORICA EN AZUL.
    ui -> ttemp ->setPalette(Qt:: blue);        //PONEMOS EL DISPLAY DE TEMPERATURA TEORICA EN AZUL.
}



proyecto_1::~proyecto_1()
{
    delete ui;
}

// PROTOTIPOS DE FUNCIONES
void toma_numero(char[], char[][13], float[]);
void abrir_texto(char[]);
void comparador(char[], float[]);
void limites(float[], float[], int[]);
void imprime(float[], double[]);
void tiempo(char[][13], double [], int );

//MAIN
void proyecto_1::on_inicioBoton_clicked()
{
    QString carnet;     //VARIABLE DEL CARNET DE TIPO QString.

    int pid= 0,cont= 0, banderas[MAXPARAMETERS]={0}, actual = 0;
    /*
     * pid       ES EL PROCESS ID DEL PROCESO HIJO.
     * cont      ES EL CONTADOR QUE USAMOS PARA OBTENER EL CARNET BIT POR BIT DE LA VARIABLE cadenaStd.
     * banderas  SON LOS INDICADORES DE SI LA COMPARACION ENTRE EL VALOR TEORICO Y EL VALOR RECIBIDO CUMPLE O NO CUMPLE CON LOS PARAMETROS DE TOLERANCIA, ESTAS PUEDEN SER "1" SI LO CUMPLE(VERDE) O "0" SI NO LO CUMPLE(ROJO).
     * actual    ES LA VARIABLE ENTERA EN LA QUE SE ALMACENA EL TIEMPO EN TIMESTAMP HASTA EL MOMENTO EN QUE SE INICIA EL PROGRAMA.
     */

    char buffer[255] = {0},Ncarnet[10] = {0}, ruta[20] = {"./comunicacion.bin"},*prog[3], datos[5][13] = {{0}, {0}, {0}, {0}, {0}};
    /*
     * buffer    ES LA VARIABLE O VECTOR DE CARACTERES EN DONDE SE ALMACENARA EL CONTENIDO DEL ARCHIVO .TXT RECIBIDO POR EL PROCESO DE COMUNICACION.
     * Ncarnet   ES LA VARIABLE QUE USAMOS PARA GUARDAR EL NUMERO DE CARNET INGESADO POR EL USUARIO.
     * ruta      ES LA RUTA DEL PROCESO DE COMUNICACION "./comunicacion.bin".
     * prog
     * datos     ES LA MATRIZ EN LA QUE SE ALMACENA EL CONTENIDO DE LOS 5 PARAMETROS.
    */
    float numeros[5]={0}, porcentajes[MAXPARAMETERS]={0};
    /*
     * numeros      ES LA VARIABLE EN LA QUE SE ALMACENA EL NUMERO RECIVIDO CONVERTIDO DEL ARCHIVO DE TEXTO (VELOCIDAD, FRECUENCIA, VOLTAJE, TEMPERATURA Y TIEMPO).
     * porcentajes  ES LA VARIABLE EN LA QUE SE ALMACENA LOS PORCENTAJES GENERADOS POR CADA CARNET INGRESADO.
    */
    double unids[4] = {0};
    /*
     * unids    ES LA VARIABLE QUE GUARDA LA PARTE ENTERA DE LOS DIAS, HORAS, MINUTOS Y SEGUNDOS PARA MOSTRAR EN LOS DISPLAYS DE TIEMPO.

    */


    //TIMESTAMP AL OPRIMIR INICIO.
    actual = (int)time(NULL);


    //PROCESO EN EL QUE TOMAMOS EN NUMERO DE CARNET QUE SE ENCUENTRA EN LA VARIABLE QString Y LO ALMACENAMOS EN LA VARIABLE DE TIPO CHAR Ncarnet.
    carnet = ui -> carnetLine -> text();
    for(cont = 0; cont < 8; cont++)
    {
        std::string cadenaStd = carnet.toStdString();       //PASAMOS DE VARIABLE STD A STRING.
        Ncarnet[cont] = cadenaStd[cont];
    }


    //PORCENTAJES
    comparador(Ncarnet, porcentajes);


    //SE INICIA EL PROCESO DE COMUNICACION.
    pid = fork();
    switch (pid)
    {
        case -1: // Error
            cout<< "ERROR" <<endl;
        break;

        case 0: // Proceso hijo
            prog[0] = ruta;
            prog[1] = Ncarnet;
            prog[2] = NULL;
            execv("/home/alse/comunicacion.bin",prog);
        break;


        default: //Continua con el proceso padre
        wait(NULL);

        //FUNCION PARA ABRIR EL ARCHIVO
        abrir_texto(buffer);

        //FUNCION PARA TOMAR LOS DATOS DEL BUFFER
        toma_numero(buffer, datos, numeros);

        //FUNCION PARA HALLAR EL TIEMPO DE OPERACION
        tiempo(datos, unids, actual);
    }

    //BANDERAS DE COMPARACION
   limites(numeros, porcentajes,banderas);

    //MOSTRAR DATOS, PORCENTAJES Y COLOR

    //MOSTRAR VELOCIDAD
    if(banderas[0]==1)      //LA BANDERA DE LA VARIABLE VELOCIDAD ESTA EN UNO (VERDE).
    {
        ui->velLCD->display(datos[0]);                          //MOSTRAMOS EL VALOR DE TIPO FLOAT DE LA VARIABLE VELOCIDAD.
        ui->velLCD->setPalette(Qt::green);                      //PONEMOS EL DISPLAY VERDE INDICANDO QUE EL VALOR RECIBIDO SE ENCUENTRA DENTRO DE LOS MARGENES DE TOLERANCIA.
        ui -> tolVel -> display(porcentajes[0]*100);            //MOSTRAMOS EL PORCENTAJE QUE SE CALCULO SEGUN EL CARNET INGRESADO PARA LA VELOCIDAD.
        ui->tolVel->setPalette(Qt::black);                      //PONEMOS EL DISPLAY DE VELOCIDAD EN NEGRO. ESTETICAMENTE
    }
    else                    //LA BANDERA DE LA VARIABLE VELOCIDAD ESTA EN CERO (ROJO).
    {
        ui->velLCD->display(datos[0]);                          //MOSTRAMOS EL VALOR DE TIPO FLOAT DE LA VARIABLE VELOCIDAD.
        ui->velLCD->setPalette(Qt::red);                        //PONEMOS EL DISPLAY ROJO INDICANDO QUE EL VALOR RECIBIDO SE ENCUENTRA FUERA DE LOS MARGENES DE TOLERANCIA.
        ui -> tolVel -> display(porcentajes[0]*100);            //MOSTRAMOS EL PORCENTAJE QUE SE CALCULO SEGUN EL CARNET INGRESADO PARA LA VELOCIDAD.
        ui->tolVel->setPalette(Qt::black);                      //PONEMOS EL DISPLAY DE VELOCIDAD EN NEGRO. ESTETICAMENTE.
    }
    //REALIZAMOS EL MISMO PROCEDIMIENTO CON CADA UNA DE LAS VARIABLES EXEPTUANDO EL TIEMPO.

    //MOSTRAR FRECUENCIA
    if(banderas[1]==1)
    {
        ui->frecLCD->display(datos[1]);
        ui->frecLCD->setPalette(Qt::green);
        ui -> tolFrec -> display(porcentajes[1]*100);
        ui->tolFrec->setPalette(Qt::black);
    }
    else
    {
        ui->frecLCD->display(datos[1]);
        ui->frecLCD->setPalette(Qt::red);
        ui -> tolFrec -> display(porcentajes[1]*100);
        ui->tolFrec->setPalette(Qt::black);
    }

    //MOSTRAR VOLTAJE
    if(banderas[2]==1)
    {
        ui->volLCD->display(datos[2]);
        ui->volLCD->setPalette(Qt::green);
        ui -> tolVol -> display(porcentajes[2]*100);
        ui->tolVol->setPalette(Qt::black);
    }
    else
    {
        ui->volLCD->display(datos[2]);
        ui->volLCD->setPalette(Qt::red);
        ui -> tolVol -> display(porcentajes[2]*100);
        ui->tolVol->setPalette(Qt::black);

    }

    //MOSTRAR TEMPERATURA
    if(banderas[3]==1)
    {
        ui->tempLCD->display(datos[3]);
        ui->tempLCD->setPalette(Qt::green);
        ui -> tolTemp -> display(porcentajes[3]*100);
        ui->tolTemp->setPalette(Qt::black);
    }
    else
    {
        ui->tempLCD->display(datos[3]);
        ui->tempLCD->setPalette(Qt::red);
        ui -> tolTemp -> display(porcentajes[3]*100);
        ui->tolTemp->setPalette(Qt::black);
    }

   //MOSTRAR TIEMPO.
    if(unids[0] > 0 && unids[1] > 0 && unids[2] > 0 && unids[3] > 0)
    {
        ui -> diasLCD ->display(unids[0]);              //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LOS DIAS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> diasLCD ->setPalette(Qt::black);          //PONEMOS EL DISPLAY DE DIAS EN NEGRO. ESTETICAMENTE

        ui -> horasLCD ->display(unids[1]);             //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LAS HORAS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> horasLCD ->setPalette(Qt::black);         //PONEMOS EL DISPLAY DE HORAS EN NEGRO. ESTETICAMENTE

        ui -> minLCD ->display(unids[2]);               //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LOS MINUTOS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> minLCD ->setPalette(Qt::black);           //PONEMOS EL DISPLAY DE MINUTOS EN NEGRO. ESTETICAMENTE

        ui -> segLCD ->display(unids[3]);               //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LOS SEGUNDOS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> segLCD ->setPalette(Qt::black);           //PONEMOS EL DISPLAY DE SEGUNDOS EN NEGRO. ESTETICAMENTE
    }
    else
    {
        ui -> diasLCD ->display("E");              //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LOS DIAS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> diasLCD ->setPalette(Qt::red);          //PONEMOS EL DISPLAY DE DIAS EN NEGRO. ESTETICAMENTE

        ui -> horasLCD ->display("4");             //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LAS HORAS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> horasLCD ->setPalette(Qt::red);         //PONEMOS EL DISPLAY DE HORAS EN NEGRO. ESTETICAMENTE

        ui -> minLCD ->display("0");               //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LOS MINUTOS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> minLCD ->setPalette(Qt::red);           //PONEMOS EL DISPLAY DE MINUTOS EN NEGRO. ESTETICAMENTE

        ui -> segLCD ->display("4");               //PARA EL TIEMPO MOSTRAMOS EN EL DISPLAY EL VALOR DE LOS SEGUNDOS CALCULADOS EN LA FUNCION TIEMPO.
        ui -> segLCD ->setPalette(Qt::red);           //PONEMOS EL DISPLAY DE SEGUNDOS EN NEGRO. ESTETICAMENTE
    }
    //FUNCION PARA IMPRIMIR LOS DATOS EN EL CUADRO DE TEXTO
    imprime(numeros, unids);

}


/*
 * LA FUNCION DE TIEMPO

*/
void tiempo(char datos[][13], double unids[], int actual)
{
    int  segs = 0, recibido = 0;
    double paso, decimal , entera;

    recibido = atoi(datos[4]);
    segs = actual - recibido;       //para obtener el tiempo de operación en segundos.
    cout << "segundos en operación: " << segs << endl;

    paso = segs/86400.0;       // 86400 -> CANTIDAD DE SEGUNDOS EN UN DÍA.
    decimal = modf(paso, &entera); //Para separar parte entera de la parte decimal.
    unids[0] = entera; //días.


    paso = segs/3600.0;        // 3600 -> CANTIDAD DE SEGUNDOS EN UNA HORA.
    decimal = modf(paso, &entera);
    unids[1] = entera; //horas.
    while(unids[1] > 23)
    {
        unids[1] = unids[1] - 24;
    }


    paso = segs/60.0;           //60 -> CANTIDAD DE SEGUNDOS EN UN MINUTO.
    decimal = modf(paso, &entera);
    unids[2] = entera; // minutos
    while(unids[2] > 59)
    {
        unids[2] = unids[2] - 60;
    }

    unids[3] = decimal * 60; //segundos
}



//EN LA FUNCION IMPRIME SE MUESTRA LA INFORMACION PROCESADA EN EL CUADRO DE TEXTO

void imprime(float numeros[], double unids[])
{
    int cont=4;                                                                     //ESTECONTADOR ESTA INICIALIZADO EN 4 PARA MOSTRAR LOS 4 PARAMETROS DEL MOTOR.
    char parametros[][500]={"Velocidad","Frecuencia","Voltaje", "Temperatura"};     //VECTOR parametros INICIALIZADO CON EL NOMBRE DE LOS PARAMETROS, PARA LA IMPRESION EN EL CUADRO DE TEXTO.
    if(unids[0] > 0 && unids[1] > 0 && unids[2] > 0 && unids[3] > 0)
    {
        cout <<"Días: " << unids[0] << endl;                                            //SALIDA PARA MOSTRAR EL TIEMPO DE OPERACION DEL MOTOR.
        cout << unids[1] << ":" << unids[2] << ":" << unids[3] << endl;
    }
    else
    {
        cout << "\n\tERROR 404." << endl;
    }
    for(cont=0; cont<4; cont++)
    {
        cout<<"\t" <<cont+1<<"-)" << parametros[cont]<< "=" << numeros[cont]<< "\n"<<endl;
    }
}



//EN LA FUNCION LIMITES REALIZA LA COMPARACION ENTRE LOS VALORES RECIVIDOS EN EL ARCHIVO TXT Y LOS VALORES TEORICOS SEGUN LOS MARGENES DE TOLERANCIA
//DEL CARNET.

void limites(float numeros[], float porcentajes[], int banderas[])
{
    int cont=0;                         //CONTADOR PARA RECORRER TANTO PORCENTAJES DE LOS PARAMETROS COMO LOS VALORES DE CADA PARAMETRO.
    int pred[4]={1800,60,120,40};       // VECTOR pred INICIALIZADO CON LOS VALORES TEORICOS PARA HACER LA COMPARACION.
    for(cont=0;cont<4;cont++)
    {
        if((numeros[cont]<pred[cont]*(1+porcentajes[cont])) && (numeros[cont]>pred[cont]*(1-porcentajes[cont])))  //PROCESO DE COMPARACION PARA SABER SI ESTA EN EL MARGEN.
        {
            banderas[cont]=1;           //SI CUMPLE CON LOS PARAMETROS DESCRITOS EN EL IF, LA BANDERA SE PONE EN 1(VERDE).
        }
        else
        {
            banderas[cont]=0;           //SI NO CUMPLE CON LOS PARAMETROS DESCRITOS EN EL IF, LA BANDERA SE PONE EN 0(ROJO).
        }
    }
}

//EN LA FUNCION COMPARADOR SE HALLAN LOS PORCENTAJES DE CADA PARAMETRO DEL MOTOR SEGUN EL NUMERO DE CARNET INGRESADO.

void comparador(char Ncarnet[], float porcentajes[])
{
    float digitos[7] = {0};     //EN ESTA VARIABLE digitos SE GUARDARA EL NUMERO DE CARNET EN TIPO FLOAT.
    char carne[7]={0};          //EN ESTA PARTE LA VARIABLE carne TIENE TAMAÑO 7 PARA TOMAR LOS DIGITOS DEL Ncarnet.
    int cont = 0;               //CONTADOR PARA ASIGNAR EL Ncarnet A digitos BIR POR BIT.
    for (cont = 0; cont < 7; cont++)
    {
        carne[cont] = Ncarnet[cont];
        digitos[cont] = atof(&carne[cont]);         //HACEMOS LA CONVERSION DE TIPO CHAR (Ncarnet) A TIPO  FLOAT (digitos).
    }
    //MARGEN DE TOLERANCIA DEL PARAMETRO VELOCIDAD
    porcentajes[0]=((digitos[3] + digitos[4]*0.1))/100;

    //MARGEN DE TOLERANCIA DEL PARAMETRO FRECUENCIA
    porcentajes[1]=0.05;

    //MARGEN DE TOLERANCIA DEL PARAMETRO VOLTAJE
    porcentajes[2]=(digitos[2]/100);

    //MARGEN DE TOLERANCIA DEL PARAMETRO TEMPERATURA
    porcentajes[3]=(digitos[6]/100);
}

void abrir_texto(char buffer[])
{
    string buff;
    ifstream MyReadFile("/home/alse/Documents/datos.txt");
    while(getline(MyReadFile, buff))
        cout << "\n\n Datos motor:\n" << buff << endl;
    MyReadFile.close();
    strcpy(buffer,buff.c_str()); //para convertir de string a char[].
}

void toma_numero(char buffer[], char datos[][13], float numeros[])
{
    int tam=0, cont=0;
    int tam_pal[30]={0};
    char nuevobuff[200] = {0};
    char variables[5][17] = {{'\"', 'v', 'e', 'l', 'o', 'c', 'i', 'd', 'a', 'd', '\"', ':'},
                             {'\"', 'f', 'r', 'e', 'c', 'u', 'e', 'n', 'c', 'i', 'a', '\"', ':'},
                             {'\"', 'v', 'o', 'l', 't', 'a', 'j', 'e', '\"', ':'},
                             {'\"', 't', 'e', 'm', 'p', 'e', 'r', 'a', 't', 'u', 'r', 'a', '\"', ':'},
                             {'\"', 't', 'i', 'e', 'm', 'p', 'o', '_', 'i', 'n', 'i', 't','_','o','p', '\"', ':'}};

    for(cont = 0; cont < 5; cont++)
    {
        tam_pal[cont] = strlen(variables[cont]);    //Longitud de las palabras.
        tam = strstr(buffer, variables[cont])+ tam_pal[cont] - buffer; // tamaño sin la palabra.
        strcpy(nuevobuff, buffer + tam); //copia en un nuevo arreglo el buffer sin la palabra.
        if(cont == 4)
            tam = strchr(nuevobuff, '}') - nuevobuff -3; // busca el caracter en el nuevo arreglo.
        else
            tam = strchr(nuevobuff, ',') - nuevobuff;
        strncpy(datos[cont], nuevobuff, tam);   //copia el numero de cada variable en un vector de cadenas de caracteres.
        numeros[cont] = atof(datos[cont]);  //convierte a float para poder comparar.

    }
}
