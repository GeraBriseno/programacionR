#include <LiquidCrystal_I2C.h>


//pantalla
int pinSDA = 21;
int pinSCL = 22;
int lcdColumns = 20; 
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  //asume que la direccion I2C de la pantalla es 0x27

//sensores ultrasonicos
const int trigPin1 = 15;      //2
const int echoPin1 = 18;     //4
/*const int trigPin2 = 1;
const int echoPin2 = 3;
const int trigPin3 = 17;
const int echoPin3 = 16;
*/
long duracion;
int distancia;

int distanciaSensorAlPiso = 36;       //distancia del sensor al piso en centímetros /////////////////
int distanciaSensorANivelLLeno = 10;   //distancia del sensor al nivel del tanque lleno ///////////////////

//niveles tanques
String nivelesTanques= "Tanques ";

// botones inicio 
int boton1 = 4;
int boton2 = 5;
int boton3 = 23;
int boton4 = 0;
int estadoBoton1;
int estadoBoton2;
int estadoBoton3;
int estadoBoton4;
int ultimoEstadoBoton1 = LOW;
int ultimoEstadoBoton2 = LOW;
int ultimoEstadoBoton3 = LOW;
int ultimoEstadoBoton4 = LOW;
bool valorBoton4 = false;
int menuPrincipal = 1;
int menuRecirculacion = 1;
int menuInyeccion = 1;
int menuLavado = 1;
int menuSensores = 1;

int menuPrincipalSeleccionado = true;
int menuRecirculacionSeleccionado = false;
int menuInyeccionSeleccionado = false;
int menuLavadoSeleccionado = false;
int menuSensoresSeleccionado = false;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

long tiempoRecirculacion = 0;
long tiempoPurgaR = 0;
long tiempoPurgaH = 0;
long tiempoLavado = 0;
long tiempoSensado = 0;
long tRestante = 0;
int tiempoEnMinutos = 0;

String operacion;

//valvulas
int RR =19;  //RECIRCULAR RESINA
int RH =2;  //18 //RECIRCULAR HARDENER
int BHA = 32;  //BOMBA HARDENER ALTA
int BHB =33; //BOMBA HARDENER BAJA
int BRB =25;  //BOMBA RESINA BAJA
int BRA =26; //BOMBA RESINA ALTA
int SR =27;  //SALIDA RESINA
int SH =14;  //SALIDA HARDNER
int SA =12;  //SALIDA AIRE
int BL=13;  //BOMBA AGUA

void setup (){

  pinMode(pinSDA, OUTPUT);
  pinMode(pinSCL, OUTPUT);

  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);
  pinMode(boton4, INPUT_PULLUP);
  //valvulas
  pinMode(RR, OUTPUT);
  pinMode(RH, OUTPUT);
  pinMode(BHA, OUTPUT);
  pinMode(BHB, OUTPUT);
  pinMode(BRB, OUTPUT);
  pinMode(BRA, OUTPUT);
  pinMode(SR, OUTPUT);
  pinMode(SH, OUTPUT);
  pinMode(SA, OUTPUT);
  pinMode(BL, OUTPUT);    
  pinMode(trigPin1, OUTPUT);
  //pinMode(trigPin2, OUTPUT);
  //pinMode(trigPin3, OUTPUT);
  pinMode(echoPin1, INPUT);
  //pinMode(echoPin2, INPUT);
  //pinMode(echoPin3, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  updateMenu();

  Serial.begin(9600);
}

/////FUNCIONES ON/OFF//////////////////////////////////////////////////////////////////////////////////////////////////
void boton4encender(){
  digitalWrite(BHA,HIGH);
  digitalWrite(BRA,HIGH);
  digitalWrite(SR,HIGH);
  digitalWrite(SH,HIGH);
  digitalWrite(SA,HIGH);  
  valorBoton4 = true;
}
void boton4apagar(){
  digitalWrite(BHA,LOW);
  digitalWrite(BRA,LOW);
  digitalWrite(SR,LOW);
  digitalWrite(SH,LOW);
  digitalWrite(SA,LOW);  
  valorBoton4 = false;
}
void recirculacionEncender(){
  digitalWrite(BRB,HIGH);
  digitalWrite(BHB,HIGH);
  digitalWrite(RR,HIGH); 
  digitalWrite(RH,HIGH);
}
void recirculacionApagar(){
  digitalWrite(BRB,LOW);
  digitalWrite(BHB,LOW);
  digitalWrite(RR,LOW); 
  digitalWrite(RH,LOW);
}
void purgarResina(){
  digitalWrite(BRA,HIGH);
  digitalWrite(SR,HIGH);
  digitalWrite(SA,HIGH); 
  }
void purgarHardener(){
  digitalWrite(BHA,HIGH);
  digitalWrite(SH,HIGH);
  digitalWrite(SA,HIGH);  
}
void terminarPurgaResina(){
  digitalWrite(BRA,LOW);
  digitalWrite(SR,LOW);
  digitalWrite(SA,LOW); 
}
void terminarPurgaHardener(){
  digitalWrite(BHA,LOW);
  digitalWrite(SH,LOW);
  digitalWrite(SA,LOW);  
}

void lavadoEncender(){
  digitalWrite(RR,HIGH);
  digitalWrite(RH,HIGH);
  digitalWrite(SR,HIGH);
  digitalWrite(SH,HIGH);
  digitalWrite(SA,HIGH);
  digitalWrite(BL,HIGH);  
}
void lavadoApagar(){
  digitalWrite(RR,LOW);
  digitalWrite(RH,LOW);
  digitalWrite(SR,LOW);
  digitalWrite(SH,LOW);
  digitalWrite(SA,LOW);
  digitalWrite(BL,LOW);  
}

//FUNCION SENSORES ULTRASONICOS//
int medicionUltrasonico(int trigPin, int echoPin, String x){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duracion = pulseIn(echoPin, HIGH);
  distancia = (duracion * 0.034) / 2;

  float nivelTanque = map(distancia, distanciaSensorAlPiso, distanciaSensorANivelLLeno, 0, 100);
  nivelesTanques= nivelesTanques+ "T"+x+": "+nivelTanque+"\n ";
  delay(5);
  return distancia;
}

//FUNCIONES MENU//
void updateMenu(){
  switch(menuPrincipal){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(">Recirculacion");
      lcd.setCursor(1,1);
      lcd.print("Inyeccion");
      lcd.setCursor(1,2);
      lcd.print("Lavado");
      lcd.setCursor(1,3);
      lcd.print("Sensores");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Recirculacion");
      lcd.setCursor(0,1);
      lcd.print(">Inyeccion");
      lcd.setCursor(1,2);
      lcd.print("Lavado");
      lcd.setCursor(1,3);
      lcd.print("Sensores");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Recirculacion");
      lcd.setCursor(1,1);
      lcd.print("Inyeccion");
      lcd.setCursor(0,2);
      lcd.print(">Lavado");
      lcd.setCursor(1,3);
      lcd.print("Sensores");
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Recirculacion");
      lcd.setCursor(1,1);
      lcd.print("Inyeccion");
      lcd.setCursor(1,2);
      lcd.print("Lavado");
      lcd.setCursor(0,3);
      lcd.print(">Sensores");
      break;            
  }
}

void updateRecirculacion(){
  switch(menuRecirculacion){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(0,1);
      lcd.print(">10 20 30 60");
      lcd.setCursor(0,2);
      lcd.print("minutos");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(1,1);
      lcd.print("10 >20 30 60");
      lcd.setCursor(0,2);
      lcd.print("minutos");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(1,1);
      lcd.print("10 20 >30 60");
      lcd.setCursor(0,2);
      lcd.print("minutos");
      break; 
    case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Recircular por:");
      lcd.setCursor(1,1);
      lcd.print("10 20 30 >60");
      lcd.setCursor(0,2);
      lcd.print("minutos");
      break;       
  }
}

void updateInyeccion(){
    switch(menuInyeccion){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Inyeccion Purgar:");
      lcd.setCursor(0,1);
      lcd.print(">Resina");
      lcd.setCursor(1,2);
      lcd.print("Hardener");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Inyeccion Purgar:");
      lcd.setCursor(1,1);
      lcd.print("Resina");
      lcd.setCursor(0,2);
      lcd.print(">Hardener");
      break;      
  }
}

void updateLavado(){
    int r = random(1,11);
    String x = "minutos";
    if(r == 10){
      x = "minitis";
      } 
    switch(menuLavado){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Opciones Lavado:");
      lcd.setCursor(0,1);
      lcd.print(">5 minutos");
      lcd.setCursor(1,2);
      lcd.print("10 "+x);
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Opciones Lavado:");
      lcd.setCursor(1,1);
      lcd.print("5 minutos");
      lcd.setCursor(0,2);
      lcd.print(">10 "+x);
      break;      
  }
}
void updateSensores(){
    switch(menuSensores){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Opciones Sensores");
      lcd.setCursor(0,1);
      lcd.print(">Tanques");
      lcd.setCursor(1,2);
      lcd.print("Presiones");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("*Opciones Sensores:");
      lcd.setCursor(1,1);
      lcd.print("Tanques");
      lcd.setCursor(0,2);
      lcd.print(">Presiones");
      break;      
  }
}
void inyeccionLCD(int tiempoTranscurrido){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Inyeccion");
  lcd.setCursor(0,1);
  lcd.print(tiempoTranscurrido+" mins");
  }
void mensajeTerminado(int tiempoEnMinutos, int tRestante, String operacion){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(operacion);
  lcd.setCursor(0,1);
  lcd.print(String(tiempoEnMinutos-((tRestante)-1))+" mins");
  delay(3000); 
  }
int terminarOperacion(){
  int lectura3 = !digitalRead(boton3);
  if(lectura3 == HIGH){
  return 0;
  }
  else{
  return 1;  
  }
}

/////FUNCIONES//////////////////////////////////////////////////////////////////////////////////////////////////

void loop(){
  lcd.setCursor(0, 0);
  
  int lectura1 = !digitalRead(boton1);              //LOS VALORES ESTAN INVERTIDOS (!digitalRead(boton1)) POR EL USO DE PULLUPS EN LOS BOTONES
  int lectura2 = !digitalRead(boton2);
  int lectura3 = !digitalRead(boton3);
  int lectura4 = !digitalRead(boton4);
  
  if (lectura1 != ultimoEstadoBoton1 || lectura2 != ultimoEstadoBoton2 || lectura3 != ultimoEstadoBoton3 || lectura4 != ultimoEstadoBoton4 ){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay){

    //CODIGO PARA INICIAR/TERMINAR INYECCION, BOTON4//
    if(lectura4 != estadoBoton4){
      estadoBoton4 = lectura4;
      if (lectura4 == HIGH) {
        if(valorBoton4 == false){
          boton4encender();
          valorBoton4 = true;
          menuSensoresSeleccionado == false;
          menuLavadoSeleccionado = false;
          menuRecirculacionSeleccionado = false;
          menuInyeccionSeleccionado = false;
          menuPrincipalSeleccionado = false;
          inyeccionLCD(millis()/60000);
          }
        else if (valorBoton4 == true) {
          boton4apagar();
          valorBoton4 = false;
          menuPrincipalSeleccionado = true;
          updateMenu();
          }
        }
      }
    //CODIGO PARA REGRESAR AL MENU PRINCIPAL, BOTON3//
    if(lectura3 != estadoBoton3){
      estadoBoton3 = lectura3;
      if (lectura3 == HIGH) {
        if(valorBoton4 == false){
          menuSensoresSeleccionado = false;
          menuLavadoSeleccionado = false;
          menuRecirculacionSeleccionado = false;
          menuInyeccionSeleccionado = false;
          menuPrincipalSeleccionado = true;
          updateMenu();
          }
        }
      }

    //CODIGO PARA SELECCIONAR EN LOS MENUS, BOTON 1//
    if (lectura1 != estadoBoton1){
      estadoBoton1 = lectura1;
      if (lectura1 == HIGH) {
        if(menuPrincipalSeleccionado == true){
          switch(menuPrincipal){
            case 1:
              //RECIRCULACION
              updateRecirculacion();
              menuPrincipalSeleccionado = false;
              menuRecirculacionSeleccionado = true;
              break;
            case 2:
              //INYECCION
              updateInyeccion();
              menuPrincipalSeleccionado = false;
              menuInyeccionSeleccionado = true;
              break;
            case 3:
              //LAVADO
              updateLavado();
              menuPrincipalSeleccionado = false;
              menuLavadoSeleccionado = true;
              break;
            case 4:
              //SENSORES
              updateSensores();
              menuPrincipalSeleccionado = false;
              menuSensoresSeleccionado = true;
              break;
            }
          return;  
          }
          
        //RECIRCULACION  
        if(menuRecirculacionSeleccionado == true){
          bool activado = true;
          operacion = "Recirculacion";
          switch(menuRecirculacion){
            case 1:
            tiempoEnMinutos = 10;                                                               //TIEMPO QUE DURARA LA OPERACION
              tiempoRecirculacion = millis() + tiempoEnMinutos*1000;                           
              recirculacionEncender();                                                          //SE INICIA LA OPERACION
              lcd.clear();
              while(tiempoRecirculacion > millis()){                                             //MIENTRAS NO TERMINE EL TIEMPO SE EJECUTARA ESTE CICLO WHILE     
                tRestante = ((tiempoRecirculacion - millis())/1000)+1;                          //TIEMPO RESTANTE SE DIVIDE ENTRE 60000 PARA mins y 1000 PARA segs
                if(terminarOperacion() == 0){                                                     
                  break;                                                                          //SI SE PRESIONA EL BOTON 3, SE TERMINA LA OPERACION ANTES DEL TIEMPO
                  }
                if(tRestante<10 && activado == true){                                             //SE LIMPIA LA PANTALLA CUANDO SEAN MENOS DE DOS CIFRAS
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));                                                     //SE MUESTRA EL TIEMPO RESTANTE
              }
              recirculacionApagar();                                                             //AL TERMINARSE EL TIEMPO SE LLAMA LA FUNCION PARA CERRAR LAS VALVULAS 
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              mensajeTerminado(tiempoEnMinutos, tRestante, operacion);  
              updateMenu();                                                                      //SE MUESTRA EL MENU PRINCIPAL
              break;
            case 2:
              tiempoEnMinutos = 20;
              tiempoRecirculacion = millis() + tiempoEnMinutos*1000;                                          //20 MINUTOS
              recirculacionEncender();
              lcd.clear();
              while(tiempoRecirculacion > millis()){
                tRestante = ((tiempoRecirculacion - millis())/1000)+1;
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              recirculacionApagar();
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              mensajeTerminado(tiempoEnMinutos, tRestante, operacion); 
              updateMenu();
              break;
            case 3:
              tiempoEnMinutos = 30;
              tiempoRecirculacion = millis() + tiempoEnMinutos*1000;                                        //30 MINUTOS
              recirculacionEncender();
              lcd.clear();
              while(tiempoRecirculacion > millis()){
                tRestante = ((tiempoRecirculacion - millis())/1000)+1;
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              recirculacionApagar();
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              mensajeTerminado(tiempoEnMinutos, tRestante, operacion);
              updateMenu();
              break;
            case 4:
              tiempoEnMinutos = 60;
              tiempoRecirculacion = millis() + tiempoEnMinutos*1000;                                  //60 MINUTOS
              recirculacionEncender();
              lcd.clear();
              while(tiempoRecirculacion > millis()){
                tRestante = ((tiempoRecirculacion - millis())/1000)+1;
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              recirculacionApagar();
              menuRecirculacionSeleccionado = false;
              menuPrincipalSeleccionado = true;
              mensajeTerminado(tiempoEnMinutos, tRestante, operacion); 
              updateMenu();
              break;
            }
            return; 
          }
          
        //INYECCION  
        if(menuInyeccionSeleccionado == true){
          bool activado = true;
          switch(menuInyeccion){
            case 1:
              //purga Resina
              tiempoEnMinutos = 5;
              tiempoPurgaR = millis() + tiempoEnMinutos*10000;                         //15 SEGUNDOS
              purgarResina();
              lcd.clear();
              while(tiempoPurgaR > millis()){
                tRestante = ((tiempoPurgaR - millis())/10000)+1;
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante)+"Res");
              }
              terminarPurgaResina();
              mensajeTerminado(tiempoEnMinutos, tRestante, "Purga Resina"); 
              updateInyeccion();
              break;
            case 2:
              //purga Hardener
              tiempoEnMinutos = 10;
              tiempoPurgaH = millis() + tiempoEnMinutos*10000;                            //15 SEGUNDOS
              purgarHardener();
              lcd.clear();
              while(tiempoPurgaH > millis()){
                tRestante = ((tiempoPurgaH - millis())/10000)+1;
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante)+"Hard");
              }
              terminarPurgaHardener();
              mensajeTerminado(tiempoEnMinutos, tRestante, "Purga Hardener");
              updateInyeccion();
              break;
            }
            return; 
          }
          
        //LAVADO  
        if(menuLavadoSeleccionado == true){
          bool activado = true;
          switch(menuLavado){
            case 1:
              tiempoEnMinutos = 5;
              tiempoLavado = millis() + tiempoEnMinutos*10000;                          //15 SEGUNDOS
              lavadoEncender();
              lcd.clear();
              while(tiempoLavado > millis()){
                tRestante = ((tiempoLavado - millis())/10000)+1;
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              lavadoApagar();
              menuLavadoSeleccionado = false;
              menuPrincipalSeleccionado = true;
              mensajeTerminado(tiempoEnMinutos, tRestante, "Lavado");
              updateMenu();
              break;
            case 2:
              tiempoEnMinutos = 10;
              tiempoLavado = millis() + tiempoEnMinutos*10000;                     //20 SEGUNDOS
              lavadoEncender();
              lcd.clear();
              while(tiempoLavado > millis()){
                tRestante = ((tiempoLavado - millis())/10000)+1;
                if(terminarOperacion() == 0){
                  break;
                  }
                if(tRestante<10 && activado == true){
                  lcd.clear();
                  activado = false;}
                lcd.setCursor(0,0);
                lcd.print(String(tRestante));
              }
              lavadoApagar();
              menuLavadoSeleccionado = false;
              menuPrincipalSeleccionado = true;
              mensajeTerminado(tiempoEnMinutos, tRestante, "Lavado");
              updateMenu();
              break;
            }
            return; 
          }
        //SENSORES
        if(menuSensoresSeleccionado == true){
          int counter = 0;
          switch(menuSensores){
            case 1:
              tiempoSensado = millis() + 600000;                          //15 SEGUNDOS
              lcd.clear();
              while(tiempoSensado > millis()){
                for(int i = 0; i < 50; i++){
                  distancia = medicionUltrasonico(trigPin1, echoPin1, "1");
                  counter+=distancia;
                }
                if(terminarOperacion() == 0){
                  break;
                }
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print(String(counter/50));
                Serial.println(distancia);
                counter = 0;
              }
              menuSensoresSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            case 2:
              tiempoSensado = millis() + 600000;                          //15 SEGUNDOS
              lcd.clear();
              while(tiempoSensado > millis()){
                distancia = medicionUltrasonico(trigPin1, echoPin1, "1");
                if(terminarOperacion() == 0){
                  break;
                  }
                lcd.setCursor(0,0);
                lcd.print(String(distancia));
                Serial.println(distancia);
              }
              menuSensoresSeleccionado = false;
              menuPrincipalSeleccionado = true;
              updateMenu();
              break;
            }
            return; 
          }
      }
    }
    
    //CODIGO PARA MOVERSE EN LOS MENUS, BOTON 2//
    if (lectura2 != estadoBoton2){
      estadoBoton2 = lectura2;
      if(lectura2 == HIGH) {
          if(menuPrincipalSeleccionado == true){
            if(menuPrincipal <= 3){                 //NUMERO DE MENU ITEMS MENOS 1 
              menuPrincipal++;
              updateMenu();

            }
            else{
              menuPrincipal = 1;
              updateMenu();
            }
          }
      if(menuRecirculacionSeleccionado == true){
          if(menuRecirculacion <= 3){
            menuRecirculacion++;
            updateRecirculacion();
            }
          else{
            menuRecirculacion = 1;
            updateRecirculacion();
            }
          }
        if(menuInyeccionSeleccionado == true){
          if(menuInyeccion == 1){
            menuInyeccion++;
            updateInyeccion();
            }
          else{
            menuInyeccion--;
            updateInyeccion();
            }
          }
        if(menuLavadoSeleccionado == true){
          if(menuLavado == 1){
            menuLavado++;
            updateLavado();
            }
          else{
            menuLavado--;
            updateLavado();
            }
          }
         if(menuSensoresSeleccionado == true){
          if(menuSensores == 1){
            menuSensores++;
            updateSensores();
            }
          else{
            menuSensores--;
            updateSensores();
            }
          } 
        }
    }
  }
  nivelesTanques= "Tanques: ";
  ultimoEstadoBoton1 = lectura1;
  ultimoEstadoBoton2 = lectura2;
  ultimoEstadoBoton3 = lectura3;
  ultimoEstadoBoton4 = lectura4;
}
